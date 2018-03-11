#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import clang.cindex

types = ['void', 'int', '_t']

def main():
	if len(sys.argv) < 2:
		print('Usage: ./add_probe.py filename.c')
		exit(1)

	new_cfile = []
	with open(sys.argv[1], 'r') as cfile:
		func_name = None
		in_function = False
		func_begin = True
		name_on_next_line = False
		left_paren = 0

		for row in cfile:
			new_cfile.append(row)
			
			if name_on_next_line:
				func_name = row.split('(')[0]
				name_on_next_line = False

			# Add the entry probes
			if func_begin and row == '{\n':
				new_cfile.append('HERE\n')
				func_begin = False

			for t in types:
				if row.endswith(t + '\n'):
					if t == 'int' and len(row.split(' ')[-1]) > 3:
						continue
					func_begin = True
					in_function = True
					name_on_next_line = True
					break



			# another condition to set in_function to False
	
	new_cfile = ''.join(new_cfile)
	with open('output.c', 'w') as out_cfile:
		out_cfile.write(new_cfile)

	# index = clang.cindex.Index.create()
	# tu = index.parse(sys.argv[1])

	# node = tu.cursor
	# for child in node.get_children():
	# 	print(child.type.spelling)
		# print(child.spelling, child.type.spelling)
	
if __name__ == '__main__':
	main()