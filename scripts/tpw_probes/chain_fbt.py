#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

def main():
	if len(sys.argv) < 2:
		print('Usage: ./chain_fbt.py dtrace_output.txt')
		exit(1)
	
	filename = sys.argv[1]
	lines = []
	with open(filename, 'r') as dtrace_out:
		for row in dtrace_out:
			if len(row.strip()) != 0:
				lines.append(row.strip())
	
	lines = lines[2:-1]

	current_func = []
	indent_level = 0
	for entry in lines:
		splitted = entry.split()

		funcname = splitted[5]
		pid = splitted[4]
		provider = splitted[3]
		functype = splitted[2].split(':')[1]

		if functype == 'entry':
			for i in range(indent_level):
				print('\t', end='')
			print('-> %s' % funcname)
			indent_level += 1
			current_func.append(funcname)
		else:
			if len(current_func) <= 0:
				continue
			if current_func[-1] == funcname:
				indent_level -= 1
				for i in range(indent_level):
					print('\t', end='')
				print('<- %s' % funcname)
				current_func = current_func[:-1]


if __name__ == '__main__':
	main()