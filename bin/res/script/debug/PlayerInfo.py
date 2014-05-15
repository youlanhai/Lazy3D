#-*- coding: utf-8 -*-

import Lazy
import CallBack
import Helper

from gui import GUI

from GData import GData

class PlayerInfo(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(PlayerInfo, self).__init__(id, parent, image, x, y, w, h)
		self.lblPosition 	= GUI.Label(0, self, 2, 10, "pos")
		self.lblLook 		= GUI.Label(0, self, 2, 30, "look")
		self.lblUp 			= GUI.Label(0, self, 2, 50, "up")
		self.lblRight 		= GUI.Label(0, self, 2, 70, "right")
		self.lblDistToPlayer = GUI.Label(0, self,2, 90, "dist")
		self.lblCursorPos 	= GUI.Label(0, self, 2, 110, "cursor")
		self.enableDrag(False)
		self.backColor = 0x9f000000
		self.show(False)
		
	def onShow(self, show_) :
		if show_ :
			self.updateData()
			
	def getTarget(self) :
		if GData.targetLocked :
			return GData.targetLocked
		else :
			return GData.player
		
	def updateData(self) :
		if not self.isVisible() :
			return
		target = self.getTarget()
		if target :
			self.lblPosition.text 	= Helper.formatVector3Ex("  pos", target.position)
			self.lblLook.text 		= Helper.formatVector3Ex(" look", target.look)
			self.lblUp.text 		= Helper.formatVector3Ex("   up", target.up)
			self.lblRight.text 		= Helper.formatVector3Ex("right", target.right)
			self.lblDistToPlayer.text = "distToP:%.2f" % target.distToPlayer()
		pos = Lazy.getApp().getCursorPos()
		self.lblCursorPos.text = "cursor(%d, %d)" % (pos.x, pos.y)
		CallBack.callBack(0.5, self.updateData)
		

