# -*- coding: utf-8 -*-

import lui
import gui
import weakref

DEFAULT_PATH = "res/"

def list_dir(path):
	import os
	return os.listdir(DEFAULT_PATH + path)

def is_dir(path):
	import os
	return os.path.isdir(DEFAULT_PATH + path)

def normal_path(path):
	import os
	path = os.path.normpath(path)
	return path.replace('\\', '/')

def get_file_ext(filename):
	ret = filename.split('.')
	return ret[-1] if len(ret) > 1 else ""

def format_path_name(path):
	if len(path) == 0: return path

	path = path.replace('\\', '/')
	if path[-1] != '/': path += '/'

	return path

##################################################
###
##################################################
class FileExplorer(lui.Form):

	def __init__(self):
		super(FileExplorer, self).__init__()
		self.onCloseCallback = None
		self.filters = None
		self.selectedPath = ""

	def onLoadLayout(self, config):
		self.listView = self.getChildByName("list")
		self.listView.setMsgHandler( weakref.proxy(self) )

		btn = self.getChildByName("btn_ok")
		if btn: btn.onButtonClick = gui.MethodProxy(self, "onBtnOK")

		btn = self.getChildByName("btn_cancel")
		if btn: btn.onButtonClick = gui.MethodProxy(self, "onBtnCacnel")
	
	def listdir(self, path, callback=None, filters=None):
		self.onCloseCallback = callback
		self.filters = filters
		self._listdir(path)

	def _listdir(self, path):
		path = format_path_name(path)
		infos = self._collectDirInfo(path, self.filters)
		self.listView.setInfo(infos)

	def onPathSeleted(self, path):
		self.selectedPath = normal_path(path)

		if is_dir(self.selectedPath):
			self.listdir(self.selectedPath)
			self.selectedPath += '/'

		lbl = self.getChildByName("lbl_path")
		if lbl: lbl.text = self.selectedPath

	def onBtnCacnel(self):
		self.closeWithCode(False)

	def onBtnOK(self):
		self.closeWithCode(True)

	def closeWithCode(self, code):
		if self.onCloseCallback:
			self.onCloseCallback(code, self.selectedPath)

		self.onCloseCallback = None
		self.selectedPath = ""
		self.visible = False

	@staticmethod
	def _collectDirInfo(path, filters):
		dirFiles = list_dir(path)

		paths = ["../"]
		files = []
		for name in dirFiles:
			if is_dir(path + name):
				paths.append(name + '/')
			else:
				if filters and get_file_ext(name) not in filters:
					continue
				files.append(name)

		paths.extend(files)
		infos = [(x, path + x) for x in paths]
		return infos



class ListItem(gui.IListItem):

	def __init__(self):
		super(ListItem, self).__init__()
		self.path = ""

	def setInfo(self, info):
		btn = self.getChildByName("btn")
		if btn is None: return

		name, self.path = info
		btn.text = name
		btn.onButtonClick = gui.MethodProxy(self, "onBtnOpen")

	def onBtnOpen(self):
		if len(self.path) == 0: return

		self.msgHandler.onPathSeleted(self.path)
