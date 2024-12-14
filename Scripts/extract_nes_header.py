# напиши на питоне скрипт, который вырезает первые 16 байт из файла (NES Header) и сохраняет отдельно nes_header.bin и prg.bin

# ChatGPT
# Конечно! Вот пример Python-скрипта, ...

def extract_nes_header(input_file):
    with open(input_file, 'rb') as f:
        # Читаем первые 16 байт (NES Header)
        header = f.read(16)
        
        # Читаем оставшуюся часть файла
        prg_data = f.read()
    
    # Сохраняем NES Header в отдельный файл
    with open('nes_header.bin', 'wb') as header_file:
        header_file.write(header)
    
    # Сохраняем оставшиеся данные в prg.bin
    with open('prg.bin', 'wb') as prg_file:
        prg_file.write(prg_data)

if __name__ == '__main__':
    input_file = 'zanac.nes'  # Замените на имя вашего файла
    extract_nes_header(input_file)