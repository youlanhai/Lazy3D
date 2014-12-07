# -*- coding: utf-8 -*-

import traceback

import helper
import lui
import Lazy

from share import share


def init() :
	print("game init.")
	print("lui:", dir(lui))
	
	return True


def fini() :
	print ("game fini.")

