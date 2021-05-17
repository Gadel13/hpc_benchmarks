
all: main


main: main.cpp 
	g++ -O3 -o cache_test_2 cache_test_2.cpp -L papiso/ -lpapi -lm -std=c++11 -fopenmp -lpthread
	g++ -O3 -o cache_test_2_nopapi cache_test_2_nopapi.cpp -lm -std=c++11 -fopenmp -lpthread
	
qqq:
	g++ -O3 -o cache_test cache_test.cpp -L /usr/local/lib/ -lpapi -lm -std=c++11 -fopenmp -lpthread
	g++ -O3 -o main main.cpp -L papiso/ -lpapi -lm -lpapi -std=c++11
	g++ -O3 -o main_double_2sum main_double_2sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_double_3sum main_double_3sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_double_4sum main_double_4sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_double_5sum main_double_5sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_double_6sum main_double_6sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_double_7sum main_double_7sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_1sum main_int_1sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_2sum main_int_2sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_3sum main_int_3sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_4sum main_int_4sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_5sum main_int_5sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_6sum main_int_6sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_int_7sum main_int_7sum.cpp -L papiso/ -lpapi -lm -std=c++11
	g++ -O3 -o main_without_papi main_without_papi.cpp -L papiso/ -lpapi -lm -std=c++11

multi_test:
	./cache_test_2 3 s i 1 3
	./cache_test_2 3 s i 2 3
	./cache_test_2 3 s i 3 3
	./cache_test_2 3 s i 4 3
	./cache_test_2 3 s i 5 3
	./cache_test_2 3 s i 6 3
	./cache_test_2 3 s i 7 3
	./cache_test_2 3 s i 8 3
	./cache_test_2 3 s i 9 3
	./cache_test_2 3 s i 10 3

	./cache_test_2 3 s d 1 3
	./cache_test_2 3 s d 2 3
	./cache_test_2 3 s d 3 3
	./cache_test_2 3 s d 4 3
	./cache_test_2 3 s d 5 3
	./cache_test_2 3 s d 6 3
	./cache_test_2 3 s d 7 3
	./cache_test_2 3 s d 8 3
	./cache_test_2 3 s d 9 3
	./cache_test_2 3 s d 10 3


t1s:
	./cache_test_2 1 s i 1 1
	./cache_test_2 1 s d 1 1
	./cache_test_2 2 s i 1 1
	./cache_test_2 2 s d 1 1
	./cache_test_2 3 s i 1 1
	./cache_test_2 3 s d 1 1

t2s:
	./cache_test_2 1 s i 2 1
	./cache_test_2 1 s d 2 1
	./cache_test_2 2 s i 2 1
	./cache_test_2 2 s d 2 1
	./cache_test_2 3 s i 2 1
	./cache_test_2 3 s d 2 1

t3s:
	./cache_test_2 1 s i 3 1
	./cache_test_2 1 s d 3 1
	./cache_test_2 2 s i 3 1
	./cache_test_2 2 s d 3 1
	./cache_test_2 3 s i 3 1
	./cache_test_2 3 s d 3 1

t4s:
	./cache_test_2 1 s i 4 1
	./cache_test_2 1 s d 4 1
	./cache_test_2 2 s i 4 1
	./cache_test_2 2 s d 4 1
	./cache_test_2 3 s i 4 1
	./cache_test_2 3 s d 4 1

t5s:
	./cache_test_2 1 s i 5 1
	./cache_test_2 1 s d 5 1
	./cache_test_2 2 s i 5 1
	./cache_test_2 2 s d 5 1
	./cache_test_2 3 s i 5 1
	./cache_test_2 3 s d 5 1

t6s:
	./cache_test_2 1 s i 6 1
	./cache_test_2 1 s d 6 1
	./cache_test_2 2 s i 6 1
	./cache_test_2 2 s d 6 1
	./cache_test_2 3 s i 6 1
	./cache_test_2 3 s d 6 1

t7s:
	./cache_test_2 1 s i 7 1
	./cache_test_2 1 s d 7 1
	./cache_test_2 2 s i 7 1
	./cache_test_2 2 s d 7 1
	./cache_test_2 3 s i 7 1
	./cache_test_2 3 s d 7 1

t8s:
	./cache_test_2 1 s i 8 1
	./cache_test_2 1 s d 8 1
	./cache_test_2 2 s i 8 1
	./cache_test_2 2 s d 8 1
	./cache_test_2 3 s i 8 1
	./cache_test_2 3 s d 8 1

t9s:
	./cache_test_2 1 s i 9 1
	./cache_test_2 1 s d 9 1
	./cache_test_2 2 s i 9 1
	./cache_test_2 2 s d 9 1
	./cache_test_2 3 s i 9 1
	./cache_test_2 3 s d 9 1

t10s:
	./cache_test_2 1 s i 10 1
	./cache_test_2 1 s d 10 1
	./cache_test_2 2 s i 10 1
	./cache_test_2 2 s d 10 1
	./cache_test_2 3 s i 10 1
	./cache_test_2 3 s d 10 1



t1l:
	./cache_test_2 1 l i 1 1
	./cache_test_2 1 l d 1 1
	./cache_test_2 2 l i 1 1
	./cache_test_2 2 l d 1 1
	./cache_test_2 3 l i 1 1
	./cache_test_2 3 l d 1 1

t2l:
	./cache_test_2 1 l i 2 1
	./cache_test_2 1 l d 2 1
	./cache_test_2 2 l i 2 1
	./cache_test_2 2 l d 2 1
	./cache_test_2 3 l i 2 1
	./cache_test_2 3 l d 2 1

t3l:
	./cache_test_2 1 l i 3 1
	./cache_test_2 1 l d 3 1
	./cache_test_2 2 l i 3 1
	./cache_test_2 2 l d 3 1
	./cache_test_2 3 l i 3 1
	./cache_test_2 3 l d 3 1

t4l:
	./cache_test_2 1 l i 4 1
	./cache_test_2 1 l d 4 1
	./cache_test_2 2 l i 4 1
	./cache_test_2 2 l d 4 1
	./cache_test_2 3 l i 4 1
	./cache_test_2 3 l d 4 1

t5l:
	./cache_test_2 1 l i 5 1
	./cache_test_2 1 l d 5 1
	./cache_test_2 2 l i 5 1
	./cache_test_2 2 l d 5 1
	./cache_test_2 3 l i 5 1
	./cache_test_2 3 l d 5 1

t6l:
	./cache_test_2 1 l i 6 1
	./cache_test_2 1 l d 6 1
	./cache_test_2 2 l i 6 1
	./cache_test_2 2 l d 6 1
	./cache_test_2 3 l i 6 1
	./cache_test_2 3 l d 6 1

t7l:
	./cache_test_2 1 l i 7 1
	./cache_test_2 1 l d 7 1
	./cache_test_2 2 l i 7 1
	./cache_test_2 2 l d 7 1
	./cache_test_2 3 l i 7 1
	./cache_test_2 3 l d 7 1

t8l:
	./cache_test_2 1 l i 8 1
	./cache_test_2 1 l d 8 1
	./cache_test_2 2 l i 8 1
	./cache_test_2 2 l d 8 1
	./cache_test_2 3 l i 8 1
	./cache_test_2 3 l d 8 1

t9l:
	./cache_test_2 1 l i 9 1
	./cache_test_2 1 l d 9 1
	./cache_test_2 2 l i 9 1
	./cache_test_2 2 l d 9 1
	./cache_test_2 3 l i 9 1
	./cache_test_2 3 l d 9 1

t10l:
	./cache_test_2 1 l i 10 1
	./cache_test_2 1 l d 10 1
	./cache_test_2 2 l i 10 1
	./cache_test_2 2 l d 10 1
	./cache_test_2 3 l i 10 1
	./cache_test_2 3 l d 10 1

t1t:
	./cache_test_2 1 t i 1 1
	./cache_test_2 1 t d 1 1
	./cache_test_2 2 t i 1 1
	./cache_test_2 2 t d 1 1
	./cache_test_2 3 t i 1 1
	./cache_test_2 3 t d 1 1

t2t:
	./cache_test_2 1 t i 2 1
	./cache_test_2 1 t d 2 1
	./cache_test_2 2 t i 2 1
	./cache_test_2 2 t d 2 1
	./cache_test_2 3 t i 2 1
	./cache_test_2 3 t d 2 1

t3t:
	./cache_test_2 1 t i 3 1
	./cache_test_2 1 t d 3 1
	./cache_test_2 2 t i 3 1
	./cache_test_2 2 t d 3 1
	./cache_test_2 3 t i 3 1
	./cache_test_2 3 t d 3 1

t4t:
	./cache_test_2 1 t i 4 1
	./cache_test_2 1 t d 4 1
	./cache_test_2 2 t i 4 1
	./cache_test_2 2 t d 4 1
	./cache_test_2 3 t i 4 1
	./cache_test_2 3 t d 4 1

t5t:
	./cache_test_2 1 t i 5 1
	./cache_test_2 1 t d 5 1
	./cache_test_2 2 t i 5 1
	./cache_test_2 2 t d 5 1
	./cache_test_2 3 t i 5 1
	./cache_test_2 3 t d 5 1

t6t:
	./cache_test_2 1 t i 6 1
	./cache_test_2 1 t d 6 1
	./cache_test_2 2 t i 6 1
	./cache_test_2 2 t d 6 1
	./cache_test_2 3 t i 6 1
	./cache_test_2 3 t d 6 1

t7t:
	./cache_test_2 1 t i 7 1
	./cache_test_2 1 t d 7 1
	./cache_test_2 2 t i 7 1
	./cache_test_2 2 t d 7 1
	./cache_test_2 3 t i 7 1
	./cache_test_2 3 t d 7 1

t8t:
	./cache_test_2 1 t i 8 1
	./cache_test_2 1 t d 8 1
	./cache_test_2 2 t i 8 1
	./cache_test_2 2 t d 8 1
	./cache_test_2 3 t i 8 1
	./cache_test_2 3 t d 8 1

t9t:
	./cache_test_2 1 t i 9 1
	./cache_test_2 1 t d 9 1
	./cache_test_2 2 t i 9 1
	./cache_test_2 2 t d 9 1
	./cache_test_2 3 t i 9 1
	./cache_test_2 3 t d 9 1

t10t:
	./cache_test_2 1 t i 10 1
	./cache_test_2 1 t d 10 1
	./cache_test_2 2 t i 10 1
	./cache_test_2 2 t d 10 1
	./cache_test_2 3 t i 10 1
	./cache_test_2 3 t d 10 1

store_test:
	make t1s
	make t2s
	make t3s
	make t4s
	make t5s
	make t6s
	make t7s
	make t8s
	make t9s
	make t10s

load_test:
	make t1l
	make t2l
	make t3l
	make t4l
	make t5l
	make t6l
	make t7l
	make t8l
	make t9l
	make t10l

total_test:
	make t1t
	make t2t
	make t3t
	make t4t
	make t5t
	make t6t
	make t7t
	make t8t
	make t9t
	make t10t

multi_test_v2:
	make total_test
	make store_test
	make load_test

	make total_test
	make store_test
	make load_test

	make total_test
	make store_test
	make load_test

	make total_test
	make store_test
	make load_test

	make total_test
	make store_test
	make load_test



test_int: 
	./main_int_1sum 5;
	./main_int_2sum 5;
	./main_int_3sum 5;
	./main_int_4sum 5;
	./main_int_5sum 5;
	./main_int_6sum 5;

test_double:
	./main 5;
	./main_double_2sum 5;
	./main_double_3sum 5;
	./main_double_4sum 5;
	./main_double_5sum 5;
	./main_double_6sum 5;

test:
	./main_int_5sum 5;
	./main_double_5sum 5;
	./main_int_6sum 5;
	./main_double_6sum 5;


	./main_int_1sum 5;
	./main_int_2sum 5;
	./main_int_3sum 5;
	./main_int_4sum 5;
	
	./main 5;
	./main_double_2sum 5;
	./main_double_3sum 5;
	./main_double_4sum 5;

test1: 
	./main_int_1sum 5;
	./main_int_2sum 5;
	./main_int_3sum 5;
	./main_int_4sum 5;
	./main_int_5sum 5;
	./main_int_6sum 5;

	./main 5;
	./main_double_2sum 5;
	./main_double_3sum 5;
	./main_double_4sum 5;
	./main_double_5sum 5;
	./main_double_6sum 5;
	

clean:
	rm -f *.o main main_double_2sum main_double_3sum main_double_4sum \
		main_int_1sum main_int_2sum main_int_3sum main_int_4sum \
		main_int_5sum main_int_6sum main_int_7sum main_double_5sum \
		main_double_6sum main_double_7sum main_without_papi cache_test cache_test_2 cache_test_2_nopapi
	rm -f *.o commands
	rm -f *.txt
	rm -f *.png


