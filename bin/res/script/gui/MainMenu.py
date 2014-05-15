#-*- coding: utf-8 -*-

import Lazy
from . import GUI

import CallBack
import Define
import Save
from GData import GData

FADE_SPEED = 10
FADE_DELAY = 1 #延迟时间

class MainMenu(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w=300, h=200) :
		super(MainMenu, self).__init__(id, parent, image, x, y, w, h)
		self.backColor = 0xaf000000
		self.enableDrag(False)
		self.title = GUI.Label(0, self, 0, 10, "登陆")
		self.title.setSize(300, 40)
		self.title.setAlign("center")
		self.btnStart = GUI.Button(1, self, "开始", "", 100, 60, 100, 30)
		self.btnContinue = GUI.Button(2, self, "继续", "", 100, 100, 100, 30)
		self.btnExit = GUI.Button(3, self, "退出", "", 100, 140, 100, 30)
		self.why = 0
		self.show(False)

	def onButton(self, id, isDown) :
		print("onButton", id, isDown)
		if not isDown :
			if id == 1 :
				self.onStart()
			elif id == 2 :
				self.onContinue()
			elif id == 3:
				self.onExit()
		
	def onStart(self) :
		self.show(False)
		if self.why == Define.MENU_WHY_INIT:
			GData.gameState = Define.GS_RUNNING
			GData.guiMgr.fadeDoor.changeSence()
			GData.player.onGameStart()
			
		elif self.why == Define.MENU_WHY_PLAY:
			Save.load(True)
	
	def onContinue(self) :
		self.show(False)
		
		if self.why == Define.MENU_WHY_INIT:
			Save.load(False)
			
		elif self.why == Define.MENU_WHY_PLAY:
			GData.gameState = Define.GS_RUNNING
			GData.player.onGameStart()
	
	def onExit(self) :
		self.show(False)
		GData.guiMgr.fadeDoor.quitGame()
		
	def showMenu(self, why):
		self.why = why
		self.show(True)


class FadeDoor(GUI.Panel):
	def __init__(self, id, parent) :
		super(FadeDoor, self).__init__(id, parent, 0, 0)
		windowW = Lazy.getApp().width
		windowH = Lazy.getApp().height
		self.imgLeft = GUI.Image(0, self, "res/gui/door.jpg", 0, 0, windowW/2, windowH)
		self.imgLeft.setTexUV(1, 0.5, 0)
		self.imgLeft.setTexUV(2, 0.5, 1.0)
		self.imgRight = GUI.Image(0, self, "res/gui/door.jpg", windowW/2, 0, windowW/2, windowH)
		self.imgRight.setTexUV(0, 0.5, 0)
		self.imgRight.setTexUV(3, 0.5, 1.0)
		self.completeIndex = 0
		self.openCompleteCB = self.onOpenComplete
		self.closeCompleteCB = self.onCloseComplete
		self.show(False)
		
	#游戏启动展示
	def gameStartShow(self):
		self.show(True)
		CallBack.callBack(FADE_DELAY, self.openDoor, self.gameStartShowEnd)
		
	def gameStartShowEnd(self) :
		self.onOpenComplete()
		GData.gameState = Define.GS_INIT
		CallBack.callBack(FADE_DELAY, GData.guiMgr.mainMenu.showMenu, Define.MENU_WHY_INIT)
		
	#场景过渡
	def changeSence(self):
		self.closeDoor(self.onSencechangedHalf)
		
	def onSencechangedHalf(self):
		self.openCompleteCB = self.onSenceChangedOver
		CallBack.callBack(FADE_DELAY, self.open)
		
	#场景过渡完毕
	def onSenceChangedOver(self):
		self.onOpenComplete()
		self.openCompleteCB = self.onOpenComplete
		self.closeCompleteCB = self.onCloseComplete
		
	#退出游戏
	def quitGame(self):
		self.closeDoor(self.onCloseComplete)
		
	#开门
	def openDoor(self, callback=None) :
		print("openDoor", callback)
		self.openCompleteCB = callback
		self.open()
		
	#关门
	def closeDoor(self, callback = None) :
		print("closeDoor", callback)
		self.closeCompleteCB = callback
		self.close()
		
	#开始特效
	def open(self) :
		if self.completeIndex != 0 :
			return
		GData.gameState = Define.GS_TANSITION
		self.show(True)
		self.completeIndex = 1
		self.imgLeft.setPosition(0, 0)
		self.imgRight.setPosition(Lazy.getApp().width/2, 0)
		self.openMoveToLeft()
		self.openMoveToRight()
		
	def openMoveToLeft(self) :
		pos = self.imgLeft.getPosition()
		if pos.x <= -Lazy.getApp().width/2 :
			self.openComplete()
		else:
			pos.x -= FADE_SPEED
			self.imgLeft.setPosition(pos.x, pos.y)
			CallBack.callBack(0.01, self.openMoveToLeft)
	
	def openMoveToRight(self) :
		pos = self.imgRight.getPosition()
		if pos.x >= Lazy.getApp().width:
			self.openComplete()
		else:
			pos.x += FADE_SPEED
			self.imgRight.setPosition(pos.x, pos.y)
			CallBack.callBack(0.01, self.openMoveToRight)
		
	
	def openComplete(self) :
		self.completeIndex += 1
		if self.completeIndex >= 3 :
			self.completeIndex = 0
			print("----completed")
			if self.openCompleteCB :
				self.openCompleteCB()
			
	
	#结束特效
	def close(self):
		if self.completeIndex != 0 :
			return
		GData.gameState = Define.GS_TANSITION
		self.show(True)
		self.completeIndex = 1
		self.imgLeft.setPosition(-Lazy.getApp().width/2, 0)
		self.imgRight.setPosition(Lazy.getApp().width, 0)
		self.closeLeftToCenter()
		self.closeRightToCenter()
		
		
	def closeLeftToCenter(self) :
		pos = self.imgLeft.getPosition()
		if pos.x >= 0:
			self.closeComplete()
		else:
			pos.x += FADE_SPEED
			if pos.x > 0:
				pos.x = 0
			self.imgLeft.setPosition(pos.x, pos.y)
			CallBack.callBack(0.01, self.closeLeftToCenter)
	
	def closeRightToCenter(self) :
		pos = self.imgRight.getPosition()
		x = Lazy.getApp().width//2
		if pos.x <= x:
			self.closeComplete()
		else:
			pos.x -= FADE_SPEED
			if pos.x < x:
				pos.x = x
			self.imgRight.setPosition(pos.x, pos.y)
			CallBack.callBack(0.01, self.closeRightToCenter)
	
	def closeComplete(self) :
		self.completeIndex += 1
		if self.completeIndex >= 3 :
			self.completeIndex = 0
			print("----completedex")
			if self.closeCompleteCB :
				self.closeCompleteCB()
		
	#开始特效完毕
	def onOpenComplete(self) :
		self.show(False)
		GData.gameState = Define.GS_RUNNING
		 
	#结束特效完毕
	def onCloseComplete(self):
		GData.gameState = Define.GS_EXIT
		CallBack.callBack(FADE_DELAY, Lazy.getApp().quitGame)