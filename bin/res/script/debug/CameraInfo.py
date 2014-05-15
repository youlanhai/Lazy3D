#-*- coding: utf-8 -*-

#import Lazy
import CallBack
import Helper

from gui import GUI

from GData import GData

class CameraInfo(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w=200, h=150) :
		super(CameraInfo, self).__init__(id, parent, image, x, y, w, h)
		self.lblPosition 	= GUI.Label(0, self, 2, 10, "pos")
		self.lblLook 		= GUI.Label(0, self, 2, 30, "look")
		self.lblUp 			= GUI.Label(0, self, 2, 50, "up")
		self.lblRight 		= GUI.Label(0, self, 2, 70, "right")
		self.lblRoll		= GUI.Label(0, self, 2, 90, "roll")
		self.lblPitch		= GUI.Label(0, self, 2, 110, "pitch")
		self.lblYaw			= GUI.Label(0, self, 2, 130, "yaw")
		self.backColor = 0x9f000000
		self.show(False)
		
	def onShow(self, show_) :
		if show_ :
			self.updateData()
			
	def getTarget(self) :
		return GData.camera
		
	def updateData(self) :
		if not self.isVisible() :
			return
			
		target = self.getTarget()
		#print "updateData", target
		if target :
			self.lblRoll.text 		= " roll: %.2f" % target.roll
			self.lblPitch.text 		= "pitch: %.2f" % target.pitch
			self.lblYaw.text 		= "  yaw: %.2f" % target.yaw
			self.lblPosition.text 	= Helper.formatVector3Ex("  pos", target.getPosition())
			self.lblLook.text 		= Helper.formatVector3Ex(" look", target.getLook())
			self.lblUp.text 		= Helper.formatVector3Ex("   up", target.getUp())
			self.lblRight.text 		= Helper.formatVector3Ex("right", target.getRight())
		
		CallBack.callBack(0.5, self.updateData)
		

