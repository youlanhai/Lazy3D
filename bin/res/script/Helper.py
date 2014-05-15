#-*- coding: utf-8 -*-

def formatVector3(vec) :
	return "(%0.1f %0.1f %0.1f)" % (vec.x, vec.y, vec.z)
	
def formatVector3Ex(prefix, vec) :
	return prefix+formatVector3(vec)
