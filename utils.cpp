#include "utils.hpp"

long long GetRand_idx(long long lower, long long upper){
	static thread_local mt19937 gen_ind(time(0));
	uniform_int_distribution<long long> f_ind(lower, upper);

    return f_ind(gen_ind);
}