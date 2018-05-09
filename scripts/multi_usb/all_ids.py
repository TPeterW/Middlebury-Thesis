#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Find all device ID's
'''

import sys

def main():
	if len(sys.argv) < 2:
		print('Usage: ./all_ids.py output.chain')
		exit(1)
	
	filename = sys.argv[1]

	with open(filename, 'r') as chain:
		ids = set()
		for row in chain:
			if '*' in row or '=' in row:
				continue
			ids.add(row.strip().split()[-1])
	ids.remove('-1')
	print(ids)

if __name__ == '__main__':
	main()