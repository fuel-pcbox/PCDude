#!/usr/bin/python
from os import *
from string import *
import io

thead=u"""
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
<head>
<title>Opcodes map</title>
<meta name="keywords" content="opcode pcdude">
<meta name="description" content="">
<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8">
<meta http-equiv="content-style-type" content="text/css">
<meta http-equiv="expires" content="0">
<link rel="StyleSheet" href="map.css" type="text/css"></link>
</head>
<body>

<a href="http://www.mlsite.net/8086/" target="_blank">Source</a>

"""
tend=u"""
</body>
</html>
"""

def main():
	fx = io.open("opcodes.txt")
	ops= fx.readlines()
	fx.close()
	
	a = [thead]
	
	for l in ops:
		opa= l[0]
		op = l[1:-1]
		a += [u"""<p class="done%c">%s</p><br />\n""" % (opa,op)]
		pass
	
	a += [tend]
	
	fx = io.open("opcodes.html","w")
	fx.writelines(a)
	fx.flush()
	fx.close()
	
	pass

main()

