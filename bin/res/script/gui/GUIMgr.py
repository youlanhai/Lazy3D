#-*- coding: utf-8 -*-
import Lazy
from . import GUI
import Define

from . import Chat
from . import Config

from . import EntityInfo

from . import MainMenu
from . import Map

from . import Talk
from . import TaskTrace
from . import Transport

from . import WordMsg

from debug import Debug

#import CallBack
from GData import GData


class GuiMgr(object) :
	def __init__(self) :
		windowW = Lazy.getApp().width
		windowH = Lazy.getApp().height
		self.debug = Debug.Debug()
		self.fadeDoor = MainMenu.FadeDoor(1, GUI.gui.topPanel)
		self.mainMenu = MainMenu.MainMenu(2, GUI.gui, "", (windowW-300)//2, (windowH-200)//2, 300, 200)
		self.talkForm = Talk.Talk(3, GUI.gui, "", 0, windowH-100, windowW, 100)
		self.transForm = Transport.TransForm(4, GUI.gui, "", (windowW-200)/2, (windowH-200)/2, 200, 200)
		self.config = Config.Config(5, GUI.gui, "", 500, 100, 300, 500)
		self.map = Map.Map(7, GUI.gui.topPanel, "", 0, 0, windowW, windowH)
		self.chat = Chat.ChatClientMsgEx(6, GUI.gui.topPanel, 0, 160)
		self.playerInfo = EntityInfo.PlayerInfo(7, GUI.gui, "res/gui/head.png", 10, 20, 217, 122)
		self.targetInfo = EntityInfo.TargetInfo(8, GUI.gui, "res/gui/head.png", 360, 20, 217, 122)
		self.wordMsg = WordMsg.WordMsg(9, GUI.gui.bottomPanel, "", 0, windowH-220, 300, 200)
		self.taskTrace = TaskTrace.TaskTrace(10, GUI.gui.bottomPanel, 0, 200)
		
	def onKey(self, key, isDown) :
		if not isDown :
			if key == Define.VK_F1 :
				GData.player.toggle()
			elif key == Define.VK_F2 :
				GData.skyBox.toggle()
			elif key == Define.VK_F3 :
				GData.fog.toggle()
			elif key == Define.VK_F4 :
				self.entitySphereToggle()
			elif key == Define.VK_F5 :
				self.terrainObjMsgToggle()
			elif key == Define.VK_F6:
				self.snowToggle()
			elif key == Define.VK_F7:
				self.drawCollisionToggle()
			elif key == Define.VK_F8:
				self.toggleFillMode()
			elif key == Define.VK_M :
				self.map.toggle()
			elif key == Define.VK_L :
				self.config.toggle()
		self.debug.onKey(key, isDown)
		
	def entitySphereToggle(self) :
		Lazy.setEntitySphereEnable(not Lazy.isEntitySphereEnable())
		
	def terrainObjMsgToggle(self) :
		GData.pick.setTerrainObjEnable(not GData.pick.isTerrainObjEnable())
		
	def snowToggle(self) :
		GData.snow.show(not GData.snow.visible())
		
	def drawCollisionToggle(self) :
		Lazy.enableDrawCollision(not Lazy.isDrawCollisionEnable())
		
	def toggleFillMode(self):
		mode = Lazy.getFillMode()
		mode = (mode + 1) % Define.MAX_FILL_MODE
		
		print("set fill mode", mode)
		Lazy.setFillMode(mode)
