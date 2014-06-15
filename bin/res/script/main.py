# -*- coding: utf-8 -*-

import traceback

import lui
import ui

from share import share


def init() :
	print("game init.")
	print("lui:", dir(lui))
	
	try :
		import gui
		lui.regUIFactory(gui.createUIFromStream)

		import guimgr
		share.gui = guimgr.guimgr()

		import Lazy
		from avatar import Avatar
		playerID = Lazy.createEntity(Avatar)
		Lazy.setPlayer(Lazy.entity(playerID))
		
		Lazy.map().loadMap("map/test/map.lzd")

	except:
		print(traceback.print_exc())
		
	return True


def fini() :
	try :
		
		share.guimgr = None
		lui.root().clearChildren()
		
	except:
		print(traceback.print_exc())

	print ("game fini.")



#窗口改变大小
def onSize(w, h):
	print("python onSize", w, h)
	ui.onSize(w, h)
	share.gui.editor.setSize(w, h)

def onClientSize(w, h):
	ui.onClientSize(w, h)

def onKeyEvent(isDown, key):
	print("onKeyEvent", isDown, key)
	
	return share.gui.onKeyEvent(isDown, key)
