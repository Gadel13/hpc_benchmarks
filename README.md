# hpc_benchmarks

## Последовательные бенчмарки:
* load µops/sec
* store µops/sec
* l1_miss_load miss/sec
* l1_miss_store miss/sec
* l2_miss_load miss/sec
* l2_miss_store miss/sec
* l3_miss_load miss/sec
* l3_miss_store miss/sec
*memfree

## MPI бенчмарки:
* mpi_bw (osu_bw http://mvapich.cse.ohio-state.edu/benchmarks/)
* fs_write/read (mpi-tile-io https://www.vi4io.org/tools/benchmarks/mpi-tile-io)

Необходимые для бенчмарков характеристики системы необходимо изменить в файле utils.hpp.
MPI бенчмарки запускаются по одному.

## Параметры последовательных бенчмарков:
-h | --help - выводит все доступные параметры и их сначения по умолчанию.

-b | --bench - <бенчмарки> названия бенчмарков (через пробел), которые необходимо запустить. (По умолчанию запустятся все бенчмарки) Пример: --bench l3_miss memfree store. Доступные: load, store, l1_miss, l1_miss_load, l1_miss_store, l2_miss, l2_miss_load, l2_miss_store, l3_miss, l3_miss_load, l3_miss_store, memfree.

-t | --time <кол-во минут> - время работы каждого бенчмарка [1, ∞). Бенчмарк будет запускаться пока не пройдет заданнаое количество минут. (По умолчанию нет)

-o | --outfile <файл> - название файла для вывода результатов. (По умолчанию выводит на экран)

-th | --threads <число потоков> - число OpenMP потоков [1, core_num] (По умолчанию core_num из utils.hpp)

--max <0/1> - запускает тесты несколько раз для разного числа потоков (1 - core_num) для получения максимально возможных значений. (0 - false/ 1 - true; По умолчанию 0) При использовании --max 1 флаг -t не работает.

## Параметры MPI бенчмарков:
-h | --help - выводит все доступные параметры и их сначения по умолчанию.

-b | --bench - <бенчмарк> название бенчмарка, который необходимо запустить. (Обязательный параметр) Пример: --bench mpi_bw. Доступные: mpi_bw, fs_write, fs_read.

-t | --time <кол-во минут> - время работы бенчмарка [1, ∞). Бенчмарк будет запускаться пока не пройдет заданнаое количество минут. (По умолчанию нет)

--max <0/1> - запускает бенчмарк несколько раз для получения максимально возможных значений. (0 - false/ 1 - true; По умолчанию 0) При использовании --max 1 флаг -t не работает.



## Сборка
make

Собираются все бенчмарки. Все последовательные бенчмарки в программе hpc_benchmarks, MPI бенчмарки отдельно.
Для упрощения запусков на СК Ломоносов 2 реализован скрипт run_hpc_benchmarks.sh.

## Пример запуска на СК Ломоносов 2.
module add openmpi/1.8.4-gcc slurm gcc

make

./run_hpc_benchmarks.sh seq -h (выведет параметры запуска последовательных бенчмарков)

./run_hpc_benchmarks.sh seq -b load store memfree l1_miss -t 1 (запусит каждый из бенчмарков load, store, memfree, l1_miss_load, l1_miss_store на 1 минуту)

./run_hpc_benchmarks.sh mpi -h (выведет параметры запуска MPI бенчмарков)

./run_hpc_benchmarks.sh mpi -b mpi_bw (Запустит бенчмарк mpi_bw один раз)
