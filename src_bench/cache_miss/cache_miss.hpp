#pragma once

#include <string>

struct cache_miss_best_result{
	int cache_level;
	std::string operation;
	std::string data_type;
	int num_sum;
	int total_sum;
	int threads;
	long double time;
	long long counter_value;
	long double bench_value;
};

template<typename data_type>
void cache_test(data_type** A, char d_type, int num_sum_per_oper, long long num_sum, long long segment);

template <typename T>
void cache_miss_bench(int cache_lv, char op, char d_type, int num_sum, int iter);

cache_miss_best_result cache_miss (int lvl, char oper, int iter, long long time_minutes);

