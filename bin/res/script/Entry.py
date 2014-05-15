#-*- coding: utf-8 -*-

from tools import Redirect

def enter() :
	Redirect.load()
	print("----enter script")
	
	import Game
	Game.initScript()
	return True
		
def leave() :
	import Game
	Game.releaseScript()
	
	print("----leave script")
	Redirect.unload()