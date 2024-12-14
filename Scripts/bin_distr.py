import numpy as np
from PIL import Image
import sys

def binary_to_image(file_path):
    # Читаем бинарный файл
    with open(file_path, 'rb') as f:
        data = f.read()

    # Получаем длину данных
    data_length = len(data)

    # Вычисляем размер стороны квадратного изображения
    side_length = int(np.ceil(np.sqrt(data_length)))

    # Создаем массив для изображения, инициализируем нулями (черный цвет)
    image_data = np.zeros((side_length, side_length), dtype=np.uint8)

    # Заполняем массив значениями из бинарного файла
    for i in range(data_length):
        # Заполняем пиксели значениями из бинарного файла
        image_data[i // side_length, i % side_length] = data[i]

    # Создаем изображение из массива
    image = Image.fromarray(image_data, 'L')

    # Сохраняем изображение в файл
    output_file = file_path + '.png'
    image.save(output_file)
    print(f'Изображение сохранено как {output_file}')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Использование: python script.py <путь_к_бинарному_файлу>")
    else:
        binary_to_image(sys.argv[1])