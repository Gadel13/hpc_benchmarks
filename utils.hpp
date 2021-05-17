#pragma once

#include <fstream>
#include <sys/time.h>
#include <type_traits>
#include <random>

using namespace std;

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
