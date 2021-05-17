#pragma once

#include<iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <signal.h>

#include <sys/sysinfo.h>
#include "../../utils.hpp"
using namespace std;

struct memfree_best_result{
	long long system_memfree;
	long long system_buf_cache;
	long long bench_value;
};

memfree_best_result memfree(long long time_minutes);