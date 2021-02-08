сборка через bash-скрипт

пока убрал сборку последовательной версии, добавлю чуть позже

ИНСТРУКЦИИ ПО ЗАПУСКУ из директории benchmarks

bash make_omp.sh --prog=triada --length=2000000 --compiler=g++ --threads=48 --radius=10 --lower_bound=4 --higher_bound=4 --no_run=false

--prog - название директории с бенчмарком

--length - бывший параметр N в бенчмарках

--radius -пока нужен для радиуса в stencil

--lower_bound --right_bound - диапазон изменения MODE за один вызов скрипта

--no_run - true - только сборка приложения, false - запуск
