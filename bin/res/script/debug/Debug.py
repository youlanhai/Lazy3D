#-*- coding: utf-8 -*-

#import Lazy
import Define

from gui import GUI

from . import MapEdit
from . import PlayerInfo
from . import CameraInfo
from . import SystemInfo

class Debug(object) :
	def __init__(self) :
		#windowW = Lazy.getApp().width
		#windowH = Lazy.getApp().height
		self.mapEdit = MapEdit.MapEdit(1, GUI.gui, "", 100, 100, 500, 400)
		self.playerInfo = PlayerInfo.PlayerInfo(2, GUI.gui, "", 0, 30, 300, 200)
		self.cameraInfo = CameraInfo.CameraInfo(3, GUI.gui, "", 500, 100)
		self.systemInfo = SystemInfo.SystemInfo(4, GUI.gui, "", 500, 100, 300, 200)
		
	def onKey(self, key, isDown) :
		if isDown :
			if key == Define.VK_E:
				self.mapEdit.toggle()
			elif key == Define.VK_P :
				self.playerInfo.toggle()
			elif key == Define.VK_O :
				self.cameraInfo.toggle()
			elif key == Define.VK_K:
				self.systemInfo.toggle()
