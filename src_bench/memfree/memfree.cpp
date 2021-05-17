#include "memfree.hpp"

ofstream LOG_MEMFREE;

memfree_best_result memfree(long long time_minutes) {

	memfree_best_result best = {};
	if(LOGS){
		LOG_MEMFREE.open("memfree_log.txt", ios::out|ios::app);
	}

	ifstream meminfo("/proc/meminfo", ios::in);
  	string current = "";
  	while(!meminfo.eof()){
  		meminfo >> current;
	  	if(current == "MemAvailable:"){
	  		meminfo >> current;
  			if(LOGS)
		  		LOG_MEMFREE << "Available memory in system(from /proc/meminfo): " << atoi(current.data()) << endl << endl;
	  		best.system_memfree = atoi(current.data());
	  	}
	  	if(current == "Buffers:"){
	  		meminfo >> current;
  			if(LOGS)
		  		LOG_MEMFREE << "Buffers in system(from /proc/meminfo): " << atoi(current.data()) << endl << endl;
	  		best.system_buf_cache += atoi(current.data());
	  	}
	  	if(current == "Cached:"){
	  		meminfo >> current;
  			if(LOGS)
		  		LOG_MEMFREE << "Cached in system(from /proc/meminfo): " << atoi(current.data()) << endl << endl;
	  		best.system_buf_cache += atoi(current.data());
	  	}
  	}

  	meminfo.close();


	ofstream RES, RES_s;

	if(LOGS){
		RES.open("dynamic_mem_use.csv", ios::out|ios::trunc);
		RES_s.open("static_mem_use.csv", ios::out|ios::trunc);
	}

	auto start = std::chrono::high_resolution_clock::now();

  	while(1){
		//Dynamic
		unsigned long long tot_kb = 0;

		//GB
		unsigned long long tot_size_gb = 0;;
		double *a;
		unsigned long long gb;

		try {

			for(unsigned long long i = 0; ; i++){
				a = new double[i * 1024 * 1024 * 1024 / sizeof(double)];
				gb = i * 1024 * 1024 * 1024 / sizeof(double);

				tot_size_gb = gb;

				long double size_bytes = gb * sizeof(double);
				long double size_Kbytes = (long double)size_bytes / (1024);
				long double size_Mbytes = (long double)size_bytes / (1024 * 1024);
				long double size_Gbytes = (long double)size_bytes / (1024 * 1024 * 1024);

				if(LOGS)
					RES << size_bytes << "; " << size_Kbytes << "; " << size_Mbytes << "; " << size_Gbytes << endl;

				delete[] a;

			}
		}
		catch (const std::bad_alloc&) {
			a = new double[gb];
		}

		if(LOGS)
			LOG_MEMFREE << "Dynamic GBytes = " << gb / (1024 * 1024 * 1024 / sizeof(double)) << endl;

		tot_kb += tot_size_gb * sizeof(double) / 1024;

		//MB
		unsigned long long tot_size_mb = 0;;
		double *b;
		unsigned long long mb;

		try {
			for(unsigned long long i = 0; ; i++){
				b = new double[i * 1024 * 1024 / sizeof(double)];
				mb = i * 1024 * 1024 / sizeof(double);

				tot_size_mb = mb;

				long double size_bytes = mb * sizeof(double) + tot_kb * 1024;
				long double size_Kbytes = (long double)size_bytes / (1024);
				long double size_Mbytes = (long double)size_bytes / (1024 * 1024);
				long double size_Gbytes = (long double)size_bytes / (1024 * 1024 * 1024);

				if(LOGS)
					RES << size_bytes << "; " << size_Kbytes << "; " << size_Mbytes << "; " << size_Gbytes << endl;

				delete[] b;

			}
		}

		catch (const std::bad_alloc&) {
			b = new double[mb];
		}
		if(LOGS)
			LOG_MEMFREE << "Dynamic MBytes = " << mb / (1024 * 1024 / sizeof(double)) << endl;

		tot_kb += tot_size_mb * sizeof(double) / 1024;

		//KB
		unsigned long long tot_size_kb = 0;;
		double *c;
		unsigned long long kb;

		try {
			for(unsigned long long i = 0; ; i++){
				c = new double[i * 1024 / sizeof(double)];
				kb = i * 1024 / sizeof(double);

				tot_size_kb = kb;

				long double size_bytes = kb * sizeof(double) + tot_kb * 1024;
				long double size_Kbytes = (long double)size_bytes / (1024);
				long double size_Mbytes = (long double)size_bytes / (1024 * 1024);
				long double size_Gbytes = (long double)size_bytes / (1024 * 1024 * 1024);

				if(LOGS)
					RES << size_bytes << "; " << size_Kbytes << "; " << size_Mbytes << "; " << size_Gbytes << endl;

				delete[] c;

			}
		}

		catch (const std::bad_alloc&) {
			c = new double[kb];
		}
		if(LOGS)
			LOG_MEMFREE << "Dynamic KBytes = " << kb / ( 1024 / sizeof(double)) << endl;

		tot_kb += tot_size_kb * sizeof(double) / 1024;


		//B
		unsigned long long tot_size_by = 0;;
		double *d;
		unsigned long long by;

		try {
			for(unsigned long long i = 0; ; i++){
				d = new double[i];
				by = i;

				tot_size_by = by*sizeof(double);

				long double size_bytes = by + tot_kb * 1024;
				long double size_Kbytes = (long double)size_bytes / (1024);
				long double size_Mbytes = (long double)size_bytes / (1024 * 1024);
				long double size_Gbytes = (long double)size_bytes / (1024 * 1024 * 1024);

				if(LOGS)
					RES << size_bytes << "; " << size_Kbytes << "; " << size_Mbytes << "; " << size_Gbytes << endl;

				delete[] d;

			}
		}

		catch (const std::bad_alloc&) {
			d = new double[by];
		}

		if(LOGS)
			LOG_MEMFREE << "Dynamic Bytes = " << by << endl;

		tot_kb += tot_size_by / 1024;

		long double total_bytes = tot_kb * 1024;
		long double total_Kbytes = tot_kb;
		long double total_Mbytes = (long double)total_bytes / (1024 * 1024);
		long double total_Gbytes = (long double)total_bytes / (1024 * 1024 * 1024);

		if(LOGS){
			LOG_MEMFREE << "/---------------------------------------/" << endl;
			LOG_MEMFREE << "Total Dynamic memory in Bytes = " << total_bytes << endl;
			LOG_MEMFREE << "Total Dynamic memory in KBytes = " << total_Kbytes << endl;
			LOG_MEMFREE << "Total Dynamic memory in MBytes = " << total_Mbytes << endl;
			LOG_MEMFREE << "Total Dynamic memory in GBytes = " << total_Gbytes << endl;
			LOG_MEMFREE << "/---------------------------------------/" << endl;
		}

		//Static
		unsigned long long tot_kb_s = 0;

		// //B
		// unsigned long long tot_size_by_s = 0;;
		// unsigned long long by_s;

		// try {
		// 	RES_s << "Static bytes start" << endl;
		// 	for(unsigned long long i = 1; i <= best.system_memfree - tot_kb; i++){
		// 		double d_s[i];
		// 		by_s = i;

		// 		tot_size_by_s = by_s*8;

		// 		long double size_bytes = by_s + tot_kb_s * 1024;
		// 		long double size_Kbytes = (long double)size_bytes / (1024);
		// 		long double size_Mbytes = (long double)size_bytes / (1024 * 1024);
		// 		long double size_Gbytes = (long double)size_bytes / (1024 * 1024 * 1024);

		// 		RES_s << size_bytes << "; " << size_Kbytes << "; " << size_Mbytes << "; " << size_Gbytes << endl;
		// 	}
		// }

		// catch (...) {
		// }
		// double d_s[by_s];
		// LOG_MEMFREE << "Static Bytes = " << by_s << endl;

		// tot_kb_s += tot_size_by_s / 1024;

		// long double total_bytes_s = tot_kb_s * 1024;
		// long double total_Kbytes_s = tot_kb_s;
		// long double total_Mbytes_s = (long double)total_bytes_s / (1024 * 1024);
		// long double total_Gbytes_s = (long double)total_bytes_s / (1024 * 1024 * 1024);

		// LOG_MEMFREE << "Total Static memory in Bytes = " << total_bytes_s << endl;
		// LOG_MEMFREE << "Total Static memory in KBytes = " << total_Kbytes_s << endl;
		// LOG_MEMFREE << "Total Static memory in MBytes = " << total_Mbytes_s << endl;
		// LOG_MEMFREE << "Total Static memory in GBytes = " << total_Gbytes_s << endl;
		// LOG_MEMFREE << "/---------------------------------------/" << endl;

			
		// if(LOGS){
		// 	LOG_MEMFREE << "SUMMARY memory in Bytes = " << total_bytes_s + total_bytes << endl;
		// 	LOG_MEMFREE << "SUMMARY memory in KBytes = " << total_Kbytes_s + total_Kbytes << endl;
		// 	LOG_MEMFREE << "SUMMARY memory in MBytes = " << total_Mbytes_s + total_Mbytes << endl;
		// 	LOG_MEMFREE << "SUMMARY memory in GBytes = " << total_Gbytes_s + total_Gbytes << endl;
		// 	LOG_MEMFREE << "/---------------------------------------/" << endl;
		// }


		if(best.bench_value < tot_kb + tot_kb_s){
			best.bench_value = tot_kb + tot_kb_s;
		}

		delete[] a;
		delete[] b;
		delete[] c;
		delete[] d;

		
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		if(elapsed.count() >= time_minutes*60){
		    break;
		}
  	}

	if(LOGS){
		RES.close();
		RES_s.close();
		LOG_MEMFREE.close();
	}
	
	return best;
}