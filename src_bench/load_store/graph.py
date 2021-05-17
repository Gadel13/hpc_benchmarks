# -*- coding: utf-8 -*-
#import matplotlib.pyplot as plt
import numpy as np
import csv
#from pylab import *

glob_max_load = 0
glob_max_load_op = ""
glob_max_load_thr = 1
glob_max_load_str = ""

glob_max_store = 0
glob_max_store_op = ""
glob_max_store_thr = 1
glob_max_store_str = ""

L1size = 14*32*1024;
L2size = 14*256*1024;
L3size = 35*1024*1024;

load_off = 7
store_off = 6

op1 = "load_op"
op2 = "norand_load_op"

op3 = "store_op"
op4 = "norand_store_op"

op5 = "fibo_load_op"
op6 = "fibo_store_op"

op7 = "list_load_op"
op8 = "list_store_op"
thread_list = [1, 2, 4, 8, 10, 12, 14]
# thread_list = [2, 4, 8, 10, 12, 14]

for thread_num in thread_list:
	thr = "_"+str(thread_num)+"thr"

	filepath1 = op1+"_lom2_ResultOMP_ver2"+thr+".csv"
	filepath2 = op2+"_lom2_ResultOMP_ver2"+thr+".csv"
	filepath3 = op3+"_lom2_ResultOMP_ver2"+thr+".csv"
	filepath4 = op4+"_lom2_ResultOMP_ver2"+thr+".csv"

	filepath5 = op5+"_lom2_ResultOMP_ver2"+thr+".csv"
	filepath6 = op6+"_lom2_ResultOMP_ver2"+thr+".csv"

	filepath7 = op7+"_lom2_ResultOMP_ver2"+thr+".csv"
	filepath8 = op8+"_lom2_ResultOMP_ver2"+thr+".csv"

	maxi1 = 0.0
	max_str1 = []

	with open(filepath1, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - load_off]) > maxi1 ):
	        	max_str1 = cur_arr
	        	maxi1 = float(cur_arr[leng - load_off])

	if glob_max_load < maxi1:
		glob_max_load = maxi1
		glob_max_load_op = op1
		glob_max_load_thr = thread_num
		glob_max_load_str = max_str1

	print(op1, ' ', thread_num, ' ',maxi1, ' ',max_str1)


	maxi2 = 0.0
	max_str2 = []
	with open(filepath2, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - load_off]) > maxi2 ):
	        	max_str2 = cur_arr
	        	maxi2 = float(cur_arr[leng - load_off])

	if glob_max_load < maxi2:
		glob_max_load = maxi2
		glob_max_load_op = op2
		glob_max_load_thr = thread_num
		glob_max_load_str = max_str2

	print(op2, ' ', thread_num, ' ',maxi2, ' ',max_str2)


	maxi5 = 0.0
	max_str5 = []
	with open(filepath5, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - load_off]) > maxi5 ):
	        	max_str5 = cur_arr
	        	maxi5= float(cur_arr[leng - load_off])

	if glob_max_load < maxi5:
		glob_max_load = maxi5
		glob_max_load_op = op5
		glob_max_load_thr = thread_num
		glob_max_load_str = max_str5

	print(op5, ' ', thread_num, ' ',maxi5, ' ',max_str5)

	maxi7 = 0.0
	max_str7 = []
	with open(filepath7, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - load_off]) > maxi7 ):
	        	max_str7 = cur_arr
	        	maxi7= float(cur_arr[leng - load_off])

	if glob_max_load < maxi7:
		glob_max_load = maxi7
		glob_max_load_op = op7
		glob_max_load_thr = thread_num
		glob_max_load_str = max_str7

	print(op7, ' ', thread_num, ' ',maxi7, ' ',max_str7)

	print()


	maxi3 = 0.0
	max_str3 = []

	with open(filepath3, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - store_off]) > maxi3 ):
	        	max_str3 = cur_arr
	        	maxi3 = float(cur_arr[leng - store_off])

	if glob_max_store < maxi3:
		glob_max_store = maxi3
		glob_max_store_op = op3
		glob_max_store_thr = thread_num
		glob_max_store_str = max_str3

	print(op3, ' ', thread_num, ' ',maxi3, ' ',max_str3)

	maxi4 = 0.0
	max_str4 = []
	with open(filepath4, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - store_off]) > maxi4 ):
	        	max_str4 = cur_arr
	        	maxi4 = float(cur_arr[leng - store_off])

	if glob_max_store < maxi4:
		glob_max_store = maxi4
		glob_max_store_op = op4
		glob_max_store_thr = thread_num
		glob_max_store_str = max_str4

	print(op4, ' ', thread_num, ' ',maxi4, ' ',max_str4)

	maxi6 = 0.0
	max_str6 = []
	with open(filepath6, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - store_off]) > maxi6 ):
	        	max_str6 = cur_arr
	        	maxi6 = float(cur_arr[leng - store_off])

	if glob_max_store < maxi6:
		glob_max_store = maxi6
		glob_max_store_op = op6
		glob_max_store_thr = thread_num
		glob_max_store_str = max_str6

	print(op6, ' ', thread_num, ' ',maxi6, ' ',max_str6)

	maxi8 = 0.0
	max_str8 = []
	with open(filepath8, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        leng = len(cur_arr)
	        if(float(cur_arr[leng - store_off]) > maxi8 ):
	        	max_str8 = cur_arr
	        	maxi8 = float(cur_arr[leng - store_off])

	if glob_max_store < maxi8:
		glob_max_store = maxi8
		glob_max_store_op = op8
		glob_max_store_thr = thread_num
		glob_max_store_str = max_str8

	print(op8, ' ', thread_num, ' ',maxi8, ' ',max_str8)
	print()


print("\nGLOBALS:")
print(glob_max_load_op, ' ', glob_max_load_thr, ' ',glob_max_load, ' ',glob_max_load_str)
print(glob_max_store_op, ' ', glob_max_store_thr, ' ',glob_max_store, ' ',glob_max_store_str)
