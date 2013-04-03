#!/usr/bin/python
"""
poem = ''''' \
	p is fun
	phruck it.
	or it will.
	phruck u.
'''

f = file ('poem.txt', 'w')
f.write(poem)
f.close()

f = file('poem.txt')

while True:
	line = f.readline()
	if len(line) == 0:
		break;
	print line;
f.close;
 for test 
"""




