#!/bin/bash

time=30
maximum="none"
queue="compute"

if [ $1 = "test" ] || [ $1 = "compute" ] || [ $1 != "pascal" ] || [ $1 = "compute_prio" ]
then
	queue=$1

	if [ $2 = "mpi" ]
	then
		if [ $3 = "-h" ] || [ $3 = "--help" ]
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
				if [ $param != "mpi" ] && [ $param != $1 ]
				then
					p="${p} ${param}"
				fi
			done

			if [ $maximum -eq 1 ] || [ $maximum = "1" ]
			then	
				time=$(($time * 10 + 30))
			fi
			if [ $queue = "test" ]
			then
				time=15
			fi

			echo "sbatch -n $np -N $nodes -t $time -p $queue mpi_hpc_benchmarks.sh $p"
			sbatch -n $np -N $nodes -t $time -p $queue mpi_hpc_benchmarks.sh $p
		fi
	else

		if [ $2 = "seq" ]
		then
			if [ $3 = "-h" ] || [ $3 = "--help" ]
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
					if [ $param != "seq" ] && [ $param != $1 ]
					then
						p="${p} ${param}"
					fi
				done

				if [ $maximum -eq 1 ] || [ $maximum = "1" ]
				then	
					time=$(($time * 5 + 30))
				fi
				if [ $queue = "test" ]
				then
					time=15
				fi

				
				echo "export LD_LIBRARY_PATH=papiso/lib:\$LD_LIBRARY_PATH && export PATH=papiso/bin:\$PATH && export GOMP_CPU_AFFINITY=\"0-13\" && sbatch -n ${np} -N ${nodes} -t ${time} -C nomonitoring -p $queue run ./hpc_benchmarks $p"
				export LD_LIBRARY_PATH=papiso/lib:$LD_LIBRARY_PATH && export PATH=papiso/bin:$PATH && export GOMP_CPU_AFFINITY="0-13" && sbatch -n ${np} -N ${nodes} -t ${time} -C nomonitoring -p $queue run ./hpc_benchmarks $p
			fi
		else
			echo "Incorrect run: ./run_hpc_benchmarks.sh compute/pascal/test/compute_prio mpi/seq <params>"
		fi
	fi
else
	echo "Incorrect run: ./run_hpc_benchmarks.sh compute/pascal/test/compute_prio mpi/seq <params>"
fi



