#include "neslib.h"
#include "zanac.h"

// Y=SET_COLORED_TITLE_SCREEN: Multicolored elements on the title screen
// Y=SET_COLORED_ENDING_SCREEN: Multicolored elements on the Ending screen
void set_colored_attribute_elements(int Y) 			// ROM:E399
{
	static uint8_t data[] = { 			// byte_E3B8
		// +0x0: Multicolored elements on the title screen
		0xC2, 0, 0, 0xAA, 0, 0, 0x80,
		0xCA, 0, 0x80, 0xD2, 0xFF, 0xFF, 0xFF, 0xFF, 0x80,
		0xE5, 0xA5, 0xA5, 0x80,
		0xEB, 0xA0, 0xA0, 0x80, 0
		// +0x19: Multicolored elements on the Ending screen
		0xCA, 0x5A, 0x52, 0x50, 0x50, 0x80,
		0xD3, 0x0A, 0x0A, 0x80,
		0xD8, 0, 0, 0, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0x80,
		0xE8, 0xFF, 0xFF, 0xFF, 0xFF, 0x50, 0x50, 0x80, 0
	};

	while (1) {
		uint8_t value = data[Y];	// Reading a value from an array
		if (value == 0) {		// If the value is 0, terminate the function
			return;
		}

		PPU_ADDRESS(0x23);		// Write the high byte of the PPU address
		PPU_ADDRESS(value);		// Write the low byte of the PPU address
		Y++;

		while (1) {
			value = data[Y];	// Reading the next value from the array
			Y++;

			if (value == 0x80) {	// If the value is 0x80, exit the inner loop
				break;
			}

			PPU_DATA(value);	// Writing the value to the PPU
		}
	}
}