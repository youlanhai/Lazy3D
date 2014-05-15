#-*- coding: utf-8 -*-
import sys
import os
import Lazy

g_oldStdout = sys.stdout
g_oldStderr = sys.stderr

class Redirect(object):
	def __init__(self):
		self.cache = ""
		self.out = Lazy.debug
		
	def init(self) :
		sys.stdout = self
		sys.stderr = self
		
	def write(self, s):
		if s == "\n" :
			self.out(self.cache)
			self.cache = ""
		else :
			self.cache += s
	
	def release(self):
		global g_oldStdout, g_oldStderr
		sys.stdout = g_oldStdout
		sys.stderr = g_oldStderr
		

g_redirect = Redirect()

def load() :
	g_redirect.init()

def unload() :
	g_redirect.release()

def startDbgView() :
	try :
		os.spawnv(0, "Dbgview", ["1"])
	except :
		pass
