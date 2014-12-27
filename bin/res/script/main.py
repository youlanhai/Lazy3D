# -*- coding: utf-8 -*-

import traceback

import helper
import Lazy

from share import share


def init() :
	print("game init.")
	print("Lazy:", dir(Lazy))

	window = Lazy.createWidget("Window")
	print("window:", window, window.object)

	window.position = (100, 100)
	Lazy.uiroot().addChild(window)

	window.visible = False
	
	return True


def fini() :
	print ("game fini.")

