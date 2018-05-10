#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Graphs the chains to histograms
'''

import sys
import numpy as np
import matplotlib.pyplot as plt

def main():
	if len(sys.argv) < 2:
		print('Usage: ./graph.py chain.?')
		exit(1)

	filename = sys.argv[1]
	device_num = filename.split('.')[-1]

	data = []
	with open(filename, 'r') as chain_output:
		for row in chain_output:
			level = 0
			while row[0] == '\t':
				level += 1
				row = row[1:]
			data.append(level)
	
	# histo(data)
	bar(data, device_num)

def bar(data, device_num):
	plt.bar(range(len(data)), data, width=1)
	plt.ylabel('Nested Level')
	plt.title('Device %s' % device_num)
	plt.show()

def histo(data):
	plt.hist(data, normed=False, bins=10)
	plt.ylabel('Nested Level')
	plt.title('Histogram for Device 2')
	plt.show()


if __name__ == '__main__':
	main()
