#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

def main():
	if len(sys.argv) < 2:
		print('Usage: ./filter.py op.chain')
		exit(1)

	filename = sys.argv[1]
	with open(filename, 'r') as chain_input:
		for row in chain_input:
			if '=' in row or '*' in row:
				print(row.strip())

if __name__ == '__main__':
	main()