#coding: utf-8

#测试 lazy py
def doTest():
	import Lazy
	Lazy.print('hello python!')

	import noddy2
	a = noddy2.Noddy('you', 'lanhai', 23)
	a.last = 'lan2hai'
	a.number = 123
	print('name:', a.name(), 'number:', a.number)

	import Lazy
	print("dir Lazy:", dir(Lazy))
	Lazy.hello()

	a = Lazy.SubClass()
	print("dir a:", dir(a))

	a.id = 1
	print("a.id = ", a.id)
	print("a.data = ", a.data)
	a.hello('hello python!')

	print('Test End')

def test2(str):
	print("From Python:", str)
