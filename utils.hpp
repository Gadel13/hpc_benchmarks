#pragma once

#include <fstream>
#include <sys/time.h>
#include <type_traits>
#include <random>

using namespace std;

//theoretical values

// uops/sec (millions)
#define theoretical_load 86000.8 // uops/sec (millions)
#define theoretical_store 43000.4 // uops/sec (millions)

// miss/sec (millions)
#define theoretical_l1_miss_load 9963.636363
#define theoretical_l1_miss_store 5381.818181
#define theoretical_l2_miss_load 3364.705882
#define theoretical_l2_miss_store 1982.352941
#define theoretical_l3_miss_load 1118.306304
#define theoretical_l3_miss_store 1118.306304

// KB
#define theoretical_memfree 64*1024

// Mbit/sec
#define theoretical_mpi_bw 54.54*1024
#define theoretical_fs_write 54.54*1024
#define theoretical_fs_read 54.54*1024


//all sizes in Bytes
#define L1size 32*1024 //per thread
#define L2size 256*1024 //per thread
#define L3size 35*1024*1024

#define RAM_size (long long)64*1024*1024*1024
#define line_size 64 //Bytes

#define core_num 14

#define LOGS 0


template<class T>
using uniform_distribution = 
typename std::conditional<
    std::is_floating_point<T>::value,
    std::uniform_real_distribution<T>,
    typename std::conditional<
        std::is_integral<T>::value,
        std::uniform_int_distribution<T>,
        void
    >::type
>::type;

template <class T>
T GetRand(T lower, T upper){
    static thread_local std::mt19937_64 mt(std::random_device{}());
    uniform_distribution<T> dist(lower,upper);

    return dist(mt);
}

long long GetRand_idx(long long lower, long long upper);
