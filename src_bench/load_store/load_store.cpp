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
#include <type_traits>
#include <list>
#include <iterator>


#include <vector>

#include <sys/time.h>
#include <type_traits>

#include "load_store.hpp"
#include "../../utils.hpp"

#define thread_num omp_get_max_threads()

char op_load_store;
long long datasize_load_store;
long long datasize_per_thread_load_store;

#define NUM_EVENTS 5

#define fiMin 0
#define fiMax datasize_per_thread_load_store-1

load_store_best_result best_load_store;
ofstream LOG_LOAD_STORE;


template<typename data_type>
int ld_st_test_random(data_type* A, char d_type, int num_sum_per_oper, long long num_sum) {
	// omp_set_num_threads(thread_num);
	long long imax = num_sum_per_oper*num_sum;

	if(LOGS)
		LOG_LOAD_STORE << "___Start random test___" << endl;

	int* EventSet = new int[thread_num];

	PAPI_library_init(PAPI_VER_CURRENT);

	if (PAPI_thread_init( (unsigned long (*)(void))pthread_self ) != PAPI_OK)
		LOG_LOAD_STORE << "thread PAPI ERROR" << endl;


	// vector<data_type> sum(thread_num);

	long long ** tmpi = new long long* [thread_num];
	#pragma omp parallel
	{
		int th = omp_get_thread_num();
		tmpi[th] = new long long [imax];
		// sum[th] = 0;
		EventSet[th] = PAPI_NULL;
		for (long long i = 0; i < imax; i++){
			tmpi[th][i] = GetRand_idx(0, fiMax);
		}
	}

	vector<double> begin(thread_num, 0), end(thread_num, 0);

	vector<long long> load(thread_num, 0);
	vector<long long> store(thread_num, 0);
	vector<long long> ls_compl(thread_num, 0);

	vector<long long> load_uops_ret(thread_num, 0);
	vector<long long> store_uops_ret(thread_num, 0);

	int cur_thr;
	long double total_sum = 0;

	if(LOGS)
		LOG_LOAD_STORE << "start random counting" << endl;
	#pragma omp parallel private (cur_thr)
	{
		data_type sum = 0;
		long double start_time, end_time;
		long long thr_off = cur_thr * datasize_per_thread_load_store;
		cur_thr = omp_get_thread_num();
		long long values[NUM_EVENTS] = {0, 0, 0, 0, 0};

		if(PAPI_register_thread() != PAPI_OK)
            LOG_LOAD_STORE << "random test register thread PAPI ERROR" << endl;

	    EventSet[cur_thr] = PAPI_NULL;
		if ( PAPI_create_eventset( &( EventSet[cur_thr] ) )  != PAPI_OK )
			LOG_LOAD_STORE << "random test create EventSet PAPI ERROR" << endl;


		if (PAPI_add_event(EventSet[cur_thr], PAPI_LD_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "random test 1add event PAPI ERROR" << endl;

	    if (PAPI_add_event(EventSet[cur_thr], PAPI_SR_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "random test 2add event PAPI ERROR" << endl;

	    if (PAPI_add_event(EventSet[cur_thr], PAPI_LST_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "random test 3add event PAPI ERROR" << endl;

	    //MEM_UOPS_RETIRED:ALL_LOADS
	   	unsigned int native_load = 1073741827;
	    if (PAPI_add_event(EventSet[cur_thr], native_load) != PAPI_OK)
	    	LOG_LOAD_STORE << "random test 4add event PAPI ERROR" << endl;

	    //MEM_UOPS_RETIRED:ALL_STORES
	    unsigned int native_store = 1073741828;
	    if (PAPI_add_event(EventSet[cur_thr], native_store) != PAPI_OK)
	    	LOG_LOAD_STORE << "random test 5add event PAPI ERROR" << endl;

	    #pragma omp barrier

		if (PAPI_start(EventSet[cur_thr]) != PAPI_OK)
			LOG_LOAD_STORE << "random test start PAPI ERROR" << endl;

		if(num_sum_per_oper == 1) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 2) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 3) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 4) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 5) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 6) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
					A[thr_off + tmpi[cur_thr][i+5]] = i+5;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]] + A[thr_off + tmpi[cur_thr][i+5]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 7) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
					A[thr_off + tmpi[cur_thr][i+5]] = i+5;
					A[thr_off + tmpi[cur_thr][i+6]] = i+6;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]] + A[thr_off + tmpi[cur_thr][i+5]] + A[thr_off + tmpi[cur_thr][i+6]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 8) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
					A[thr_off + tmpi[cur_thr][i+5]] = i+5;
					A[thr_off + tmpi[cur_thr][i+6]] = i+6;
					A[thr_off + tmpi[cur_thr][i+7]] = i+7;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]] + A[thr_off + tmpi[cur_thr][i+5]] + A[thr_off + tmpi[cur_thr][i+6]] + A[thr_off + tmpi[cur_thr][i+7]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 9) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
					A[thr_off + tmpi[cur_thr][i+5]] = i+5;
					A[thr_off + tmpi[cur_thr][i+6]] = i+6;
					A[thr_off + tmpi[cur_thr][i+7]] = i+7;
					A[thr_off + tmpi[cur_thr][i+8]] = i+8;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]] + A[thr_off + tmpi[cur_thr][i+5]] + A[thr_off + tmpi[cur_thr][i+6]] + A[thr_off + tmpi[cur_thr][i+7]] + A[thr_off + tmpi[cur_thr][i+8]];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 10) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + tmpi[cur_thr][i]] = i;
					A[thr_off + tmpi[cur_thr][i+1]] = i+1;
					A[thr_off + tmpi[cur_thr][i+2]] = i+2;
					A[thr_off + tmpi[cur_thr][i+3]] = i+3;
					A[thr_off + tmpi[cur_thr][i+4]] = i+4;
					A[thr_off + tmpi[cur_thr][i+5]] = i+5;
					A[thr_off + tmpi[cur_thr][i+6]] = i+6;
					A[thr_off + tmpi[cur_thr][i+7]] = i+7;
					A[thr_off + tmpi[cur_thr][i+8]] = i+8;
					A[thr_off + tmpi[cur_thr][i+9]] = i+9;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + tmpi[cur_thr][i]] + A[thr_off + tmpi[cur_thr][i+1]] + A[thr_off + tmpi[cur_thr][i+2]] + A[thr_off + tmpi[cur_thr][i+3]] + A[thr_off + tmpi[cur_thr][i+4]] + A[thr_off + tmpi[cur_thr][i+5]] + A[thr_off + tmpi[cur_thr][i+6]] + A[thr_off + tmpi[cur_thr][i+7]] + A[thr_off + tmpi[cur_thr][i+8]] + A[thr_off + tmpi[cur_thr][i+9]];
				}
				end_time = omp_get_wtime() * 1000000;
			} 
		}

		if (PAPI_stop(EventSet[cur_thr], values) != PAPI_OK)
			LOG_LOAD_STORE << "random test stop PAPI ERROR" << endl;

		begin[cur_thr] = start_time;
		end[cur_thr] = end_time;

		load[cur_thr] += values[0];
		store[cur_thr] += values[1];
		ls_compl[cur_thr] += values[2];
		load_uops_ret[cur_thr] += values[3];
		store_uops_ret[cur_thr] += values[4];

		total_sum += sum/std::numeric_limits<data_type>::max();

		if( PAPI_cleanup_eventset(EventSet[cur_thr]) != PAPI_OK) 
			LOG_LOAD_STORE << "random test cleanup eventset PAPI ERROR" << endl;

		if(PAPI_destroy_eventset(&EventSet[cur_thr]) != PAPI_OK) 
			LOG_LOAD_STORE << "random test destroy eventset PAPI ERROR" << endl;

		if(PAPI_unregister_thread() != PAPI_OK)
        	LOG_LOAD_STORE << "random test unregister thread PAPI ERROR" << endl;
	}
	if(LOGS)
		LOG_LOAD_STORE <<"random test stop counting" << endl;

	long long tot_load = 0;
	long long tot_store = 0;
	long long tot_ls_compl = 0;
	long long tot_load_uops_ret = 0;
	long long tot_store_uops_ret = 0;

	long double time = 0;

	// long double total_sum = 0;
	for(int i = 0; i < thread_num; i++) {
		tot_load += load[i];
		tot_store += store[i];
		tot_ls_compl += ls_compl[i];
		tot_load_uops_ret += load_uops_ret[i];
		tot_store_uops_ret += store_uops_ret[i];

		// total_sum += sum[i]/std::numeric_limits<data_type>::max();

		if(end[i] - begin[i] > time){
			time = end[i] - begin[i];
		}
	}


	string filename;
	if (op_load_store == 's') {
		filename = "qqq/store_op_lom2_ResultOMP_ver2_";

		if((long double)tot_store/(time) > best_load_store.bench_value) {
			best_load_store.type = "random";
			best_load_store.data_type = "double";
			if(d_type == 'i'){
				best_load_store.data_type = "int";
			}
			if(d_type == 'c'){
				best_load_store.data_type = "char";
			}
			best_load_store.num_sum = num_sum_per_oper;
			best_load_store.total_sum = num_sum*num_sum_per_oper;
			best_load_store.time = time;
			best_load_store.counter_value = tot_store;
			best_load_store.bench_value = (long double)tot_store/(time);
		}
	} else if (op_load_store == 'l') {
		filename = "qqq/load_op_lom2_ResultOMP_ver2_";

		if((long double)tot_load/(time) > best_load_store.bench_value) {
			best_load_store.type = "random";
			best_load_store.data_type = "double";
			if(d_type == 'i'){
				best_load_store.data_type = "int";
			}
			if(d_type == 'c'){
				best_load_store.data_type = "char";
			}
			best_load_store.num_sum = num_sum_per_oper;
			best_load_store.total_sum = num_sum*num_sum_per_oper;
			best_load_store.time = time;
			best_load_store.counter_value = tot_load;
			best_load_store.bench_value = (long double)tot_load/(time);
		}
	}
	filename += to_string(thread_num);
	filename += "thr.csv";

	if(LOGS){
		ofstream REZ(filename.c_str(), ios::out|ios::app);

		REZ << d_type << ";"  << total_sum << ";" << datasize_load_store << ";" << num_sum*num_sum_per_oper << ";" << thread_num
			<< ";" << num_sum << ";" << num_sum_per_oper << ";" << time << ';'  << tot_load << ';' << tot_store << ';'
			<<  tot_ls_compl << ';' << (long double)tot_load/(time) << ';' << (long double)tot_store/(time) << ';' << (long double)tot_ls_compl/(time) << ";"
			<< tot_load_uops_ret << ";" << tot_store_uops_ret << endl;
		REZ.close();
	}

	for(int th = 0; th < thread_num; th++) {
		delete[] tmpi[th];
	}
	delete[] tmpi;
	delete[] EventSet;

	if(LOGS)
		LOG_LOAD_STORE << "___Stop random test___" << endl;

	return 0;

}

template<typename data_type>
int ld_st_test_norandom(data_type* A, char d_type, int num_sum_per_oper, long long num_sum, int offset) {
	// omp_set_num_threads(thread_num);
	long long imax = num_sum_per_oper*num_sum;

	if(imax + offset*(num_sum_per_oper - 1) + 1 > fiMax){
		if(LOGS)
			LOG_LOAD_STORE << "___Return test no random___" << endl;
		return 1;
	}

	if(LOGS)
		LOG_LOAD_STORE << "___Start test no random___" << endl;

	int* EventSet = new int[thread_num];

	PAPI_library_init(PAPI_VER_CURRENT);

	if (PAPI_thread_init( (unsigned long (*)(void))pthread_self ) != PAPI_OK)
		LOG_LOAD_STORE << "no random thread PAPI ERROR" << endl;

	// vector<data_type> sum(thread_num);

	#pragma omp parallel
	{
		int th = omp_get_thread_num();
		// sum[th] = 0;
		EventSet[th] = PAPI_NULL;
	}

	vector<double> begin(thread_num, 0), end(thread_num, 0);
	
	vector<long long> load(thread_num, 0);
	vector<long long> store(thread_num, 0);
	vector<long long> ls_compl(thread_num, 0);

	vector<long long> load_uops_ret(thread_num, 0);
	vector<long long> store_uops_ret(thread_num, 0);

	int cur_thr;
	long double total_sum = 0;

	// auto begin = std::chrono::high_resolution_clock::now();
	if(LOGS)
		LOG_LOAD_STORE << "no random start counting" << endl;
	#pragma omp parallel private (cur_thr)
	{
		data_type sum = 0;

		long double start_time, end_time;
		long long thr_off = cur_thr * datasize_per_thread_load_store;
		cur_thr = omp_get_thread_num();
		long long values[NUM_EVENTS] = {0, 0, 0, 0, 0};

		if(PAPI_register_thread() != PAPI_OK)
            LOG_LOAD_STORE << "no random register thread PAPI ERROR" << endl;

	    EventSet[cur_thr] = PAPI_NULL;
		if ( PAPI_create_eventset( &( EventSet[cur_thr] ) )  != PAPI_OK)
			LOG_LOAD_STORE << "no random create EventSet PAPI ERROR" << endl;


		if (PAPI_add_event(EventSet[cur_thr], PAPI_LD_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "no random 1add event PAPI ERROR" << endl;

	    if (PAPI_add_event(EventSet[cur_thr], PAPI_SR_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "no random 2add event PAPI ERROR" << endl;

	    if (PAPI_add_event(EventSet[cur_thr], PAPI_LST_INS) != PAPI_OK)
	    	LOG_LOAD_STORE << "no random 3add event PAPI ERROR" << endl;

	    //MEM_UOPS_RETIRED:ALL_LOADS
	   	unsigned int native_load = 1073741827;
	    if (PAPI_add_event(EventSet[cur_thr], native_load) != PAPI_OK)
	    	LOG_LOAD_STORE << "no random 4add event PAPI ERROR" << endl;

	    //MEM_UOPS_RETIRED:ALL_STORES
	    unsigned int native_store = 1073741828;
	    if (PAPI_add_event(EventSet[cur_thr], native_store) != PAPI_OK)
	    	LOG_LOAD_STORE << "no random 5add event PAPI ERROR" << endl;

	    #pragma omp barrier

		if (PAPI_start(EventSet[cur_thr]) != PAPI_OK)
			LOG_LOAD_STORE << "no random start PAPI ERROR" << endl;


		if(num_sum_per_oper == 1) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 2) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 3) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum +=  A[thr_off + i + offset*0] +  A[thr_off + i + offset*1] +  A[thr_off + i + offset*2];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 4) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 5) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l')  {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4];
				}
				end_time = omp_get_wtime() * 1000000;
			} 

		} else if(num_sum_per_oper == 6) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
					A[thr_off + i + offset*5] = i+5;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4] + A[thr_off + i + offset*5];
				}
				end_time = omp_get_wtime() * 1000000;
			}

		} else if(num_sum_per_oper == 7) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
					A[thr_off + i + offset*5] = i+5;
					A[thr_off + i + offset*6] = i+6;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4] + A[thr_off + i + offset*5] + A[thr_off + i + offset*6];
				}
				end_time = omp_get_wtime() * 1000000;
			} 

		} else if(num_sum_per_oper == 8) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
					A[thr_off + i + offset*5] = i+5;
					A[thr_off + i + offset*6] = i+6;
					A[thr_off + i + offset*7] = i+7;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4] + A[thr_off + i + offset*5] + A[thr_off + i + offset*6] + A[thr_off + i + offset*7];
				}
				end_time = omp_get_wtime() * 1000000;
			} 

		} else if(num_sum_per_oper == 9) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
					A[thr_off + i + offset*5] = i+5;
					A[thr_off + i + offset*6] = i+6;
					A[thr_off + i + offset*7] = i+7;
					A[thr_off + i + offset*8] = i+8;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4] + A[thr_off + i + offset*5] + A[thr_off + i + offset*6] + A[thr_off + i + offset*7] + A[thr_off + i + offset*8];
				}
				end_time = omp_get_wtime() * 1000000;
			} 
		} else if(num_sum_per_oper == 10) {

			if(op_load_store == 's') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					A[thr_off + i + offset*0] = i;
					A[thr_off + i + offset*1] = i+1;
					A[thr_off + i + offset*2] = i+2;
					A[thr_off + i + offset*3] = i+3;
					A[thr_off + i + offset*4] = i+4;
					A[thr_off + i + offset*5] = i+5;
					A[thr_off + i + offset*6] = i+6;
					A[thr_off + i + offset*7] = i+7;
					A[thr_off + i + offset*8] = i+8;
					A[thr_off + i + offset*9] = i+9;
				}
				end_time = omp_get_wtime() * 1000000;
			} else if(op_load_store == 'l') {
				start_time = omp_get_wtime() * 1000000;
				for(long long i = 0; i < imax; i += num_sum_per_oper) {
					sum += A[thr_off + i + offset*0] + A[thr_off + i + offset*1] + A[thr_off + i + offset*2] + A[thr_off + i + offset*3] + A[thr_off + i + offset*4] + A[thr_off + i + offset*5] + A[thr_off + i + offset*6] + A[thr_off + i + offset*7] + A[thr_off + i + offset*8] + A[thr_off + i + offset*9];
				}
				end_time = omp_get_wtime() * 1000000;
			}
		}

		if (PAPI_stop(EventSet[cur_thr], values) != PAPI_OK)
			LOG_LOAD_STORE << "stop PAPI ERROR" << endl;

		begin[cur_thr] = start_time;
		end[cur_thr] = end_time;

		load[cur_thr] += values[0];
		store[cur_thr] += values[1];
		ls_compl[cur_thr] += values[2];
		load_uops_ret[cur_thr] += values[3];
		store_uops_ret[cur_thr] += values[4];

		total_sum += sum/std::numeric_limits<data_type>::max();

		if( PAPI_cleanup_eventset(EventSet[cur_thr]) != PAPI_OK) 
			LOG_LOAD_STORE << "cleanup eventset PAPI ERROR" << endl;

		if(PAPI_destroy_eventset(&EventSet[cur_thr]) != PAPI_OK) 
			LOG_LOAD_STORE << "destroy eventset PAPI ERROR" << endl;

		if(PAPI_unregister_thread() != PAPI_OK)
        	LOG_LOAD_STORE << "unregister thread PAPI ERROR" << endl;
	}
	if(LOGS)
		LOG_LOAD_STORE <<"no random stop counting" << endl;

	long long tot_load = 0;
	long long tot_store = 0;
	long long tot_ls_compl = 0;
	long long tot_load_uops_ret = 0;
	long long tot_store_uops_ret = 0;

	long double time = 0;

	// long double total_sum = 0;
	for(int i = 0; i < thread_num; i++) {
		tot_load += load[i];
		tot_store += store[i];
		tot_ls_compl += ls_compl[i];
		tot_load_uops_ret += load_uops_ret[i];
		tot_store_uops_ret += store_uops_ret[i];

		// total_sum += sum[i]/std::numeric_limits<data_type>::max();
		if(end[i] - begin[i] > time){
			time = end[i] - begin[i];
		}
	}

	string filename;
	if (op_load_store == 's') {
		filename = "norand_store_op_lom2_ResultOMP_ver2_";

		if((long double)tot_store/(time) > best_load_store.bench_value) {
			best_load_store.type = "no random";
			best_load_store.data_type = "double";
			if(d_type == 'i'){
				best_load_store.data_type = "int";
			}
			if(d_type == 'c'){
				best_load_store.data_type = "char";
			}
			best_load_store.num_sum = num_sum_per_oper;
			best_load_store.total_sum = num_sum*num_sum_per_oper;
			best_load_store.time = time;
			best_load_store.counter_value = tot_store;
			best_load_store.bench_value = (long double)tot_store/(time);
		}
	} else if (op_load_store == 'l') {
		filename = "norand_load_op_lom2_ResultOMP_ver2_";

		if((long double)tot_load/(time) > best_load_store.bench_value) {
			best_load_store.type = "no random";
			best_load_store.data_type = "double";
			if(d_type == 'i'){
				best_load_store.data_type = "int";
			}
			if(d_type == 'c'){
				best_load_store.data_type = "char";
			}
			best_load_store.num_sum = num_sum_per_oper;
			best_load_store.total_sum = num_sum*num_sum_per_oper;
			best_load_store.time = time;
			best_load_store.counter_value = tot_load;
			best_load_store.bench_value = (long double)tot_load/(time);
		}
	}
	filename += to_string(thread_num);
	filename += "thr.csv";

	if(LOGS){
		ofstream REZ(filename.c_str(), ios::in|ios::app);
		REZ << d_type << ";"  << total_sum << ";" << offset << ";" << datasize_load_store << ";" << num_sum*num_sum_per_oper << ";" << thread_num
			<< ";" << num_sum << ";" << num_sum_per_oper << ";" << time << ';' << tot_load << ';' << tot_store << ';'
			<<  tot_ls_compl << ';' << (long double)tot_load/(time) << ';' << (long double)tot_store/(time) << ';' << (long double)tot_ls_compl/(time) << ";"
			<< tot_load_uops_ret << ";" << tot_store_uops_ret << endl;
		REZ.close();
	}

	delete[] EventSet;

	return 0;

}

void papi_events(){
	int EventCode, retval;
    char EventCodeStr[PAPI_MAX_STR_LEN];

    ofstream REZ("papi_events.txt", ios::trunc);

    /* Initialize the library */
    retval = PAPI_library_init(PAPI_VER_CURRENT);

    if  (retval != PAPI_VER_CURRENT) {
      fprintf(stderr, "PAPI library init error!\n");
      exit(1); 
     }

    EventCode = 0 | PAPI_NATIVE_MASK;

    auto status = PAPI_event_code_to_name(EventCode, EventCodeStr);

    while (status == PAPI_OK) {
       /* Translate the integer code to a string */

      /* Print all the native events for this platform */
      printf("Name: %s\n", EventCodeStr);
      LOG_LOAD_STORE << "Code: " << EventCode << endl;

      REZ << "Name: " << EventCodeStr << endl;
      REZ << "Code: " << EventCode << endl;

      EventCode++;
   		status = PAPI_event_code_to_name(EventCode, EventCodeStr);

    }

    REZ.close();
}

template <typename T>
void load_store_bench(char op_load_store, char d_type, int num_sum, int iter) {
	datasize_per_thread_load_store = L1size/(sizeof(T));
	datasize_load_store = datasize_per_thread_load_store * thread_num;

	auto fMin = numeric_limits<T>::min();
	auto fMax = numeric_limits<T>::max();

	T* data = new T [thread_num * datasize_per_thread_load_store];

	#pragma omp parallel
	{	
		// int th = omp_get_thread_num();

		// data[th] = new T [datasize_per_thread_load_store];
		for (long long i = 0; i < datasize_per_thread_load_store * thread_num; i++){
			data[i] = GetRand<T>(fMin, fMax);
		}

	}

	long long step = 64;

	//random
	for (int i = 0; i < iter; ++i) {
		for(long long k = step; k <= L1size/line_size; k += step) {

			long double tmp1 = 0;
			for (int th = 0; th < thread_num; th++) {
				tmp1 += data[th * datasize_per_thread_load_store + GetRand_idx(fiMin, fiMax)]/1000000;
			}
			if(LOGS)
				LOG_LOAD_STORE << "random part TMP1: " << tmp1 << "  " << endl;

			int result = ld_st_test_random<T>(data, d_type, num_sum, k);
			if (result != 0){
				if(LOGS)
					LOG_LOAD_STORE << "ld_st_test_random ERROR" << endl;
				break;
			}

			long double tmp2 = 0;
			for (int th = 0; th < thread_num; th++) {
				tmp2 += data[th * datasize_per_thread_load_store + GetRand_idx(fiMin, fiMax)]/1000000;
			}
			if(LOGS)
				LOG_LOAD_STORE << "random part TMP2: " << tmp2 << endl;
		}
	}

	//norandom
	for (int i = 0; i < iter; ++i) {
		for(long long k = step; k <= L1size/line_size; k += step) {
			for(int off = 0; off <= line_size; off += sizeof(T)) {

				long double tmp1 = 0;
				for (int th = 0; th < thread_num; th++) {
					tmp1 += data[th * datasize_per_thread_load_store + GetRand_idx(fiMin, fiMax)]/1000000;
				}
				if(LOGS)
					LOG_LOAD_STORE << "norandom part TMP1: " << tmp1 << "  " << endl;

				int result = ld_st_test_norandom<T>(data, d_type, num_sum, k, off);
				if(result != 0){
					if(LOGS)
						LOG_LOAD_STORE << "ld_st_test_norandom ERROR" << endl;
					break;
				}

				long double tmp2 = 0;
				for (int th = 0; th < thread_num; th++) {
					tmp2 += data[th * datasize_per_thread_load_store + GetRand_idx(fiMin, fiMax)]/1000000;
				}
				if(LOGS)
					LOG_LOAD_STORE << "norandom part TMP2: " << tmp2 << endl;
			}
		}
	}

	delete[] data;

	return;
}


load_store_best_result load_store (char oper, int iter, long long time_minutes) { //<s/l - store load test>
	// papi_events();
	LOG_LOAD_STORE.open("load_store_log.txt", ios::out|ios::app);

	best_load_store.type = ' ';
	best_load_store.operation = "load";
	if(oper == 's'){
		best_load_store.operation = "store";
	}
	best_load_store.num_sum = 0;
	best_load_store.total_sum = 0;
	best_load_store.threads = thread_num;
	best_load_store.time = 0;
	best_load_store.counter_value = 0;
	best_load_store.bench_value = 0;

	op_load_store = oper;

	auto start = std::chrono::high_resolution_clock::now();
	while(1){
		for(int num_sum = 1; num_sum <= 10; num_sum++){
			load_store_bench<double>(op_load_store, 'd', num_sum, iter);
			load_store_bench<int>(op_load_store, 'i', num_sum, iter);
			load_store_bench<char>(op_load_store, 'c', num_sum, iter);
			
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = end - start;
			if(time_minutes >= 0 && elapsed.count() >= time_minutes*60){
			    break;
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		if(elapsed.count() >= time_minutes*60){
		    break;
		}
	}

	LOG_LOAD_STORE.close();

	return best_load_store;
}
