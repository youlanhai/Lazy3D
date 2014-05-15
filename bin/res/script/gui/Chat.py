#-*- coding: utf-8 -*-

import Lazy
import CallBack
from . import GUI

colors = [0xffff0000, 0xdfff0000, 0xafff0000, 0x7fff0000, 0x2fff0000]
colorCount = 5


class ChatMsg(object) :
	def __init__(self, msg, time) :
		self.label = None
		self.msg = msg
		self.time = time
		self.index = 0
		
	def resetLabel(self, label) :
		if self.time <= 0 :
			return
		self.index += 1
		self.label = label
		self.update(self.time, self.index)
		
	def update(self, time, index) :
		self.time = time
		if not self.label :
			return
		if self.index != index :
			return
		if time <= 0 :
			self.label.text = ""
			return
		self.label.text = self.msg
		idx = int(colorCount - time)
		if idx < 0 :
			idx = 0
		elif idx >= colorCount :
			idx = colorCount-1
		self.label.color = colors[idx]
		CallBack.callBack(0.2, self.update, time-1, index)

class ChatClientMsg(GUI.Panel) :
	def __init__(self, id, parent, x, y) :
		super(ChatClientMsg, self).__init__(id, parent, x, y)
		self.updateIndex = 0
		self.messages = []
		self.labels = []
		y = 0
		for i in range(5) :
			lbl = GUI.Label(0, self, 0, y, "")
			lbl.setSize(Lazy.getApp().width, 30)
			lbl.setAlign("center")
			self.labels.append(lbl)
			y += 24
			
	def chatMsg(self, msg) :
		self.addMsg(msg)

	def addMsg(self, msg) :
		obj = ChatMsg(msg, 5)
		if len(self.messages) >= 5 :
			self.messages.pop(0)
		self.messages.append(obj)
		self.updateLabel()
	
	def updateLabel(self) :
		lenMsg = len(self.messages)
		for i, msg in enumerate(self.messages) :
			msg.resetLabel(self.labels[lenMsg-i-1])
			
##################################################
class ChatClientMsgEx(GUI.Panel) :
	def __init__(self, id, parent, x, y) :
		super(ChatClientMsgEx, self).__init__(id, parent, x, y)
		self.updateIndex = 0
		self.messages = []
		self.label = GUI.Label(0, self, 0, 10, "")
		self.label.setSize(Lazy.getApp().width, 30)
		self.label.setAlign("center")
		self.dt = 0.05
		self.fadeInTime = 2
		self.fadeOutTime = 3
			
	def chatMsg(self, msg) :
		self.label.text = msg
		self.updateIndex += 1
		self.fadeIn(0, self.updateIndex)
		
	def fadeIn(self, elapse, index) :
		if index != self.updateIndex :
			return
		if elapse > self.fadeInTime :
			CallBack.callBack(2, self.fadeOut, 0, index)
		else :
			alph = int(255.0*elapse/self.fadeInTime)
			self.label.color = alph<<24 | 0x00ff0000
			CallBack.callBack(self.dt, self.fadeIn, elapse+self.dt, index)
		
	def fadeOut(self, elapse, index) :
		if index != self.updateIndex :
			return
		if elapse > self.fadeOutTime :
			CallBack.callBack(0, self.hide, index)
		else :
			alph = int(255.0*(self.fadeOutTime-elapse)/self.fadeOutTime)
			self.label.color = alph<<24 | 0x00ff0000
			CallBack.callBack(self.dt, self.fadeOut, elapse+self.dt, index)

	def hide(self, index) :
		if index != self.updateIndex :
			return
		self.label.text = ""
