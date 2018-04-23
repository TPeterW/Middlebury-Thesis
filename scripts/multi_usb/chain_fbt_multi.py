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
		current_process = -1

		current_func = []

#         current_funcs = {}
		indent_levels = {}

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
					for i in range(indent_levels.get(pid, 0)):
						outputfile.write('\t')
					outputfile.write('====================Switched to process %s====================\n' % (pid))
					current_process = pid
				for i in range(indent_levels.get(pid, 0)):
					outputfile.write('\t')

				if ('enqueue' in funcname or 'dequeue' in funcname) and len(splitted) > 7:
					payload_id = payload_dict.get(splitted[-3], None)
					if payload_id == None:
						payload_id = len(payload_dict)
						payload_dict[splitted[-3]] = payload_id
					queue_id = queue_dict.get(splitted[-2], None)
					if queue_id == None:
						queue_id = len(queue_dict)
						queue_dict[splitted[-2]] = queue_id
					device_id = int(splitted[-1]) % 128
					outputfile.write('********************%s Payload %s from Device %s at Queue %s********************\n' % ('Adding' if 'enqueue' in funcname else 'Popping', payload_id, device_id, queue_id))
					continue

				outputfile.write('-> %s\t%s\n' % (funcname, pid))
				indent_levels[pid] = indent_levels.get(pid, 0) + 1
#                 current_funcs.setdefault(pid, []).append(funcname)
				current_func.append(funcname)
			else:
				if ('enqueue' in funcname or 'dequeue' in funcname) and len(splitted) > 7:
					continue

#                 if len(current_funcs[pid]) <= 0:
				if len(current_func) <= 0:
					continue
#                 if current_funcs[pid][-1] == funcname:
				if current_func[-1] == funcname:
					if current_process != pid:
						for i in range(indent_levels.get(pid, 0)):
							outputfile.write('\t')
						outputfile.write('====================Switched to process %s====================\n' % (pid))
						current_process = pid
					indent_levels[pid] = indent_levels.get(pid) - 1
					for i in range(indent_levels.get(pid)):
						outputfile.write('\t')

					outputfile.write('<- %s\t%s\n' % (funcname, pid))
#                     current_funcs[pid] = current_funcs[pid][:-1]
					current_func = current_func[:-1]


if __name__ == '__main__':
	main()
