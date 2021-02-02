 Условия запуска утилиты для сбора профиля и оценок локальности:
1. Описание программно-аппаратной среды:
	1. Платформа: процессор Intel Xeon X5650 @ 2.67GHz
	2. Размер кэш-памяти (L1/L2/L3): 32K/256K/12M
	3. Компилятор: gcc version 4.4.7
	4. Строка компиляции: g++ -O2 -Wall -Wextra -Werror 
2. Описания параметров запусков:
	1. Используемая точность: double (если не указано иное)
	2. Размер "окна": 512
	3. Размер профиля для визуального анализа: < 1 млн. обращений (часто < 100.000)
	4. Размер профиля для анализа на основе оценок daps и cvg: ~ 30 млн. обращений

Сборка бенчмарков через параметры make
make ELEMS=10 LENGTH=16 MODE=3

MODE - в бенчмарках с режимом ( TRIADA TRANSP)
LENGTH - во всех остальных играет либо размер матрицы, либо связность графа, либо размер массива
ELEMS - играет роль радиуса в Stencil

сборка через bash-скрипт
PROG_NAME=$1 - первым параметром задается бенчмарк 

LAST_MODE=$2 - вторым задается режим(идет цикл от 1 до этого номера)

LENGTH=$3 - для длины массива

ELEMS=$4 - для радиуса в Stencil(например)

пример - bash make.sh gauss 6 10 10

в бенчмарках с openMP есть две версии make - make_omp make_sequential
