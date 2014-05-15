#-*- coding: utf-8 -*-

import random
#import math

import Lazy

##################################################
#数值基类 所有的数值计算，尽量放到该类
##################################################
class iData(object) :
	def __init__(self) :
		self.hp = 0
		self.mp = 0
		self.ap = 0 #攻击力
		self.dp = 0 #防御力
		self.magicAttack = 0 #攻击力
		self.magicDefence = 0 #防御力
		self.level = 0 #等级
		self.exp = 0
		self.attackSpeed = 1.0 #攻击速度，时间
		self.attackable = False
		self.skills = []
		self.initData = ()
		
		self.genInitData()
		
	def loadData(self, data):
		if not data:
			self.genInitData()
			
		else:
			for k, v in data.items():
				setattr(self, k, v)
	
	def saveData(self):
		return {
			"hp" : self.hp,
			"mp" : self.mp,
			"ap" : self.ap,
			"dp" : self.dp,
			"magicAttack" : self.magicAttack,
			"magicDefence" : self.magicDefence,
			"level" : self.level,
			"exp" : self.exp,
			"attackSpeed" : self.attackSpeed,
			"attackable" : self.attackable,
			"skills" : self.skills,
			"initData" : self.initData,
		}
	
	#初始化随机数据
	def genInitData(self) :
		self.level = 1
		self.exp = 0
		self.hp = self.maxHp = random.randint(100, 300)
		self.mp = self.maxMp = random.randint(50, 100)
		self.ap = random.randint(10, 20)
		self.dp = random.randint(5, 10)
		if self.isAvatar() :
			self.ap += 10
			self.dp += 20
		self.magicAttack = random.randint(20, 40)
		self.magicDefence = random.randint(10, 20)
		self.initData = (self.hp, self.mp, self.ap, self.dp, self.magicAttack, self.magicDefence)
		
	def isAlive(self) :
		return self.hp > 0
		
	#设置等级，强制性设置
	def setLevel(self, level) :
		self.exp = self.levelToExp(level)
		self._setLevel(level)
		
	#需要多少经验升级
	def needExpToLevel(self) :
		return self.levelToExp(self.level+1) - self.exp
		
	#设置等级
	def _setLevel(self, level) :
		if self.level == level :
			return
		oldLevel = self.level
		self.level = level
		self._levelUp(oldLevel)
		
	#增加经验
	def addExp(self, exp) :
		if exp <= 0 :
			return
		self.onExpChange(exp)
		self.exp += exp
		lv = self.expToLevel(self.exp)
		self._setLevel(lv)
		
	def getExp(self) : return self.exp
		
	def onExpChange(self, exp) :
		pass
			
	def _levelUp(self, old) :
		self.hp = self.maxHp = (self.level+1)*self.initData[0]
		self.mp = self.maxMp = (self.level+1)*self.initData[1]
		self.ap = (self.level+1)*self.initData[2]
		self.dp = (self.level+1)*self.initData[3]
		self.magicAttack = (self.level+1)*self.initData[4]
		self.magicDefence = (self.level+1)*self.initData[5]
		self.onLevelUp(old)
		
	def expToLevel(self, exp) :
		a = exp/100.0
		return int(a**0.25)
		
	def levelToExp(self, level) :
		return level**4*100
		
	#自动行为
	def ai(self) :
		pass
		
	#普攻一个人
	def attackSingle(self, otherID) :
		other = Lazy.entity(otherID)
		if not other or not other.isAlive():
			return
		demage = self.ap - other.dp + random.randint(0, self.ap/2)
		if demage <=0 :
			demage = 1
		other.hp -= demage
		if other.hp < 0:
			other.hp = 0
		self.onAttack(otherID, demage)
		other.onHit(self.id, demage)
		if not other.isAlive() :
			self.physics.stopAttack()
			other.onDead()
		
	#普通群攻
	def attackMany(self, othersID) :
		for id in othersID :
			self.attackSingle(id)
		
	#法攻一个人
	def magicSingle(self, skill, other) :
		pass
		
	#法术群攻
	def magicMany(self, skill, others) :
		pass
		
	#攻击
	def onAttack(self, otherID, demage) :
		print(("onAttack", otherID, demage))
		
	#受击
	def onHit(self, otherID, demage) :
		print(("onHit", otherID, demage, self.hp))
		
	def onMagicAttack(self, otherID, skill, demage) :
		pass
		
	def onMagicHit(self, otherID, skill, demage) :
		pass
		
	def onDead(self) :
		pass
		
	def canAttack(self) :
		return self.attackable
		
	def setAttackable(self, e) :
		self.attackable = e
		
	##################################################
	#作弊数值计算
	
	def getLevelRangeExp(self, min_, max_) :
		return self.levelToExp(max_) - self.levelToExp(min_)
		
	def addExpEx(self, targetLv) :
		if targetLv > self.level :
			targetLv = self.level
		exp = targetLv*self.getLevelRangeExp(self.level, self.level+1) // self.level
		self.addExp(exp)