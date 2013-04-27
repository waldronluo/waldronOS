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
def put16 ( line ):
	sum = 0 
	for i in range(8):
		sum *=2
		if ( line[i] == '.' ):
			sum += 0;
		if ( line[i] == '*' ):
			sum += 1;
	return str(sum)	


if __name__ == '__main__':
	fi = file ( '../src/hankaku.txt' )
	fo = file ( '../src/hankaku.h' , 'w' )
	fo.write( 'char hankaku[4096] = { ')
	while True: 
		line = fi.readline()
		if len(line) == 0:
			break;
		if (line.startswith( '.' ) or line.startswith('*')) and len(line)==10 : 
			fo.write( put16(line) )
			break;
	
	while True:
		line = fi.readline()
		if len(line) == 0 :
			break;
		if (line.startswith('.') or line.startswith('*')) and len(line)==10  :
			fo.write (',\n' )
			fo.write( put16(line) )

	fo.write( ' };' )
	fo.write( '\n' )
	fi.close()
	fo.close()


