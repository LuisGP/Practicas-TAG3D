#/usr/bin/env python
import sys
import os

if(len(sys.argv) < 3):
	print "[1] lista coordenadas UV (VRML)"
	print "[2] fichero de salida"
	exit(1)

f1 = open (sys.argv[1], 'r')
coords = f1.readlines()
f1.close()
o = open (sys.argv[2], 'w')
coordsParsed = []
coordsValues = []

out = ""
line=0
for l in coords :
	line+=1
	words = l.split(' ')
	if "texCoord" in words :
		break
coords = coords[line:]

coords = coords[4:]
line = 0

par = 0
for l in coords :
	line+=1
	l = l.strip()
	if ']' in l :
		l = l.split(']')[0]
		values = l.split(' ')
		for i in values:
		  out += i + " "
		  if par:
		    out +='\n'
		  par = 1-par
		    
		break
	values = l.split(' ')
	for i in values:
	  out += i + " "
	  if par:
	    out +='\n'
	  par = 1-par

while '' in coordsParsed:
	coordsParsed.remove('')

out = out.replace("\n ","");
print "("+out+")"
o.write(out)
o.close()

exit(0)


