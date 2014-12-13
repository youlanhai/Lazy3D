# -*- coding: utf-8 -*-

import traceback

import helper
import Lazy

from share import share


def init() :
	print("game init.")
	print("lui:", dir(Lazy))
	
	return True


def fini() :
	print ("game fini.")

