#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

def main():
	if len(sys.argv) < 2:
		print('Usage: ./chain_fbt.py dtrace_output.txt')
		exit(1)
	
	filename = sys.argv[1]

	with open(filename, 'r') as dtrace_out:
		for row in dtrace_out:
			row = row.strip()
			print(row)
	

if __name__ == '__main__':
	main()