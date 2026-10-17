/* sdl.c - the SDL3 frontend of the Zanac C port (video, audio, keyboard).
 *
 * Built when SDL3 is available (see CMakeLists.txt / build.bat); it replaces
 * frontend/headless.c, which provides the same entry points without a window.
 *
 * The host does not drive the game: the ported Reset handler runs the game's
 * outer loop, and neslib calls frontend_frame() once per completed frame (see
 * the note in frontend.h).  This file therefore only implements:
 *
 *      frontend_init()      create the window, the texture and the audio stream
 *      frontend_frame()     poll events, present the picture, feed the pad,
 *                           pump the audio, pace the run at the NTSC frame rate;
 *                           returns non-zero when the user quits
 *      frontend_shutdown()  release everything
 *
 * Video:  the neslib framebuffer (256x240, 0x00RRGGBB) is streamed into a
 *         texture; F1..F4 select the scale, F11 toggles fullscreen.
 * Audio:  the APU's decimated samples are pushed into an SDL audio stream
 *         (48 kHz, mono, float) once per frame.
 * Input:  arrows/WASD, Z or J = B, X or K = A, Enter = Start,
 *         Space/Backspace = Select, F5 = save state, F7 = load state,
 *         F12 = PPM screenshot, Esc = quit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "frontend.h"
#include "neslib/neslib.h"

static SDL_Window      *s_window;
static SDL_Renderer    *s_renderer;
static SDL_Texture     *s_texture;
static SDL_AudioStream *s_audio;
static bool             s_running;
static int              s_scale = 3;
static Uint64           s_last_counter;
static double           s_accumulator_ns;
static double           s_next_frame_ns;        /* when the next vblank is due */
static double           s_counter_freq;
static long             s_dropped_audio;

/* One NTSC frame: 262 lines * 341 dots / (3 dots per CPU cycle) / 1789772.7 Hz */
#define FRAME_NS (1000000000.0 / 60.0988)

/* The console produces exactly one frame of samples per emulated frame
 * (48000 / 60.0988 = 798.7).  Pacing every emulated frame at the vblank rise
 * keeps that equal to what the sound card plays, which is what stops the audio
 * from drifting behind. */
static void pace_frame(void)
{
    double now, wait_ms;

    if (s_counter_freq <= 0.0) return;
    now = (double)SDL_GetPerformanceCounter() * 1e9 / s_counter_freq;
    if (s_next_frame_ns == 0.0)
        s_next_frame_ns = now;
    s_next_frame_ns += FRAME_NS;

    wait_ms = (s_next_frame_ns - now) / 1e6;
    if (wait_ms > 1.0)
        SDL_Delay((Uint32)(wait_ms - 0.5));
    else if (now - s_next_frame_ns > 250.0)     /* 250 ms behind: resynchronise */
        s_next_frame_ns = now;
}

static void audio_pump(void)
{
    float buffer[1024];
    int n;
    int queued;

    if (!s_audio) return;

    while ((n = neslib_audio_read(buffer, 1024)) > 0)
        SDL_PutAudioStreamData(s_audio, buffer, n * (int)sizeof(float));

    /* Keep the cushion small and bounded.  The emulation produces 48000 samples
     * per emulated second and the card plays 48000 per second of its own clock;
     * the two crystals differ by a fraction of a percent, so without a trim the
     * queue can only drift one way - and anything the device fails to consume
     * (a startup delay, a dragged window) would otherwise stay as a permanent
     * delay.  Above 150 ms the excess is discarded, leaving 60 ms of cushion. */
    queued = SDL_GetAudioStreamQueued(s_audio);
    {
        int target = (int)(FRONTEND_SAMPLE_RATE * 0.060) * (int)sizeof(float);
        int limit  = (int)(FRONTEND_SAMPLE_RATE * 0.150) * (int)sizeof(float);
        if (queued > limit) {
            int excess = queued - target;
            while (excess > 0) {
                int chunk = excess > (int)sizeof(buffer) ? (int)sizeof(buffer) : excess;
                int got = SDL_GetAudioStreamData(s_audio, buffer, chunk);
                if (got <= 0) break;
                excess -= got;
            }
            s_dropped_audio++;
            if (getenv("ZANAC_AUDIO_LOG"))
                fprintf(stderr, "zanac: audio was %.0f ms behind, trimmed\n",
                        1000.0 * queued / (sizeof(float) * FRONTEND_SAMPLE_RATE));
        }
    }
}

static void set_buttons(void)
{
    const bool *keys = SDL_GetKeyboardState(NULL);
    if (!keys) return;
    neslib_set_button(0, NESLIB_BTN_UP,     keys[SDL_SCANCODE_UP]     || keys[SDL_SCANCODE_W]);
    neslib_set_button(0, NESLIB_BTN_DOWN,   keys[SDL_SCANCODE_DOWN]   || keys[SDL_SCANCODE_S]);
    neslib_set_button(0, NESLIB_BTN_LEFT,   keys[SDL_SCANCODE_LEFT]   || keys[SDL_SCANCODE_A]);
    neslib_set_button(0, NESLIB_BTN_RIGHT,  keys[SDL_SCANCODE_RIGHT]  || keys[SDL_SCANCODE_D]);
    neslib_set_button(0, NESLIB_BTN_A,      keys[SDL_SCANCODE_X]      || keys[SDL_SCANCODE_K]);
    neslib_set_button(0, NESLIB_BTN_B,      keys[SDL_SCANCODE_Z]      || keys[SDL_SCANCODE_J]);
    neslib_set_button(0, NESLIB_BTN_START,  keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_1]);
    neslib_set_button(0, NESLIB_BTN_SELECT, keys[SDL_SCANCODE_SPACE]  || keys[SDL_SCANCODE_BACKSPACE]);
}

int frontend_init(void)
{
    SDL_AudioSpec spec;

    s_running = true;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "zanac: SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    s_window = SDL_CreateWindow("Zanac (C port)", 256 * s_scale, 240 * s_scale,
                                SDL_WINDOW_RESIZABLE);
    if (!s_window) {
        fprintf(stderr, "zanac: SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 1;
    }
    s_renderer = SDL_CreateRenderer(s_window, NULL);
    s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_XRGB8888,
                                  SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (!s_renderer || !s_texture) {
        fprintf(stderr, "zanac: SDL renderer/texture failed: %s\n", SDL_GetError());
        return 1;
    }

    memset(&spec, 0, sizeof(spec));
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = (int)FRONTEND_SAMPLE_RATE;
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "512");
    s_audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (s_audio) {
        SDL_SetAudioStreamGain(s_audio, 1.0f);
        SDL_ResumeAudioStreamDevice(s_audio);
    } else {
        fprintf(stderr, "zanac: no audio device (%s)\n", SDL_GetError());
    }

    s_counter_freq = (double)SDL_GetPerformanceFrequency();
    s_last_counter = SDL_GetPerformanceCounter();
    s_accumulator_ns = 0.0;
    s_next_frame_ns = 0.0;
    neslib_set_vblank_hook(pace_frame);       /* pace every emulated frame */
    return 0;
}

int frontend_frame(void)
{
    SDL_Event event;
    int action = FRONTEND_CONTINUE;

    /* ZANAC_AUDIO_LOG=1 prints how far the sound card is behind the emulation,
     * once a second - the diagnostic for the audio pacing. */
    if (getenv("ZANAC_AUDIO_LOG")) {
        static long n;
        if ((n++ % 60) == 0) {
            double queued_ms = s_audio
                ? 1000.0 * SDL_GetAudioStreamQueued(s_audio)
                  / (sizeof(float) * FRONTEND_SAMPLE_RATE) : 0.0;
            fprintf(stderr, "t=%6.2fs emulated frames=%llu queued=%6.1f ms dropped=%ld\n",
                    SDL_GetTicks() / 1000.0,
                    (unsigned long long)neslib_frame_count(), queued_ms, s_dropped_audio);
        }
    }

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            s_running = false;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
            case SDLK_ESCAPE:
                s_running = false;
                break;
            case SDLK_F1: s_scale = 1; SDL_SetWindowSize(s_window, 256, 240); break;
            case SDLK_F2: s_scale = 2; SDL_SetWindowSize(s_window, 512, 480); break;
            case SDLK_F3: s_scale = 3; SDL_SetWindowSize(s_window, 768, 720); break;
            case SDLK_F4: s_scale = 4; SDL_SetWindowSize(s_window, 1024, 960); break;
            case SDLK_F5:
                /* Save/load are reported to main.c, which owns the game-side
                 * state (see frontend.h).  Ignore auto-repeat: holding the key
                 * must not write the file every keyboard repeat. */
                if (!event.key.repeat)
                    action = FRONTEND_SAVE_STATE;
                break;
            case SDLK_F7:
                if (!event.key.repeat)
                    action = FRONTEND_LOAD_STATE;
                break;
            case SDLK_F12: {
                /* F12 writes what is on screen right now; the file name is
                 * printed on stderr so a bug report can point at the frame. */
                static int shot_no;
                char name[64];
                snprintf(name, sizeof(name), "zanac_shot_%03d.ppm", shot_no++);
                frontend_write_ppm(name, neslib_framebuffer(),
                                   NESLIB_SCREEN_W, NESLIB_SCREEN_H);
                fprintf(stderr, "zanac: wrote %s\n", name);
                break;
            }
            case SDLK_F11:
                SDL_SetWindowFullscreen(s_window,
                    (SDL_GetWindowFlags(s_window) & SDL_WINDOW_FULLSCREEN) ? false : true);
                break;
            default:
                break;
            }
        }
    }

    set_buttons();
    audio_pump();

    if (s_texture) {
        SDL_UpdateTexture(s_texture, NULL, neslib_framebuffer(), 256 * 4);
        SDL_RenderClear(s_renderer);
        SDL_RenderTexture(s_renderer, s_texture, NULL, NULL);
        SDL_RenderPresent(s_renderer);
    }

    if (!s_running)
        return FRONTEND_QUIT;
    return action;
}

void frontend_state_loaded(void)
{
    /* The state belongs to a different point in time: the samples already
     * queued for the sound card are from the run that was left, and the next
     * vblank is not due at the old schedule. */
    if (s_audio)
        SDL_ClearAudioStream(s_audio);
    s_next_frame_ns = 0.0;
    s_accumulator_ns = 0.0;
}

void frontend_shutdown(void)
{
    neslib_set_vblank_hook(NULL);
    if (s_audio) {
        SDL_PauseAudioStreamDevice(s_audio);
        SDL_ClearAudioStream(s_audio);
        SDL_DestroyAudioStream(s_audio);
        s_audio = NULL;
    }
    if (s_texture) SDL_DestroyTexture(s_texture);
    if (s_renderer) SDL_DestroyRenderer(s_renderer);
    if (s_window) SDL_DestroyWindow(s_window);
    SDL_Quit();
}
