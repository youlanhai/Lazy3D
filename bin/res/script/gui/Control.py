#-*- coding: utf-8 -*-

import Lazy
#from GData import GData

class Control(Lazy.iControl) :
	def __init(self, id, parent, x, y) :
		print("Control init")
		super(Control, self).__init__()
		self.init(id, parent, "", x, y)
		
	def handlMouseEvent(self, msg, point, param) :
		pass

	def handleKeyEvent(self, key, isDown) :
		pass


class GuiMgr(Lazy.cGUIManager) :
	def __init__(self) :
		print("GuiMgr init ")
		super(Lazy.cGUIManager)

	def handlMouseEvent(self, msg, point, param) :
		pass
		
	def handleKeyEvent(self, key, isDown) :
		pass
