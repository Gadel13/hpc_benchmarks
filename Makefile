CC=gcc
CXX=g++
MPICC=mpicc
MPICXX=mpicxx

CXXFLAGS=-O3 -std=c++11 -w
LDFLAGS=-lm -lrt -fopenmp -lpthread -I /papiso/include -L /papiso/lib -lpapi

all: hpc_benchmarks osu tile

utils.o: utils.hpp utils.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c utils.hpp utils.cpp

load_store.o: src_bench/load_store/load_store.hpp src_bench/load_store/load_store.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c src_bench/load_store/load_store.hpp src_bench/load_store/load_store.cpp 

cache_miss.o: src_bench/cache_miss/cache_miss.hpp src_bench/cache_miss/cache_miss.cpp
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c src_bench/cache_miss/cache_miss.hpp src_bench/cache_miss/cache_miss.cpp

memfree.o: src_bench/memfree/memfree.hpp src_bench/memfree/memfree.cpp
	$(CXX) $(LDFLAGS) -O0 -std=c++11 -c src_bench/memfree/memfree.hpp src_bench/memfree/memfree.cpp
	
hpc_benchmarks: hpc_benchmarks.cpp load_store.o cache_miss.o utils.o memfree.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) hpc_benchmarks.cpp utils.o load_store.o cache_miss.o  memfree.o -o hpc_benchmarks
	chmod 777 mpi_hpc_benchmarks.sh
	chmod 777 run_hpc_benchmarks.sh

osu:
	cd src_bench/osu-micro-benchmarks; chmod 777 configure; ./configure CC=$(MPICC) CXX=$(MPICXX); make; cd ../..

tile:
	cd src_bench/mpi-tile-io; make; cd ../..


clean:
	rm -rf hpc_benchmarks *.o *.gch *.txt *.csv *tmp_file* DATA_FROM_TESTS_*
	cd src_bench/osu-micro-benchmarks ; make clean ; cd ../..
	cd src_bench/mpi-tile-io ; make clean ; cd ../..