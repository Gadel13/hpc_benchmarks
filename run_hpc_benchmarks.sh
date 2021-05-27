#!/bin/bash

time=30
maximum="none"

if [ $1 = "mpi" ]
then
	if [ $2 = "-h" ] || [ $2 = "--help" ]
	then
		./mpi_hpc_benchmarks.sh -h
	else
		p=''
		np=14
		nodes=1
		for param in "$@"
		do
			if [ $time = "req" ]
			then
				time=$(($param + 10))
			fi
			if [ $maximum = "req" ]
			then
				maximum=$(($param))
			fi
			if [ $param = "--max" ]
			then
				maximum="req"
			fi
			if [ $param = "-t" ] || [ $param = "--time" ]
			then
				time="req"
			fi
			if [ $param = "mpi_bw" ]
			then
				np=2
				nodes=2
			fi
			if [ $param != "mpi" ]
			then
				p="${p} ${param}"
			fi
		done

		if [ $maximum -eq 1 ] || [ $maximum = "1" ]
		then	
			time=$(($time * 10 + 30))
		fi

		echo "sbatch -n $np -N $nodes -t $time mpi_hpc_benchmarks.sh $p"
		sbatch -n $np -N $nodes -t $time mpi_hpc_benchmarks.sh $p
	fi
else
	if [ $1 = "seq" ]
	then
		if [ $2 = "-h" ] || [ $2 = "--help" ]
		then
			./hpc_benchmarks -h
		else
			p=''
			np=1
			nodes=1
			for param in "$@"
			do
				if [ $time = "req" ]
				then
					time=$(($param * 9 + 5))
				fi
				if [ $maximum = "req" ]
				then
					maximum=$(($param))
				fi
				if [ $param = "--max" ]
				then
					maximum="req"
				fi
				if [ $param = "-t" ] || [ $param = "--time" ]
				then
					time="req"
				fi	
				if [ $param != "seq" ]
				then
					p="${p} ${param}"
				fi
			done

			if [ $maximum -eq 1 ] || [ $maximum = "1" ]
			then	
				time=$(($time * 5 + 30))
			fi
			
			echo "export LD_LIBRARY_PATH=papiso/lib:\$LD_LIBRARY_PATH && export PATH=papiso/bin:\$PATH && export GOMP_CPU_AFFINITY=\"0-13\" && sbatch -n ${np} -N ${nodes} -t ${time} -C nomonitoring run ./hpc_benchmarks $p"
			export LD_LIBRARY_PATH=papiso/lib:$LD_LIBRARY_PATH && export PATH=papiso/bin:$PATH && export GOMP_CPU_AFFINITY="0-13" && sbatch -n ${np} -N ${nodes} -t ${time} -C nomonitoring run ./hpc_benchmarks $p
		fi
	else
		echo "Incorrect run: ./run_hpc_benchmarks.sh mpi/seq <params>"
	fi
fi