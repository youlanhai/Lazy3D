# -*- coding: utf-8 -*-

import lui
import gui

from share import share

ID_CLOSE = 1
ID_EXIT = 2
ID_OPEN = 3
ID_SAVE = 4

EVENT_MAP = {}

def menu_event(ID):

	def fun(method):
		EVENT_MAP[ID] = method
		return method

	return fun

@menu_event(ID_OPEN)
def onOpen():
	pass

@menu_event(ID_SAVE)
def onSave():
	pass

@menu_event(ID_CLOSE)
def onClose():
	pass

@menu_event(ID_EXIT)
def onExit():
	pass


class MenuItem(gui.IListItem):

	SCRIPT_NAME = "uieditor.edt_menu.MenuItem"

	def __init__(self, parent=None):
		super(MenuItem, self).__init__(parent)
		self.menuID = 0

	def setInfo(self, info):
		self.menuID, text = info
		self.label.text = text

	def onSelect(self):
		print("onSelect", self.label.text, self.menuID)
		if self.msgHandler:
			self.msgHandler.onItemSelect(self.key)
		
		EVENT_MAP[self.menuID]()


class MenuBar(lui.Form):

	def __init__(self, parent=None):
		super(MenuBar, self).__init__(parent)


	def onLoadLayout(self, config):
		self.file = self.getChildByName("file")
		self.file.menu.setItemCreateMethod(MenuItem.createUI)
		infos = (
			(ID_OPEN, "open"),
			(ID_SAVE, "save"),
			(ID_CLOSE, "close"),
			(ID_EXIT, "exit"),
		)
		self.file.setInfo(infos)


