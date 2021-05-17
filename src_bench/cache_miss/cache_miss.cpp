#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <papi.h>
#include <random>
#include <chrono>
#include <limits>
#include <omp.h>
#include <string>
#include <pthread.h>


#include <vector>

#include <sys/time.h>

#include "cache_miss.hpp"
#include "../../utils.hpp"

using namespace std;

long long datasize_cache_miss;
long long datasize_per_thread_cache_miss;

#define thread_num omp_get_max_threads()

int cache_lv; //1 2 3
char op_cache_miss; //s - store l - load t - total(load/store)

cache_miss_best_result best_cache_miss;
ofstream LOG_CACHE_MISS;


#define NUM_EVENTS 3

#define fiMin 0
#define fiMax datasize_per_thread_cache_miss-1

template<typename data_type>
void cache_test(data_type** A, char d_type, int num_sum_per_oper, long long num_sum) {
	// omp_set_num_threads(thread_num);
	if(LOGS)
		LOG_CACHE_MISS << "___Start test___" << endl;

	int* EventSet = new int[thread_num];

	PAPI_library_init(PAPI_VER_CURRENT);

	if (PAPI_thread_init( (unsigned long (*)(void))pthread_self ) != PAPI_OK)
		LOG_CACHE_MISS << "thread PAPI ERROR" << endl;


	vector<data_type> sum(thread_num);
	long long imax = num_sum_per_oper*num_sum;

	long long ** tmpi = new long long* [thread_num];
	#pragma omp parallel
	{
		int th = omp_get_thread_num();
		tmpi[th] = new long long [imax];
		sum[th] = 0;
		EventSet[th] = PAPI_NULL;
		for (long long i = 0; i < imax; i++){
			tmpi[th][i] = GetRand_idx(0, datasize_per_thread_cache_miss-1);
		}
	}

	long double begin[thread_num], end[thread_num];
	long long tot_L1 = 0;
	long long tot_L2 = 0;
	long long tot_L3 = 0;
	int cur_thr;

	// auto begin = std::chrono::high_resolution_clock::now();
	if(LOGS)
		LOG_CACHE_MISS << "start counting" << endl;
	#pragma omp parallel private (cur_thr)
	{
		cur_thr = omp_get_thread_num();
		long long values[NUM_EVENTS] = {0, 0, 0};

		if(PAPI_register_thread() != PAPI_OK)
            LOG_CACHE_MISS << "register thread PAPI ERROR" << endl;

	    EventSet[cur_thr] = PAPI_NULL;
		if ( PAPI_create_eventset( &( EventSet[cur_thr] ) )  != PAPI_OK )
			LOG_CACHE_MISS << "create EventSet PAPI ERROR" << endl;


		if(op_cache_miss == 's') {
			if (PAPI_add_event(EventSet[cur_thr], PAPI_L1_STM) != PAPI_OK)
		    	LOG_CACHE_MISS << "1add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L2_STM) != PAPI_OK)
		    	LOG_CACHE_MISS << "2add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L3_TCM) != PAPI_OK)
		    	LOG_CACHE_MISS << "3add event PAPI ERROR" << endl;
		} else if(op_cache_miss == 'l') {
			if (PAPI_add_event(EventSet[cur_thr], PAPI_L1_LDM) != PAPI_OK)
		    	LOG_CACHE_MISS << "1add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L2_LDM) != PAPI_OK)
		    	LOG_CACHE_MISS << "2add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L3_TCM) != PAPI_OK)
		    	LOG_CACHE_MISS << "3add event PAPI ERROR" << endl;
		} else if(op_cache_miss == 't') {
			if (PAPI_add_event(EventSet[cur_thr], PAPI_L1_TCM) != PAPI_OK)
		    	LOG_CACHE_MISS << "1add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L2_TCM) != PAPI_OK)
		    	LOG_CACHE_MISS << "2add event PAPI ERROR" << endl;

		    if (PAPI_add_event(EventSet[cur_thr], PAPI_L3_TCM) != PAPI_OK)
		    	LOG_CACHE_MISS << "3add event PAPI ERROR" << endl;
		}

		if (PAPI_start(EventSet[cur_thr]) != PAPI_OK)
			LOG_CACHE_MISS << "start PAPI ERROR" << endl;

		#pragma omp barrier

		if(num_sum_per_oper == 1) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 2) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 3) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l')  {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 4) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l')  {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 5) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l')  {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 6) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
					A[cur_thr][tmpi[cur_thr][i+5]] = i+5;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
					A[cur_thr][tmpi[cur_thr][i+15]] = A[cur_thr][tmpi[cur_thr][i+16]] + A[cur_thr][tmpi[cur_thr][i+17]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 7) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
					A[cur_thr][tmpi[cur_thr][i+5]] = i+5;
					A[cur_thr][tmpi[cur_thr][i+6]] = i+6;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]] + A[cur_thr][tmpi[cur_thr][i+6]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
					A[cur_thr][tmpi[cur_thr][i+15]] = A[cur_thr][tmpi[cur_thr][i+16]] + A[cur_thr][tmpi[cur_thr][i+17]];
					A[cur_thr][tmpi[cur_thr][i+18]] = A[cur_thr][tmpi[cur_thr][i+19]] + A[cur_thr][tmpi[cur_thr][i+20]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 8) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
					A[cur_thr][tmpi[cur_thr][i+5]] = i+5;
					A[cur_thr][tmpi[cur_thr][i+6]] = i+6;
					A[cur_thr][tmpi[cur_thr][i+7]] = i+7;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]] + A[cur_thr][tmpi[cur_thr][i+6]] + A[cur_thr][tmpi[cur_thr][i+7]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
					A[cur_thr][tmpi[cur_thr][i+15]] = A[cur_thr][tmpi[cur_thr][i+16]] + A[cur_thr][tmpi[cur_thr][i+17]];
					A[cur_thr][tmpi[cur_thr][i+18]] = A[cur_thr][tmpi[cur_thr][i+19]] + A[cur_thr][tmpi[cur_thr][i+20]];
					A[cur_thr][tmpi[cur_thr][i+21]] = A[cur_thr][tmpi[cur_thr][i+22]] + A[cur_thr][tmpi[cur_thr][i+23]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 9) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
					A[cur_thr][tmpi[cur_thr][i+5]] = i+5;
					A[cur_thr][tmpi[cur_thr][i+6]] = i+6;
					A[cur_thr][tmpi[cur_thr][i+7]] = i+7;
					A[cur_thr][tmpi[cur_thr][i+8]] = i+8;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]] + A[cur_thr][tmpi[cur_thr][i+6]] + A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
					A[cur_thr][tmpi[cur_thr][i+15]] = A[cur_thr][tmpi[cur_thr][i+16]] + A[cur_thr][tmpi[cur_thr][i+17]];
					A[cur_thr][tmpi[cur_thr][i+18]] = A[cur_thr][tmpi[cur_thr][i+19]] + A[cur_thr][tmpi[cur_thr][i+20]];
					A[cur_thr][tmpi[cur_thr][i+21]] = A[cur_thr][tmpi[cur_thr][i+22]] + A[cur_thr][tmpi[cur_thr][i+23]];
					A[cur_thr][tmpi[cur_thr][i+24]] = A[cur_thr][tmpi[cur_thr][i+25]] + A[cur_thr][tmpi[cur_thr][i+26]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 10) {

			if(op_cache_miss == 's') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[cur_thr][tmpi[cur_thr][i]] = i;
					A[cur_thr][tmpi[cur_thr][i+1]] = i+1;
					A[cur_thr][tmpi[cur_thr][i+2]] = i+2;
					A[cur_thr][tmpi[cur_thr][i+3]] = i+3;
					A[cur_thr][tmpi[cur_thr][i+4]] = i+4;
					A[cur_thr][tmpi[cur_thr][i+5]] = i+5;
					A[cur_thr][tmpi[cur_thr][i+6]] = i+6;
					A[cur_thr][tmpi[cur_thr][i+7]] = i+7;
					A[cur_thr][tmpi[cur_thr][i+8]] = i+8;
					A[cur_thr][tmpi[cur_thr][i+9]] = i+9;
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} else if(op_cache_miss == 'l') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum[cur_thr] += A[cur_thr][tmpi[cur_thr][i]] + A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]] + A[cur_thr][tmpi[cur_thr][i+3]] + A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]] + A[cur_thr][tmpi[cur_thr][i+6]] + A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]] + A[cur_thr][tmpi[cur_thr][i+9]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			} if(op_cache_miss == 't') {
				begin[cur_thr] = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper*3) {
					A[cur_thr][tmpi[cur_thr][i]] = A[cur_thr][tmpi[cur_thr][i+1]] + A[cur_thr][tmpi[cur_thr][i+2]];
					A[cur_thr][tmpi[cur_thr][i+3]] = A[cur_thr][tmpi[cur_thr][i+4]] + A[cur_thr][tmpi[cur_thr][i+5]];
					A[cur_thr][tmpi[cur_thr][i+6]] = A[cur_thr][tmpi[cur_thr][i+7]] + A[cur_thr][tmpi[cur_thr][i+8]];
					A[cur_thr][tmpi[cur_thr][i+9]] = A[cur_thr][tmpi[cur_thr][i+10]] + A[cur_thr][tmpi[cur_thr][i+11]];
					A[cur_thr][tmpi[cur_thr][i+12]] = A[cur_thr][tmpi[cur_thr][i+13]] + A[cur_thr][tmpi[cur_thr][i+14]];
					A[cur_thr][tmpi[cur_thr][i+15]] = A[cur_thr][tmpi[cur_thr][i+16]] + A[cur_thr][tmpi[cur_thr][i+17]];
					A[cur_thr][tmpi[cur_thr][i+18]] = A[cur_thr][tmpi[cur_thr][i+19]] + A[cur_thr][tmpi[cur_thr][i+20]];
					A[cur_thr][tmpi[cur_thr][i+21]] = A[cur_thr][tmpi[cur_thr][i+22]] + A[cur_thr][tmpi[cur_thr][i+23]];
					A[cur_thr][tmpi[cur_thr][i+24]] = A[cur_thr][tmpi[cur_thr][i+25]] + A[cur_thr][tmpi[cur_thr][i+26]];
					A[cur_thr][tmpi[cur_thr][i+27]] = A[cur_thr][tmpi[cur_thr][i+28]] + A[cur_thr][tmpi[cur_thr][i+29]];
				}
				end[cur_thr] = omp_get_wtime() * 1000000;
			}
		}

		if (PAPI_stop(EventSet[cur_thr], values) != PAPI_OK)
			LOG_CACHE_MISS << "stop PAPI ERROR" << endl;

		#pragma omp critical
		{
			tot_L1 += values[0];
			tot_L2 += values[1];
			tot_L3 += values[2];
		}

		if( PAPI_cleanup_eventset(EventSet[cur_thr]) != PAPI_OK) 
			LOG_CACHE_MISS << "cleanup eventset PAPI ERROR" << endl;

		if(PAPI_destroy_eventset(&EventSet[cur_thr]) != PAPI_OK) 
			LOG_CACHE_MISS << "destroy eventset PAPI ERROR" << endl;

		if(PAPI_unregister_thread() != PAPI_OK)
        	LOG_CACHE_MISS << "unregister thread PAPI ERROR" << endl;
	}
	if(LOGS)
		LOG_CACHE_MISS <<"stop counting" << endl;
	// auto end = std::chrono::high_resolution_clock::now();


	// auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();

	long double time = 0.0;

	data_type total_sum = 0;
	for(int th = 0; th < thread_num; th++){

		// long double tmp_time = (end[th].tv_sec  - begin[th].tv_sec) * 1000000 + end[th].tv_usec - begin[th].tv_usec;
		long double tmp_time = end[th] - begin[th]; 
		if(time < tmp_time){
			time = tmp_time;
		}
		total_sum += sum[th];
	}

	string filename;
	if (op_cache_miss == 's') {
		filename = "store_lom2_ResultOMP_ver2_";
	} else if (op_cache_miss == 'l') {
		filename = "load_lom2_ResultOMP_ver2_";
	} if (op_cache_miss == 't') {
		filename = "total_lom2_ResultOMP_ver2_";
	}
	filename += to_string(cache_lv);
	filename += "lvl_";
	filename += to_string(thread_num);
	filename += "thr.csv";

	if(cache_lv == 1 && (long double)tot_L1/(time) > best_cache_miss.bench_value) {
		best_cache_miss.data_type = "double";
		if(d_type == 'i'){
			best_cache_miss.data_type = "int";
		}
		if(d_type == 'c'){
			best_cache_miss.data_type = "char";
		}
		best_cache_miss.num_sum = num_sum_per_oper;
		best_cache_miss.total_sum = num_sum*num_sum_per_oper;
		best_cache_miss.threads = thread_num;
		best_cache_miss.time = time;
		best_cache_miss.counter_value = tot_L1;
		best_cache_miss.bench_value = (long double)tot_L1/(time);
	}
	if(cache_lv == 2 && (long double)tot_L2/(time) > best_cache_miss.bench_value) {
		best_cache_miss.data_type = "double";
		if(d_type == 'i'){
			best_cache_miss.data_type = "int";
		}
		if(d_type == 'c'){
			best_cache_miss.data_type = "char";
		}
		best_cache_miss.num_sum = num_sum_per_oper;
		best_cache_miss.total_sum = num_sum*num_sum_per_oper;
		best_cache_miss.threads = thread_num;
		best_cache_miss.time = time;
		best_cache_miss.counter_value = tot_L2;
		best_cache_miss.bench_value = (long double)tot_L2/(time);
	}
	if(cache_lv == 3 && (long double)tot_L3/(time) > best_cache_miss.bench_value) {
		best_cache_miss.data_type = "double";
		if(d_type == 'i'){
			best_cache_miss.data_type = "int";
		}
		if(d_type == 'c'){
			best_cache_miss.data_type = "char";
		}
		best_cache_miss.num_sum = num_sum_per_oper;
		best_cache_miss.total_sum = num_sum*num_sum_per_oper;
		best_cache_miss.threads = thread_num;
		best_cache_miss.time = time;
		best_cache_miss.counter_value = tot_L3;
		best_cache_miss.bench_value = (long double)tot_L3/(time);
	}

	if(LOGS){
		ofstream REZ(filename, ios::in|ios::app);
		REZ << cache_lv << ";" << d_type << ";"  << total_sum << ";" << datasize_cache_miss << ";" << num_sum*num_sum_per_oper << ";" << thread_num << ";" << num_sum << ";" << num_sum_per_oper << ';' << time << ';'  << tot_L1 << ';' << tot_L2 << ';' <<  tot_L3 << ';' << (long double)tot_L1/(time) << ';' << (long double)tot_L2/(time) << ';' << (long double)tot_L3/(time) << endl;
		REZ.close();
	}

	for(int th = 0; th < thread_num; th++) {
		delete[] tmpi[th];
	}
	delete[] tmpi;

	return;

}

template <typename T>
void cache_miss_bench(T** data, int cache_lv, char op_cache_miss, char d_type, int num_sum, int iter) {

	auto fMin = numeric_limits<T>::min();
	auto fMax = numeric_limits<T>::max();

	for (int i = 0; i < iter; ++i) {
		for(int k = 1024; k < 1024*1024; k *= 2) {
			double tmp1 = 0;
			for (int th = 0; th < thread_num; th++) {
				tmp1 += data[th][GetRand_idx(fiMin, fiMax)]/1000000;
			}
			if(LOGS)
				LOG_CACHE_MISS << tmp1 << "  " << endl;

			if(cache_lv == 1){
				cache_test<T>(data, d_type, num_sum, k);
			} else if(cache_lv == 2){
				cache_test<T>(data, d_type, num_sum, k);
			} else {
				cache_test<T>(data, d_type, num_sum, k);
			}

			double tmp2 = 0;
			for (int th = 0; th < thread_num; th++) {
				tmp2 += data[th][GetRand_idx(fiMin, fiMax)]/1000000;
			}
			if(LOGS)
				LOG_CACHE_MISS << tmp2 << endl;
		}
	}
}


cache_miss_best_result cache_miss (int lvl, char oper, int iter, long long time_minutes) { // <Lx x - Cache level> <s/l - store load test> <num_iter>
	LOG_CACHE_MISS.open("cache_miss_log.txt", ios::out|ios::app);

	best_cache_miss.cache_level = lvl;
	best_cache_miss.operation = "load";
	if(oper == 's'){
		best_cache_miss.operation = "store";
	}
	best_cache_miss.data_type = ' ';
	best_cache_miss.num_sum = 0;
	best_cache_miss.total_sum = 0;
	best_cache_miss.threads = thread_num;
	best_cache_miss.time = 0;
	best_cache_miss.counter_value = 0;
	best_cache_miss.bench_value = 0;

	cache_lv = lvl;
	op_cache_miss = oper;

	long long RAM = 1024*1024*1024;
	if(cache_lv == 3 && RAM_size < RAM){
		RAM = RAM_size;
	}
	if(cache_lv == 1){
		RAM = L2size*thread_num;
	}
	if(cache_lv == 2){
		RAM = L3size;
	}

	auto start = std::chrono::high_resolution_clock::now();

	while(1){
		//double
		{
			auto fMin_double = numeric_limits<double>::min();
			auto fMax_double = numeric_limits<double>::max();

			datasize_cache_miss = RAM/(sizeof(double));
			datasize_per_thread_cache_miss = datasize_cache_miss/thread_num;


			double ** data_double = new double* [thread_num];

			#pragma omp parallel
			{	
				int th = omp_get_thread_num();

				data_double[th] = new double [datasize_per_thread_cache_miss];
				for (long long i = 0; i < datasize_per_thread_cache_miss; i++){
					data_double[th][i] = GetRand<double>(fMin_double, fMax_double);
				}

			}
			for(int num_sum = 1; num_sum <= 10; num_sum++){
				cache_miss_bench<double>(data_double, cache_lv, op_cache_miss, 'd', num_sum, iter);
				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = end - start;
				if(time_minutes >= 0 && elapsed.count() >= time_minutes*60){
				    break;
				}
			}
			for(int th = 0; th < thread_num; th++){
				delete[] data_double[th];
			}
			delete[] data_double;
		}

		//int
		{
			auto fMin_int = numeric_limits<int>::min();
			auto fMax_int = numeric_limits<int>::max();

			datasize_cache_miss = RAM/(sizeof(int));
			datasize_per_thread_cache_miss = datasize_cache_miss/thread_num;

			int ** data_int = new int* [thread_num];
			#pragma omp parallel
			{	
				int th = omp_get_thread_num();

				data_int[th] = new int [datasize_per_thread_cache_miss];
				for (long long i = 0; i < datasize_per_thread_cache_miss; i++){
					data_int[th][i] = GetRand<int>(fMin_int, fMax_int);
				}

			}
			for(int num_sum = 1; num_sum <= 10; num_sum++){
				cache_miss_bench<int>(data_int, cache_lv, op_cache_miss, 'i', num_sum, iter);
				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = end - start;
				if(time_minutes >= 0 && elapsed.count() >= time_minutes*60){
				    break;
				}
			}
			
			for(int th = 0; th < thread_num; th++){
				delete[] data_int[th];
			}
			delete[] data_int;
		}


		//char
		{
			auto fMin_char = numeric_limits<char>::min();
			auto fMax_char = numeric_limits<char>::max();

			datasize_cache_miss = RAM/(sizeof(char));
			datasize_per_thread_cache_miss = datasize_cache_miss/thread_num;

			char ** data_char = new char* [thread_num];
			#pragma omp parallel
			{	
				int th = omp_get_thread_num();

				data_char[th] = new char [datasize_per_thread_cache_miss];
				for (long long i = 0; i < datasize_per_thread_cache_miss; i++){
					data_char[th][i] = GetRand<char>(fMin_char, fMax_char);
				}

			}
			for(int num_sum = 1; num_sum <= 10; num_sum++){
				cache_miss_bench<char>(data_char, cache_lv, op_cache_miss, 'c', num_sum, iter);
				auto end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = end - start;
				if(time_minutes >= 0 && elapsed.count() >= time_minutes*60){
				    break;
				}
			}	

			for(int th = 0; th < thread_num; th++){
				delete[] data_char[th];
			}
			delete[] data_char;
		}

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		if(elapsed.count() >= time_minutes*60){
		    break;
		}
	}

	LOG_CACHE_MISS.close();
	return best_cache_miss;
}
