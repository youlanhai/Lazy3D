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
		
	except:
		print(traceback.print_exc())


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
