#-*- coding: utf-8 -*-

#import Lazy
from . import GUI

from GData import GData

class TransForm(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(TransForm, self).__init__(id, parent, image, x, y, w, h)
		self.title = GUI.Label(1, self, 0, 10, "传送阵")
		self.title.setSize(w, 20)
		self.title.setAlign("center")
		self.title.setFont("宋体/22/0/b")
		self.title.color = 0xffff00ff
		self.btnClose = GUI.Button(2, self, "X", "", w-30, 10, 20, 20)
		self.btnTrans = GUI.Button(3, self, "传送", "", (w-50)//2, 100, 50, 30)
		
		self.show(False)

	def onButton(self, id, isDown) :
		if id == 2 :
			self.show(False)
		elif id == 3:
			GData.guiMgr.map.show(True)
			self.show(False)