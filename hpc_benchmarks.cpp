#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>

#include <string>

#include "src_bench/load_store/load_store.hpp"
#include "src_bench/cache_miss/cache_miss.hpp"
#include "src_bench/memfree/memfree.hpp"
#include "utils.hpp"

using namespace std;

static const int out_size = 20;

long long time_minutes = -1;
string outfilename = "";
int num_threads = core_num;
bool is_max = 0;
int iter = 1;

int count_test = 1;
int max_count_test = 9;

ofstream out;

struct benchmarks {
	bool mpi_send{1};
	bool mpi_recv{1};
	bool fs_write{1};
	bool fs_read{1};
	bool load{1};
	bool store{1};
	bool l1_miss_load{1};
	bool l2_miss_load{1};
	bool l3_miss_load{1};
    bool l1_miss_store{1};
    bool l2_miss_store{1};
    bool l3_miss_store{1};
	bool memfree{1};
};

benchmarks bench;

void usage() {
	printf("Usage:\n");
    printf("   -h | --help show usage\n");
    printf("   -b | --bench <list> select benchmarks (default - all)\n \
    	   		Avaliable: load, store, l1_miss_load/store, l2_miss(_load/store), l3_miss(_load/store), memfree, all\n \
    	   		Example: --bench mpi_send fs_write l3_miss memfree store\n");

    printf("   -t | --time <minutes> time in minutes of executing each benchmark [1, inf) (default - no limit)\n");
    printf("   -o | --outfile <filename> output file name (default - stdout)\n");
    printf("   -th | --threads <thread_num> number of OpenMP threads [1. core_num] (default - core_num from utils.hpp)\n");
    printf("   --max <0/1> get maximum values from benchmarks (0 - false/ 1 - true; default - 0) If use --max 1 - -t dont work\n");
    exit(1);
}

void init (int argc, char** argv) {
    std::string bencmark_list = "all";
  	for (int i = 1; i < argc; i++) {
        int cur = i;
  		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
  			usage();
            continue;
  		}
        if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bench")) {
            bencmark_list = "";
            max_count_test = 0;
        	//список бенчмарков
        	bench.load = 0;
        	bench.store = 0;
        	bench.l1_miss_load = 0;
        	bench.l2_miss_load = 0;
        	bench.l3_miss_load = 0;
            bench.l1_miss_store = 0;
            bench.l2_miss_store = 0;
            bench.l3_miss_store = 0;
        	bench.memfree = 0;

        	int is_correct = 0;
        	i++;
        	while(i < argc && (!strcmp(argv[i], "load") || !strcmp(argv[i], "store") || !strcmp(argv[i], "l1_miss") || !strcmp(argv[i], "l2_miss") || !strcmp(argv[i], "l3_miss") || 
                  !strcmp(argv[i], "l1_miss_load") || !strcmp(argv[i], "l2_miss_load") || !strcmp(argv[i], "l3_miss_load") ||
                  !strcmp(argv[i], "l1_miss_store") || !strcmp(argv[i], "l2_miss_store") || !strcmp(argv[i], "l3_miss_store") ||
                  !strcmp(argv[i], "memfree") || !strcmp(argv[i], "all"))) 
            {

        		if(!strcmp(argv[i], "load")){
        			bench.load = 1;
                    bencmark_list += "load ";
                    max_count_test++;
        		} else if(!strcmp(argv[i], "store")){
        			bench.store = 1;
                    bencmark_list += "store ";
                    max_count_test++;
        		} else if(!strcmp(argv[i], "l1_miss")){
        			bench.l1_miss_load = 1;
                    bench.l1_miss_store = 1;
                    bencmark_list += "l1_miss_load l1_miss_store ";
                    max_count_test++;
                    max_count_test++;
        		} else if(!strcmp(argv[i], "l2_miss")){
        			bench.l2_miss_load = 1;
                    bench.l2_miss_store = 1;
                    bencmark_list += "l2_miss_load l2_miss_store ";
                    max_count_test++;
                    max_count_test++;
        		} else if(!strcmp(argv[i], "l3_miss")){
        			bench.l3_miss_load = 1;
                    bench.l3_miss_store = 1;
                    bencmark_list += "l3_miss_load l3_miss_store ";
                    max_count_test++;
                    max_count_test++;
                } else if(!strcmp(argv[i], "l1_miss_load")){
                    bench.l1_miss_load = 1;
                    bencmark_list += "l1_miss_load ";
                    max_count_test++;
                } else if(!strcmp(argv[i], "l2_miss_load")){
                    bench.l2_miss_load = 1;
                    bencmark_list += "l2_miss_load ";
                    max_count_test++;
                } else if(!strcmp(argv[i], "l3_miss_load")){
                    bench.l3_miss_load = 1;
                    bencmark_list += "l3_miss_load ";
                    max_count_test++;
                } else if(!strcmp(argv[i], "l1_miss_store")){
                    bench.l1_miss_store = 1;
                    bencmark_list += "l1_miss_store ";
                    max_count_test++;
                } else if(!strcmp(argv[i], "l2_miss_store")){
                    bench.l2_miss_store = 1;
                    bencmark_list += "l2_miss_store ";
                    max_count_test++;
                } else if(!strcmp(argv[i], "l3_miss_store")){
                    bench.l3_miss_store = 1;
                    bencmark_list += "l3_miss_store ";
                    max_count_test++;
        		} else if(!strcmp(argv[i], "memfree")){
        			bench.memfree = 1;
                    bencmark_list += "memfree ";
                    max_count_test++;
        		} else if(!strcmp(argv[i], "all")){
                    bench.load = 1;
                    bench.store = 1;
                    bench.l1_miss_load = 1;
                    bench.l2_miss_load = 1;
                    bench.l3_miss_load = 1;
                    bench.l1_miss_store = 1;
                    bench.l2_miss_store = 1;
                    bench.l3_miss_store = 1;
                    bench.memfree = 1;
                    max_count_test = 9;
        		}
        		i++;
        		is_correct++;
        	}
        	if(!is_correct){
        		printf("Incorrect -b/--bench param\n");
            	usage();
        	}
            i--;
            continue;
        }
        if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--time")) {
            //время работы бенчмарка
            time_minutes = atoi(argv[++i]);
            if(time_minutes <= 0){
            	printf("Incorrect -t/--time param\n");
            	usage();
            }
        }
        if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--outfile")) {
            //файл вывода
            outfilename = argv[++i];
        }
        if (!strcmp(argv[i], "-th") || !strcmp(argv[i], "--threads")) {
            //число потоков
            num_threads = atoi(argv[++i]);
            if(num_threads <= 0){
                printf("Incorrect -th/--threads param\n");
            	usage();
            }
        }
        if (!strcmp(argv[i], "--max")) {
            //получить максимумы по бенчмпркам
            is_max = atoi(argv[++i]);
            if(is_max != 0 && is_max != 1){
            	printf("Incorrect --max param\n");
            	usage();
            }
            iter = 5;
        }

        if(cur == i){
            printf("Invalid params\n");
            usage();
        }
    }


    if (max_count_test > 9) {
        max_count_test = 9;
    }
    cout << "Input params:" << endl;
    cout << "Benchmarks: " << bencmark_list << endl;
    if(outfilename != ""){
        cout << "Output file: " << outfilename << endl;
    } else {
        cout << "Output: stdout" << endl;
    }
    if(time_minutes != -1){
        cout << "Time(min): " << time_minutes << endl;
    }
    if(is_max){
        cout << "Max: " << "True" << endl;
        cout << "Threads: " << 1 << "-" << core_num << endl;

        if(bench.memfree == 1) {
            max_count_test = (max_count_test - 1) * core_num + iter;
        } else {
             max_count_test = max_count_test * core_num;
        }
    } else {
        cout << "Max: " << "False" << endl;
        cout << "Threads: " << num_threads << endl;
    }
    cout << endl;


}

void cache_miss_test(int lvl, char op){
    double theoretical;
    if(lvl == 1){
        if(op == 'l'){
            theoretical = theoretical_l1_miss_load;
        } else {
            theoretical = theoretical_l1_miss_store;
        }
    } else if(lvl == 2){
        if(op == 'l'){
            theoretical = theoretical_l2_miss_load;
        } else {
            theoretical = theoretical_l2_miss_store;
        }
    } else {
        if(op == 'l'){
            theoretical = theoretical_l3_miss_load;
        } else {
            theoretical = theoretical_l3_miss_store;
        }
    }
    if(is_max) {
            cache_miss_best_result max = {};
            for(int th = 1; th <= core_num; th++) {
                omp_set_num_threads(th);
                cout << "\r" << "Running " << count_test << " of " << max_count_test << std::flush;
                count_test++;
                auto result = cache_miss(lvl, op, iter, time_minutes);
                if (max.bench_value < result.bench_value) {
                    max = result;
                } 
            }
            if(outfilename != "") {
                out <<  setw(out_size) << max.cache_level << ";" <<
                        setw(out_size) << max.operation << ";" <<
                        setw(out_size) << max.data_type << ";" <<
                        setw(out_size) << max.num_sum << ";" <<
                        setw(out_size) << max.total_sum << ";" << 
                        setw(out_size) << max.threads << ";" <<
                        setw(out_size) << max.time << ";" << 
                        setw(out_size) << max.counter_value << ";" <<
                        setw(out_size) << max.bench_value << ";" <<
                        setw(out_size) << max.bench_value*100.0/theoretical << endl;
            } else {
                cout << "\r" << setw(out_size) << max.cache_level <<
                        setw(out_size) << max.operation <<
                        setw(out_size) << max.data_type <<
                        setw(out_size) << max.num_sum <<
                        setw(out_size) << max.total_sum << 
                        setw(out_size) << max.threads <<
                        setw(out_size) << max.time << 
                        setw(out_size) << max.counter_value <<
                        setw(out_size) << max.bench_value << 
                        setw(out_size) << max.bench_value*100.0/theoretical << endl;
            }
        } else {
            cout << "\r" << "Running " << count_test << " of " << max_count_test << std::flush;
            count_test++;
            omp_set_num_threads(num_threads);
            auto result = cache_miss(lvl, op, iter, time_minutes);
            if(outfilename != "") {
                out <<  setw(out_size) << result.cache_level << ";" <<
                        setw(out_size) << result.operation << ";" <<
                        setw(out_size) << result.data_type << ";" <<
                        setw(out_size) << result.num_sum << ";" <<
                        setw(out_size) << result.total_sum << ";" << 
                        setw(out_size) << result.threads << ";" <<
                        setw(out_size) << result.time << ";" << 
                        setw(out_size) << result.counter_value << ";" <<
                        setw(out_size) << result.bench_value <<
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            } else {
                cout << "\r" << setw(out_size) << result.cache_level <<
                        setw(out_size) << result.operation <<
                        setw(out_size) << result.data_type <<
                        setw(out_size) << result.num_sum <<
                        setw(out_size) << result.total_sum << 
                        setw(out_size) << result.threads <<
                        setw(out_size) << result.time << 
                        setw(out_size) << result.counter_value <<
                        setw(out_size) << result.bench_value << 
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            }
        }
        if(outfilename == "") {
            cout << endl;
        }
}

void load_store_test(char op){
    double theoretical;
    if(op == 'l'){
        theoretical = theoretical_load;
    } else {
        theoretical = theoretical_store;
    }
    if(is_max) {
            load_store_best_result max = {};
            for(int th = 1; th <= core_num; th++) {
                omp_set_num_threads(th);
                cout << "\r" << "Running " << count_test << " of " << max_count_test << std::flush;
                count_test++;
                auto result = load_store(op, iter, time_minutes);
                if (max.bench_value < result.bench_value) {
                    max = result;
                }  
            }
            if(outfilename != ""){
                out << setw(out_size) << max.type << ";"  << 
                        setw(out_size) << max.operation << ";"  <<
                        setw(out_size) << max.data_type << ";"  <<
                        setw(out_size) << max.num_sum << ";"  <<
                        setw(out_size) << max.total_sum << ";"  << 
                        setw(out_size) << max.threads << ";"  <<
                        setw(out_size) << max.time << ";"  << 
                        setw(out_size) << max.counter_value << ";"  <<
                        setw(out_size) << max.bench_value << ";" <<
                        setw(out_size) << max.bench_value*100.0/theoretical << endl;
            } else {
                cout << "\r" << setw(out_size) << max.type << 
                        setw(out_size) << max.operation <<
                        setw(out_size) << max.data_type <<
                        setw(out_size) << max.num_sum <<
                        setw(out_size) << max.total_sum << 
                        setw(out_size) << max.threads <<
                        setw(out_size) << max.time << 
                        setw(out_size) << max.counter_value <<
                        setw(out_size) << max.bench_value << 
                        setw(out_size) << max.bench_value*100.0/theoretical << endl;
            }
        } else {
            cout << "\r" << "Running " << count_test << " of " << max_count_test << std::flush;
            count_test++;
            omp_set_num_threads(num_threads);
            auto result = load_store(op, iter, time_minutes);
            if(outfilename != ""){
                out << setw(out_size) << result.type << ";" <<
                        setw(out_size) << result.operation << ";" <<
                        setw(out_size) << result.data_type << ";" <<
                        setw(out_size) << result.num_sum << ";" <<
                        setw(out_size) << result.total_sum << ";" << 
                        setw(out_size) << result.threads << ";" <<
                        setw(out_size) << result.time << ";" << 
                        setw(out_size) << result.counter_value << ";" <<
                        setw(out_size) << result.bench_value << ";" << 
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            } else {
                cout << "\r" << setw(out_size) << result.type <<
                        setw(out_size) << result.operation <<
                        setw(out_size) << result.data_type <<
                        setw(out_size) << result.num_sum <<
                        setw(out_size) << result.total_sum << 
                        setw(out_size) << result.threads <<
                        setw(out_size) << result.time << 
                        setw(out_size) << result.counter_value <<
                        setw(out_size) << result.bench_value << 
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            }
        }
        if(outfilename == "") {
            cout << endl;
        }
}

void memfree_test(){
    double theoretical = theoretical_memfree;
    memfree_best_result max = {};
    for(int it = 0; it < iter; it++){
        cout << "\r" << "Running " << count_test << " of " << max_count_test << std::flush;
        count_test++;
        auto result = memfree(time_minutes);
        if(!is_max){
            if(outfilename != "") {
                out <<  setw(out_size) << result.system_memfree <<
                        setw(out_size) << result.system_buf_cache <<
                        setw(out_size) << result.bench_value << 
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            } else {
                cout << "\r" <<  setw(out_size) << result.system_memfree <<
                        setw(out_size) << result.system_buf_cache <<
                        setw(out_size) << result.bench_value << 
                        setw(out_size) << result.bench_value*100.0/theoretical << endl;
            }
        } else {
            if(result.bench_value > max.bench_value){
                max = result;
            }
        }
    }
    if(is_max){
        if(outfilename != "") {
            out <<  setw(out_size) << max.system_memfree <<
                    setw(out_size) << max.system_buf_cache <<
                    setw(out_size) << max.bench_value << 
                    setw(out_size) << max.bench_value*100.0/theoretical << endl;
        } else {
            cout << "\r" <<  setw(out_size) << max.system_memfree <<
                    setw(out_size) << max.system_buf_cache <<
                    setw(out_size) << max.bench_value << 
                    setw(out_size) << max.bench_value*100.0/theoretical << endl;
        }
    }
    if(outfilename == "") {
        cout << endl;
    }
}

int main(int argc, char **argv) {
	init(argc, argv);

    cout.setf(ios::left);

    if(outfilename != ""){
        out.open(outfilename, ios_base::out|ios_base::app);
    }

    if(is_max){
        cout << "MAXIMUM VALUES" << endl;
    }

	if(bench.load || bench.store) {
        if(outfilename != ""){
            out << "LOAD/STORE" << endl;
            out << setw(out_size) << "Benchmark type;" <<
                    setw(out_size) << "Operation;" <<
                    setw(out_size) << "Data type;" <<
                    setw(out_size) << "Num sum per iter;" <<
                    setw(out_size) << "Total number of sum;" << 
                    setw(out_size) << "Threads number;" <<
                    setw(out_size) << "Time(sec);" << 
                    setw(out_size) << "uops;" <<
                    setw(out_size) << "uops/sec(mln)" <<
                    setw(out_size) << "% from theoretical" << endl;
        } else{
            cout << "LOAD/STORE" << endl;
            cout << setw(out_size) << "Benchmark type" <<
                    setw(out_size) << "Operation" <<
                    setw(out_size) << "Data type" <<
                    setw(out_size) << "Num sum per iter" <<
                    setw(out_size) << "Total number of sum" << 
                    setw(out_size) << "Threads number" <<
                    setw(out_size) << "Time(sec)" << 
                    setw(out_size) << "uops" <<
                    setw(out_size) << "uops/sec(mln)" <<
                    setw(out_size) << "% from theoretical" << endl;
        }
        if(bench.load){
            load_store_test('l');
    	}
    	if(bench.store){
            load_store_test('s');
        }
        if(outfilename != ""){
            out << endl;
        } else{
            cout << endl;
        }
    }

    if (bench.l1_miss_load || bench.l1_miss_store ||
        bench.l2_miss_load || bench.l2_miss_store ||
        bench.l3_miss_load || bench.l3_miss_store) {

        if(outfilename != ""){
            out << "CACHE MISS" << endl;
            out << setw(out_size) << "Cache lvl;" <<
                    setw(out_size) << "Operation;" <<
                    setw(out_size) << "Data type;" <<
                    setw(out_size) << "Num sum per iter;" <<
                    setw(out_size) << "Total number of sum;" << 
                    setw(out_size) << "Threads number;" <<
                    setw(out_size) << "Time(sec);" << 
                    setw(out_size) << "miss number;" <<
                    setw(out_size) << "miss/sec(mln)" <<
                    setw(out_size) << "% from theoretical" << endl;
        } else {
            cout << "CACHE MISS" << endl;
            cout << setw(out_size) << "Cache lvl" <<
                    setw(out_size) << "Operation" <<
                    setw(out_size) << "Data type" <<
                    setw(out_size) << "Num sum per iter" <<
                    setw(out_size) << "Total number of sum" << 
                    setw(out_size) << "Threads number" <<
                    setw(out_size) << "Time(sec)" << 
                    setw(out_size) << "miss number" <<
                    setw(out_size) << "miss/sec(mln)" <<
                    setw(out_size) << "% from theoretical" << endl;
        }

        if(bench.l1_miss_load){
            cache_miss_test(1, 'l');
        }
        if(bench.l1_miss_store){
            cache_miss_test(1, 's');
        }

        if(bench.l2_miss_load){
            cache_miss_test(2, 'l');
        }
        if(bench.l2_miss_store){
            cache_miss_test(2, 's');
        }

        if(bench.l3_miss_load){
            cache_miss_test(3, 'l');
        }
        if(bench.l3_miss_store){
            cache_miss_test(3, 's');
        }

        if(outfilename != ""){
            out << endl;
        } else {
            cout << endl;
        }
    }

    if(bench.memfree){
        if(outfilename != ""){
            out << "MEMFREE" << endl;
            out << setw(out_size) << "Mem avaliable(KB);" <<
                    setw(out_size) << "Buf + Cache(KB);" <<
                    setw(out_size) << "Bench result(KB)" <<
                    setw(out_size) << "% from theoretical" << endl;
        } else {
            cout << "MEMFREE" << endl;
            cout << setw(out_size) << "Mem avaliable(KB)" <<
                    setw(out_size) << "Buf + Cache(KB)" <<
                    setw(out_size) << "Bench result(KB)" <<
                    setw(out_size) << "% from theoretical" << endl;
        }
        memfree_test();
        if(outfilename != ""){
            out << endl;
        } else {
            cout << endl;
        }
    }

    if(outfilename != ""){
        out.close();
    }

	return 0;
}
