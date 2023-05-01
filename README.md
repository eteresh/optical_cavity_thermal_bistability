## Зависимости
Код использует библиотеку линейной алгебры [armadillo](http://arma.sourceforge.net),
данные сохраняются в формате [hdf5](https://www.hdfgroup.org/solutions/hdf5/).
(Также для отображения прогресса симуляций используется библиотека [tqdm](https://github.com/aminnj/cpptqdm/blob/master/tqdm.h), для удобства файл `tqdm.h` был скопирован из репозитория автора [aminnj](https://github.com/aminnj))

### Установка зависимостей в Ubuntu
```
sudo apt install libarmadillo-dev libhdf5-dev
```

### Установка зависимостей в Arch linux
1. Установить библиотеку `hdf5`:
```
sudo pacman --sync hdf5
```

2. Установить библиотеку `armadillo` стандартными средствами из AUR-репозитория:
[https://aur.archlinux.org/packages/armadillo](https://aur.archlinux.org/packages/armadillo)

## Сборка
После установки зависимостей достаточно выполнить:
```
make clean
make
make install
```

## Запуск
Исполняемый скрипт называется `cavity_scan`, который принимает на вход 4 позиционных аргумента:
- первый аргумент: `beta`, влияние теплового расширения зеркал на сдвиг частоты,
- второй аргумент: `x0`, частота в полуширинах исходного контура, проходимая за время температурного отклика,
- третий аргумент: `simulation_iterations`, число итераций симуляции.
- четвертый аргумент: `dest_dir`, директория, в которую будут сохранятся файлы

Пример запуска:
```
./cavity_scan 6.0 0.1 100000 ./data
```
Пример визуализации результатов можно посмотреть в ноутбуке [EDA.ipynb](./EDA.ipynb).
