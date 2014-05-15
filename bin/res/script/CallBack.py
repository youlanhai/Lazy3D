#coding = utf-8

import Lazy

def callback(time, call, *args) :
	return Lazy.callback(time, call, args)

class Functor(object) :
	def __init__(self, func, *args) :
		print("Functor", func, args)
		self.func = func
		self.args = args
		
	def __call__(self) :
		self.func(*self.args)