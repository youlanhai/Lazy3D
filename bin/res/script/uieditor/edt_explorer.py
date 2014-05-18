# -*- coding: utf-8 -*-

import lui
import gui
import os

DEFAULT_PATH = "res/"

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

	def onLoadLayout(self, config):
		self.listView = self.getChildByName("list")
	
	def _listdir(self, path, filters):

		infos = []
		files = os.listdir(path)

		for name in files:
			fullname = path + name
			if os.path.isdir(fullname):
				fullname += '/'
				name += '/'
			else:
				if filters and get_file_ext(name) not in filters: continue
			infos.append((name, fullname))

		infos.insert(0, ("../", path + "../"))
		return infos

	def listdir(self, path, filters=None):
		path = format_path_name(path)
		infos = self._listdir(DEFAULT_PATH + path, filters)
		self.listView.setInfo(infos)


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
		print("onBtnOpen", self.path)

		if os.path.isdir(self.path):
			pass
		else:
			pass
