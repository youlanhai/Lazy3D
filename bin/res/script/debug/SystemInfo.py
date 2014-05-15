#-*- coding: utf-8 -*-

import Lazy
import CallBack

from gui import GUI


class SystemInfo(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(SystemInfo, self).__init__(id, parent, image, x, y, w, h)
		self.backColor = 0x9f000000
		self.show(False)
		self.lblOCTreeCache 	= GUI.Label(0, self, 2, 10, "error")
		
		self.lblTimeScale = GUI.Label(0, self, 2, 30, "")
		self.scroll = GUI.Scroll(1, self, "", 50, 50, 100, 20)
		self.scroll.setVertical(False)
		self.scroll.setRate(0.5)
		self.onScroll(1, 0.5)
		
		
	def onShow(self, show_) :
		if show_ :
			self.updateData()
			
	def updateData(self) :
		if not self.isVisible() :
			return
		self.lblOCTreeCache.text = "octree cache size: %d bytes" % Lazy.getOCTreeCacheBytes()
		CallBack.callBack(0.5, self.updateData)
		
	def onScroll(self, id, pos) :
		#print "onScroll", id, pos
		
		if pos > 0.501:
			timeScale = (pos - 0.5) * 100
		elif pos < 0.499:
			timeScale = pos*2
		else:
			timeScale = 1.0
		
		if timeScale < 0.0001 :
			timeScale = 0.0001
		
		Lazy.getApp().setTimeScale(timeScale)
		self.updateTimeScale()
		
	def updateTimeScale(self):
		timeScale = Lazy.getApp().getTimeScale()
		self.lblTimeScale.text = "时间缩放%0.2f倍" % timeScale

