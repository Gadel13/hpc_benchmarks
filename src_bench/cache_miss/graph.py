# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import numpy as np
import csv
from pylab import *

L1size = 14*32*1024;
L2size = 14*256*1024;
L3size = 35*1024*1024;

op = "store"
thread_list = [1, 2, 4, 10, 12, 14]

for thread_num in thread_list:
	thr = "_"+str(thread_num)+"thr"

	filepath1 = op+"_lom2_ResultOMP_ver2_1lvl"+thr+".csv"
	filepath2 = op+"_lom2_ResultOMP_ver2_2lvl"+thr+".csv"
	filepath3 = op+"_lom2_ResultOMP_ver2_3lvl"+thr+".csv"

	maxi1 = 0.0
	max_str1 = []
	maxi2 = 0.0
	max_str2 = []
	maxi3 = 0.0
	max_str3 = []

	with open(filepath1, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        if(float(cur_arr[13]) > maxi1 and (cur_arr[1] == 'i' and L2size/4 == int(cur_arr[4]) or cur_arr[1] == 'd' and L2size/8 == int(cur_arr[4])) ):
	        	max_str1 = cur_arr
	        	maxi1 = float(cur_arr[13])

	with open(filepath2, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        if(float(cur_arr[14]) > maxi2 and (cur_arr[1] == 'i' and L3size/4 == int(cur_arr[4]) or cur_arr[1] == 'd' and L3size/8 == int(cur_arr[4])) ):
	        	max_str2 = cur_arr
	        	maxi2 = float(cur_arr[14])

	with open(filepath3, "r") as file:
	    reader = csv.reader(file)
	    for row in reader:
	        cur_arr = row[0].split(';')
	        if(float(cur_arr[15]) > maxi3):
	        	max_str3 = cur_arr
	        	maxi3 = float(cur_arr[15])
	    
	# print(op, " ", thread_num, " ",maxi1, " ",max_str1)
	# print(op, " ", thread_num, " ",maxi2, " ",max_str2)
	print(op, " ", thread_num, " ",maxi3, " ",max_str3)
