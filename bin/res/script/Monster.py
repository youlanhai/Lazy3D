#-*- coding: utf-8 -*-

import Lazy
import Npc

from data import d_monster

class Monster(Npc.Npc) :
	@staticmethod
	def isMonster() : return True
	
	def __init__(self) :
		super(Monster, self).__init__()
		
	def getFriendState(self) :
		return d_monster.getFriendState(self.eno)
		
	def onTargetActive(self) :
		print("onTargetActive", self)
		
		if not self.isAlive(): return
		if not GData.player.canAttack(self): return
		
		GData.player.attack()
		self.beAttack()
		
	def beAttack(self) :
		self.friendState = 2
		
	def updateData(self) :
		self.friendState = self.getFriendState()
		self.setName(d_monster.getName(self.eno))
		self.loadModel(d_monster.getModel(self.eno))
		
		self.updateShow()
		
	def onDead(self) :
		Lazy.playSound("res/sound/dead.wav")
		objects = d_monster.getTaskObj(self.eno)
		print("objects", objects)
		for no in objects :
			if GData.player.isTaskObject(no) :
				GData.player.addTaskObject(no, 1)
		super(Monster, self).onDead()
		
		#数据作弊
		GData.player.addExpEx(self.level)
	
	def attack(self, targetID) :
		entity = GData.entityMgr.entity(targetID)
		if not entity:
			return
		self.lookAtPosition(entity.position)
		self.attackSingle(targetID)
