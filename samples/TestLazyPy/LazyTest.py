# -*- coding: utf-8 -*-

def doTest():
	print("LazyTest.doTest")
	
	import helper
	print("helper:", dir(helper))
	print("TestClass:", dir(helper.TestClass))
	
	a = helper.TestClass()
	a.testId = 999
	print("a.testId:", a.testId)
	#a.testId = "hello world!"


def test2(arg):
	print("LazyTest.test2:", arg)
