сборка через bash-скрипт

пока убрал сборку последовательной версии, добавлю чуть позже

ИНСТРУКЦИИ ПО ЗАПУСКУ из директории benchmarks_new

bash make_omp.sh --prog=triada --length=2000000 --compiler=g++ --threads=48 --radius=10 --lower_bound=4 --higher_bound=4 --no_run=false

--prog - название директории с бенчмарком

--length - бывший параметр N в бенчмарках

--compiler - используемый компилятор (команда вызова соответствующего компилятора из командной строки)

--threads - число потоков, явно экспортируемых в переменную окружения перед запуском бенчмарка

--radius -пока нужен для радиуса в stencil

--lower_bound --higher_bound - диапазон (оба краевых значения входят в интервал) изменения MODE за один вызов скрипта

--no_run - true - только сборка приложения, false - запуск

MODE в бенчмарках:

для matrix_mult доступны 0-5 режима умножения матриц, отличающиеся порядком вложенных циклов

для triada доступны 1 - 16 типы операций над массивами данных,  в DEBUG версии пока использовать только первые 4

для matrix_transp доступны 0-3 методы транспонирования, 0-1 - обычный, 2-3 - блочный