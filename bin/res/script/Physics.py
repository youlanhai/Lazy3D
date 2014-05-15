#-*- coding: utf-8 -*-

import random

import Lazy

import CallBack
import Define

from data import d_action

from GData import GData

##################################################
#
##################################################
class Physics(Lazy.Physics) :
	def __init__(self):
		super(Physics, self).__init__()

	def enterWorld(self):
		pass
		
	def onAITrigger(self) :
		self.source.setAttackable(True)
	
	def onStateChange(self, oldSate):
		print("----onStateChange", oldSate, self.state)
		if not self.source :
			return
		if self.state == Define.PS_FREE :
			self.source.playAction(d_action.FREE)
		elif self.state == Define.PS_MOVE:
			self.source.playAction(d_action.WALK)
		elif self.state == Define.PS_ATTACK :
			self.source.playAction(d_action.ATTACK)
		elif self.state == Define.PS_DEAD :
			self.source.playAction(d_action.DEATH)


	def onMoveToFinished(self, succed) :
		print("----onMoveToFinished", succed)
	
	
##################################################
#
##################################################
class PlayerPhysics(Physics):
	def __init__(self) :
		super(PlayerPhysics, self).__init__()
		self.target = None
		self.attackIndex = 0
		self.attackTargetID = 0
		self.attackSpeed = 1
		
	def onStateChange(self, oldSate):
		super(PlayerPhysics, self).onStateChange(oldSate)
		
	def onMoveToFinished(self, succed) :
		print("----onMoveToFinished", succed, self.state)
		entity = GData.entityMgr.entity(GData.cursorEntID)
		if entity: entity.show(False)
	
	def onMoveToEntityFinish(self, succed) :
		self.source.onMoveToEntityFinish()
		
	#####################################################
	#
	#####################################################
	def getAttackTarget(self) :
		return GData.entityMgr.entity(self.attackTargetID)
		
	def attack(self, targetID):
		print("attack", targetID, self.attackTargetID)
		if targetID == self.attackTargetID:
			return
		self.attackTargetID = targetID
		self.attackIndex += 1
		self.onAttack(self.attackIndex)
		
	def stopAttack(self) :
		self.attackIndex += 1
		self.attackTargetID = 0
		self.state = Define.PS_FREE
			
	def onAttack(self, index) :
		print("onAttack", index, self.attackIndex, self.attackTargetID)
		if index != self.attackIndex :
			return
		target = self.getAttackTarget()
		if not target.isAlive() or not self.source.canAttack(target):
			self.stopAttack()
			return
		self.faceTo(target.position)
		self.state = Define.PS_ATTACK
		self.source.attackSingle(self.attackTargetID)
		Lazy.playSound("res/sound/attack.wav")
		CallBack.callBack(self.attackSpeed, self.onAttack, index)
		

##################################################
#
##################################################
class NpcPhysics(Physics) :
	def __init__(self) :
		super(NpcPhysics, self).__init__()
		
	def enterWorld(self):
		if hasattr(self.source, "moveable") :
			self.aiEnable = self.source.moveable
		else :
			self.aiEnable = True
			
		self.aiInterval = 1.0
		self.freeState = 0
		self.activeRadius = 1000
		self.activeCenter = Lazy.Vector3(self.source.position)
		
	def setActiveRange(self, position, radius) :
		self.activeCenter = position
		self.activeRadius = radius
		
	def onAITrigger(self) :
		if not self.source.visible(): return
		
		if not self.source.isAlive() :
			self.state = Define.PS_DEAD
			self.breakAutoMove()
			return
		
		super(NpcPhysics, self).onAITrigger()
		
		dist = self.source.distToPlayer() 
		if self.source.isEnemy() :
			self.enemyAI(dist)
		elif self.source.isNormal() :
			self.normalAI(dist)
		elif self.source.isFriend() :
			self.friendAI(dist)
		
	def friendAI(self, dist) :
		self.free()
		
	def normalAI(self, dist) :
		self.free()
			
	def enemyAI(self, dist) :
		if not GData.player.isAlive():
			self.free()
			return
		
		if dist <= Lazy.getMaxDistToNpc() :
			self.attack()
		elif dist < 1000 :
			self.follow()
		else:
			self.free()
			
	def free(self) :
		#print("---free")
		if not self.source.canMove() :
			return
		if self.freeState != 0 :
			return
		self.breakAutoMove()
		self.state = Define.PS_FREE
		if random.random() > 0.5 :
			self.guard()
		else :
			self.idle()
		
	def follow(self) :
		#print("---follow")
		self.breakFree()
		self.moveToEntity(GData.player)
		
	def attack(self) :
		#print("---attack")
		GData.player.lockTarget(self.source)
		self.breakFree()
		self.breakAutoMove()
		self.state = Define.PS_ATTACK
		self.source.attack(GData.player.id)
		
	def stopAttack(self) :
		pass
		
	def dead(self) :
		print("---dead")
		self.breakFree()
		pass

	def guard(self) :
		#print("---guard")
		self.freeState = 1
		self.moveTo(self.getRandomPos())
		
	def idle(self) :
		#print("---idle")
		self.breakAutoMove()
		self.freeState = 2
		self.state = Define.PS_FREE
		CallBack.callBack(random.uniform(4, 10), self.onIdleFinished)
		
	def breakFree(self) :
		self.freeState = 0
		
	def getRandomPos(self) :
		pos = Lazy.Vector3(self.getRandomf(), self.getRandomf(), self.getRandomf())
		return pos + self.activeCenter
		
	def getRandomf(self) :
		return random.uniform(-self.activeRadius, self.activeRadius)
		
	def onMoveToFinished(self, succed) :
		if self.freeState == 1:
			self.freeState = 0
			self.free()
			
	def onIdleFinished(self) :
		if self.freeState == 2:
			self.freeState = 0
			self.free()
