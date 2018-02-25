#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
from itertools import groupby, count

def main():
	if len(sys.argv) < 2:
		print('Usage: ./consolidate.py probes.list')
		exit(1)

	ids = []
	with open(sys.argv[1]) as probefile:
		for row in probefile:
			ids.append(row.split()[0])	
	
	res = ','.join(as_range(g) for _, g in groupby(ids, key=lambda n, c=count(): int(n)-next(c)))

	predicate = '/'
	for r in res.split(','):
		if '-' in r:
			predicate += '(id >= '	+ r.split('-')[0] + ' && id <= ' + r.split('-')[1] + ') || '
		else:
			predicate += '(id == ' + r + ') || '
	predicate = predicate[:-4] + '/'
 
	print(predicate)

def as_range(iterable): 
    l = list(iterable)
    if len(l) > 1:
        return '{0}-{1}'.format(l[0], l[-1])
    else:
        return '{0}'.format(l[0])

if __name__ == '__main__':
	main()

