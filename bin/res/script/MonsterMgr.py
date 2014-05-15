#-*- coding: utf-8 -*-

#怪点相关

import random
import Lazy

import CallBack

from data import d_monster

from GData import GData

##################################################
#怪点
##################################################
class MonsterCenter(object) :
	def __init__(self, no) :
		self.no = no
		self.position = d_monster.getCenterPos(no)
		self.amount = d_monster.getCenterAmount(no)
		self.monster = d_monster.getCenterMonster(no)
		self.level = d_monster.getCenterLevel(self.no)
		self.radius = 1500
		self.monsterPool = []
		
	def getRandomPos(self) :
		x = random.randint(-self.radius, self.radius) + self.position[0]
		z = random.randint(-self.radius, self.radius) + self.position[2]
		return Lazy.Vector3(x, 0, z)
		
	def createOne(self) :
		id = GData.entityMgr.createEntity("Monster", self.monster)
		print("----createOne", id)
		if not id :
			return
		ent = GData.entityMgr.entity(id)
		ent.setLevel(self.level)
		ent.initActiveRange(self.getRandomPos(), self.radius)
		self.monsterPool.append(id)
		
	def deleteOne(self, id) :
		try :
			self.monsterPool.remove(id)
			print("----deleteOne", id)
		except :
			pass
		
	def update(self) :
		if len(self.monsterPool) < self.amount :
			self.createOne()
			
	def updateLevel(self, lv) :
		if self.level < lv :
			self.level = lv

##################################################
#
##################################################
class MonsterMgr(object) :
	def __init__(self) :
		self.monsterCenters = []
		self.index = 0
		
	def loadMonsterCenter(self, mno) :
		print("----loadMonsterCenter", mno)
		self.index += 1
		self.monsterCenters = []
		lst = d_monster.getMapCenter(mno)
		for no in lst :
			self.monsterCenters.append(MonsterCenter(no))
		CallBack.callBack(5, self.update, self.index)
			
	#当有ent被删除时调用
	def onRemove(self, ent) :
		if ent.isMonster() :
			for d in self.monsterCenters :
				d.deleteOne(ent.id)
			
	def update(self, index) :
		#print "----MonsterMgr update", index
		if index != self.index :
			return
		for d in self.monsterCenters :
			d.update()
		CallBack.callBack(1, self.update, index)
		
	def updateLevel(self, lv) :
		for item in self.monsterCenters :
			item.updateLevel(lv)