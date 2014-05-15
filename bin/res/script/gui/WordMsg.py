#-*- coding: utf-8 -*-

from . import GUI

class TextView(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h, dh):
		self.rows = h // dh
		h = self.rows * dh
		super(TextView, self).__init__(id, parent, image, x, y, w, h)
		self.backColor = 0x00ffffff
		self.labels = []
		self.data = []
		for i in range(self.rows) :
			self.labels.append(GUI.Label(0, self, 0, h-(i+1)*dh, color=0xffff0000))
			
	def setData(self, data) :
		if isinstance(data, list):
			self.data = data
		else :
			self.data = []
		self.updateData()
				
	def updateData(self) :
		if len(self.data) > 100 :
			self.data.pop()
		for i,v in enumerate(self.labels) :
			if i < len(self.data):
				v.text = self.data[i]
			else :
				v.text = ""
				
	def addText(self, txt) :
		self.data.insert(0, str(txt))
		self.updateData()
			

class WordMsg(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h):
		super(WordMsg, self).__init__(id, parent, image, x, y, w, h)
		self.enableHandleMsg(False)
		self.backColor = 0x5f000000
		self.title = GUI.Label(0, self, 0, 2, "系统提示信息", color=0xffff00ff)
		self.title.setSize(w, 20)
		self.title.setAlign("center")
		self.textView = TextView(1, self, "", 2, 20, w-4, h-20, 20)
		
	def addMsg(self, msg) :
		self.textView.addText(msg)