#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Formulate a pattern for each device
'''

import sys

def main():
	if len(sys.argv) < 3:
		print('Usage: ./formulate.py output.chain device_id')
		exit(1)

	filename = sys.argv[1]
	device_id = sys.argv[2]

	with open(filename, 'r') as chain, open(filename[:-6] + '.' + device_id, 'w') as result:
		for row in chain:
			if row.strip().endswith(device_id):
				result.write(row)
	

if __name__ == '__main__':
	main()