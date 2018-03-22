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

	with open(filename[:-4] + '.chain', 'w') as outputfile:
		current_func = []
		current_process = -1
		indent_level = 0

		queue_dict = {}
		payload_dict = {}

		for entry in lines:
			splitted = entry.split()

			funcname = splitted[5]
			pid = int(splitted[4])
			provider = splitted[3]
			functype = splitted[2].split(':')[1]

			if functype == 'entry':
				if current_process != pid:
					for i in range(indent_level):
						outputfile.write('\t')
					outputfile.write('====================Switched to process %s====================\n' % (pid))
					current_process = pid
				for i in range(indent_level):
					outputfile.write('\t')

				if ('enqueue' in funcname or 'dequeue' in funcname) and len(splitted) > 6:
					payload_id = payload_dict.get(splitted[-2], None)
					if payload_id == None:
						payload_id = len(payload_dict)
						payload_dict[splitted[-2]] = payload_id
					queue_id = queue_dict.get(splitted[-1], None)
					if queue_id == None:
						queue_id = len(queue_dict)
						queue_dict[splitted[-1]] = queue_id
					outputfile.write('********************%s Payload %s at Queue %s********************\n' % ('Adding' if 'enqueue' in funcname else 'Popping', payload_id, queue_id))
					continue

				outputfile.write('-> %s\t%s\n' % (funcname, pid))
				indent_level += 1
				current_func.append(funcname)
			else:
				if ('enqueue' in funcname or 'dequeue' in funcname) and len(splitted) > 6:
					continue

				if len(current_func) <= 0:
					continue
				if current_func[-1] == funcname:
					if current_process != pid:
						for i in range(indent_level):
							outputfile.write('\t')
						outputfile.write('====================Switched to process %s====================\n' % (pid))
						current_process = pid
					indent_level -= 1
					for i in range(indent_level):
						outputfile.write('\t')

					outputfile.write('<- %s\t%s\n' % (funcname, pid))
					current_func = current_func[:-1]


if __name__ == '__main__':
	main()