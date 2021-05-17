#!/bin/bash
bench="none"
time=0
is_max=0
proc=1

if [ -n "$1" ]
then
	if [ $1 = "-h" ] || [ $1 = "--help" ]
	then
		echo "HELP!"
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
	fi
else
echo "No parameters found. "
fi

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

	if [ $bench = "fs_bw_write" ] || [ $bench = "fs_bw_read" ]
	then
		for (( iter=1; iter<=$ITERS; iter++ ))
	    do
	    	if [ $bench = "fs_bw_write" ]
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
