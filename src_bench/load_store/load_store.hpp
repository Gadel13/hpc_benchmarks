#pragma once

#include <list>
#include <vector>
#include <string>

using namespace std;

struct load_store_best_result{
	string type;
	string operation;
	string data_type;
	int num_sum;
	int total_sum;
	int threads;
	long double time;
	long long counter_value;
	long double bench_value;
};

load_store_best_result load_store (char oper, int iter, long long time_minutes);

template <typename T>
void load_store_bench(char op, char d_type, int num_sum, int iter);

template<typename data_type>
int ld_st_test_norandom(data_type* A, char d_type, int num_sum_per_oper, long long num_sum, int offset);

template<typename data_type>
int ld_st_test_random(data_type* A, char d_type, int num_sum_per_oper, long long num_sum);
