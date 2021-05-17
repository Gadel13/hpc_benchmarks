#define BENCHMARK "OSU MPI%s Bandwidth Test"
/*
 * Copyright (C) 2002-2021 the Network-Based Computing Laboratory
 * (NBCL), The Ohio State University. 
 *
 * Contact: Dr. D. K. Panda (panda@cse.ohio-state.edu)
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level OMB directory.
 */

#include <osu_util_mpi.h>

#ifdef _ENABLE_CUDA_
double measure_kernel_lo(char **, int, int);
void touch_managed_src(char **, int, int);
void touch_managed_dst(char **, int, int);
#endif
double calculate_total(double, double, double, int);

int
osu_bw ();

double measure_kernel_lo(char **buf, int size, int window_size);

void touch_managed_src(char **buf, int size, int window_size);

void touch_managed_dst(char **buf, int size, int window_size);

double calculate_total(double t_start, double t_end, double t_lo, int window_size);
