#!/bin/bash

HELP="Usage:\nExecute: sbatch -n <np> ./mpi_hpc_benchmarks.sh <params>\n-h | --help show usage\n-b | --bench <bencmark> select benchmark (Avaliable: mpi_bw, fs_write, fs_read)\n-t | --time <minutes> time in minutes of executing each benchmark (default - 0 no limit)\n--max <0/1> get maximum values from benchmarks (0 - false/ 1 - true; default - 0)\n"

bench="none"
time=0
is_max=0
proc=14

if [ -n "$1" ]
then
	if [ $1 = "-h" ] || [ $1 = "--help" ]
	then
		echo -e $HELP
	else
		p="none"
		for param in "$@"
		do
			if [ $p = "b" ]
			then
				bench=$param
				p="none"
			fi

			if [ $p = "t" ]
			then
				time=$param
				p="none"
			fi

			if [ $p = "max" ]
			then
				is_max=$param
				p="none"
			fi

			if [ $p = "proc" ]
			then
				proc=$param
				p="none"
			fi


			if [ $p = "none" ]
			then
				if [ $param = "-b" ] || [ $param = "--bench" ]
				then
					p="b"

				fi

				if [ $param = "-t" ] || [ $param = "--time" ]
				then
					p="t"
				fi

				if [ $param = "--max" ]
				then
					p="max"
				fi

				if [ $param = "-p" ] || [ $param = "--proc" ]
				then
					p="proc"
				fi
			fi
		done

		echo "Benchmark: $bench"
		echo "Time: $time"
		echo "Count max: $is_max"
		echo ""

		start=`date +%s`
		end=$start

		is_error=0

		while [ $(($end - $start)) -le $(($time * 60)) ] && [ $is_error -eq 0 ]
		do
			if [ $is_max -eq 0 ]
			then
				ITERS=1
			else
				ITERS=10
			fi

			if [ $bench = "fs_write" ] || [ $bench = "fs_read" ]
			then
				for (( iter=1; iter<=$ITERS; iter++ ))
			    do
			    	if [ $bench = "fs_write" ]
			    	then
			        	ompi ./src_bench/mpi-tile-io/mpi-tile-io --nr_files $proc --nr_tiles_x $proc --nr_tiles_y 1 --sz_tile_x 1 --sz_tile_y 67108864 --sz_element 1 --filename mpi-tile-io_tmp_file_$proc --write_file
			        else
			        	ompi ./src_bench/mpi-tile-io/mpi-tile-io --nr_files $proc --nr_tiles_x $proc --nr_tiles_y 1 --sz_tile_x 1 --sz_tile_y 67108864 --sz_element 1 --filename mpi-tile-io_tmp_file_$proc
			    	fi
			    done
			else
				if [ $bench = "mpi_bw" ]
				then
					for (( iter=1; iter<=$ITERS; iter++ ))
				    do
				        ompi ./src_bench/osu-micro-benchmarks/mpi/pt2pt/osu_bw
				    done
			    else
			    	echo "Incorrect Benchmark name"
			    	is_error=1
				fi
			fi
			end=`date +%s`
		done

		echo "FINISH"
	fi
else
echo -e "Incorrect parameters!\n"
echo -e $HELP
fi
