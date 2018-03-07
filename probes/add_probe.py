#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

def main():
	if len(sys.argv) < 2:
		print('Usage: ./add_probe.py filename.c')
		exit(1)
	
	with open(sys.argv[1]) as cfile:
		# there is a balanced parenthesis problem in here
		for row in cfile:
			print(row)

if __name__ == '__main__':
	main()