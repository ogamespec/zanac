# напиши на питоне программу которая разбивает входной файл prg.bin (NES UNROM) на банки по 16 Кбайт и называет полученные файлы bank0.bin, bank1.bin итд.

# ChatGPT
# Конечно! Вот пример программы на Python, ...

import os

def split_nes_banks(input_file, bank_size=16 * 1024):
    # Проверяем, существует ли входной файл
    if not os.path.isfile(input_file):
        print(f"Файл {input_file} не найден.")
        return

    # Открываем входной файл для чтения в бинарном режиме
    with open(input_file, 'rb') as f:
        # Читаем все содержимое файла
        data = f.read()
    
    # Определяем количество банков
    num_banks = (len(data) + bank_size - 1) // bank_size  # Округляем вверх

    # Разбиваем данные на банки и сохраняем их в отдельные файлы
    for i in range(num_banks):
        bank_data = data[i * bank_size:(i + 1) * bank_size]
        # Создаем имя файла для банка
        bank_file_name = f"bank{i}.bin"
        
        # Записываем данные в файл
        with open(bank_file_name, 'wb') as bank_file:
            bank_file.write(bank_data)

        print(f"Создан файл: {bank_file_name}, размер: {len(bank_data)} байт")

if __name__ == "__main__":
    input_filename = "prg.bin"
    split_nes_banks(input_filename)