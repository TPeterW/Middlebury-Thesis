#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

types = ['void', 'int', '_t', ' *']

def main():
	if len(sys.argv) < 3:
		print('Usage: ./add_probe.py header.h path/to/directory ...')
		print('Usage: ./add_probe.py --revert path/to/directory')
		exit(1)

	path = sys.argv[2]

	# Revert all changes made, back to a no-probe world
	if sys.argv[1] == '--revert':
		all_files = os.listdir(path)
		for source_file in all_files:
			if source_file.endswith('.orig'):
				os.rename(os.path.join(path, source_file), os.path.join(path, source_file)[:-5])
		return

	# Find all the C source files
	header_file = sys.argv[1]
	all_source_files = os.listdir(sys.argv[2])
	source_files = []
	for original_source_file in all_source_files:
		if original_source_file.endswith('.c'):
			source_files.append(original_source_file)

	defined_probes = set()
	declared_probes = set()
	# For every C source file
	for source_file in source_files:
		new_cfile = []
		old_cfile = []			# backup of the original file
		with open(os.path.join(path, source_file), 'r') as cfile:
			func_name = None
			in_function = False
			func_begin = True
			name_on_next_line = False
			levels_down = 0
			return_void = False
			line_num = 0

			new_cfile.append('#include <dev/usb/usb_tpw_probe_declare.h>\n')

			for row in cfile:
				line_num += 1
				old_cfile.append(row)

				if row.strip().endswith('{'):
					levels_down += 1
				if row.strip().startswith('}'):
					levels_down -= 1
					if levels_down == 0:
						if return_void:
							probe_use = '\tSDT_PROBE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
							probe_declare = '\tSDT_PROBE_DECLARE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
							probe_defined = '\tSDT_PROBE_DEFINE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
							new_cfile.append(probe_use)
							declared_probes.add(probe_declare)
							defined_probes.add(probe_defined)
						in_function = False
						return_void = False			# Stop tracking return type
						new_cfile.append(row)
						continue

				# Return probes
				if in_function and row.strip().startswith('return'):
					probe_use = '\tSDT_PROBE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
					probe_declare = '\tSDT_PROBE_DECLARE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
					probe_defined = '\tSDT_PROBE_DEFINE(tpw, kernel, %s, return);\n' % (source_file[:-2] + '_' + func_name)
					new_cfile.append(probe_use)
					declared_probes.add(probe_declare)
					defined_probes.add(probe_defined)
					
				# Return probe has to be added before return statement
				new_cfile.append(row)

				if name_on_next_line:
					func_name = row.split('(')[0]
					name_on_next_line = False

				# Entry probes
				if func_begin and row == '{\n':
					probe_use = '\tSDT_PROBE(tpw, kernel, %s, entry);\n' % (source_file[:-2] + '_' + func_name)
					probe_declare = '\tSDT_PROBE_DECLARE(tpw, kernel, %s, entry);\n' % (source_file[:-2] + '_' + func_name)
					probe_defined = '\tSDT_PROBE_DEFINE(tpw, kernel, %s, entry);\n' % (source_file[:-2] + '_' + func_name)
					new_cfile.append(probe_use)
					declared_probes.add(probe_declare)
					defined_probes.add(probe_defined)

					func_begin = False

				if not in_function:
					for t in types:
						if row.endswith(t + '\n'):
							if t == 'int' and len(row.split(' ')[-1]) > 4:
								continue
							if t == ' *' and len(row.strip()) == 1:
								continue
							if t == 'void':
								return_void = True
							func_begin = True
							in_function = True
							name_on_next_line = True
							break
		
		
		new_cfile = ''.join(new_cfile)
		os.rename(os.path.join(path, source_file), os.path.join(path, source_file + '.orig'))
		with open(os.path.join(path, source_file), 'w') as out_cfile:
			out_cfile.write(new_cfile)

		with open(os.path.join(path, 'usb_tpw_probe_declare.h'), 'w') as declare_file:
			declare_file.write('#include <sys/sdt.h>\n')
			declare_file.write('SDT_PROVIDER_DECLARE(tpw);\n')
			declare_file.write(''.join(list(declared_probes)))

		with open(os.path.join(path, 'usb_tpw_probe.h'), 'w') as define_file:
			define_file.write('#include <sys/sdt.h>\n')
			define_file.write('#ifndef USB_TPW_PROBE\n')
			declare_file.write('SDT_PROVIDER_DEFINE(tpw);\n')
			define_file.write(''.join(defined_probes))
			define_file.write('#define USB_TPW_PROBE\n')
			define_file.write('#endif\n')
	
if __name__ == '__main__':
	main()