#-*- coding: utf-8 -*-

import Entity
import Physics
from share import share

class Npc(Entity.Entity) :
	@staticmethod
	def isNpc() : return True

	def __init__(self) :
		super(Npc, self).__init__()
		self.__friendState = 0
		self.onFriendStateChange()
			
	def setFState(self, s) :
		if self.__friendState == s :
			return
		self.__friendState = s
		self.onFriendStateChange()
	def getFState(self) :
		return self.__friendState
	friendState = property(getFState, setFState)
	
	def onFriendStateChange(self) :
		#print "onFriendStateChange", self.friendState
		if self.topboard :
			cr = 0xff00bf00
			if self.isFriend() :
				cr = 0xffffff00
			elif self.isNormal() :
				cr = 0xff0000ff
			elif self.isEnemy() :
				cr = 0xffff0000
			self.topboard.color = cr
		
	#创建物理
	def createPhysics(self) :
		self.physics = Physics.NpcPhysics()
	
	#朋友
	def isFriend(self) : return self.friendState <= 0
		
	#普通关系。被动可攻击。
	def isNormal(self) : return self.friendState == 1
		
	#敌对。主动攻击
	def isEnemy(self) : return self.friendState == 2
		
	#是否可移动
	def canMove(self) : 
		if self == GData.targetLocked :
			return False
		else : return True
		
	def onTargetActive(self) :
		self.physics.breakFree()
		self.physics.breakAutoMove()
		self.lookAtPosition(GData.player.position)
			
	def onTargetSelect(self) :
		pass
		
	#受击
	def onHit(self, otherID, demage) :
		print(("onHit", otherID, demage, self.hp))
		if self.id == GData.guiMgr.targetInfo.entityID :
			GData.guiMgr.targetInfo.updateInfo(self)
	
	#初始化活动范围
	def initActiveRange(self, position, radius) :
		self.position = position
		if self.physics :
			self.physics.setActiveRange(position, radius)
	
	#死亡回调
	def onDead(self) :
		super(Npc, self).onDead()
		CallBack.callBack(1.2, GData.entityMgr.remove, self)
		