#-*- coding: utf-8 -*-

import Lazy

import Monster

from data import d_monster
from GData import GData

##################################################
#boss类
##################################################
class Boss(Monster.Monster) :
	
	def canShow(self) :
		return super(Boss, self).canShow() and d_monster.canShow(self.eno, GData.player)
		
	def updateData(self) :
		self.position = self.getPosition()
		self.physics.setActiveRange(self.position, 500)
		self.setLevel(d_monster.getLevel(self.eno))
		
		super(Boss, self).updateData()
		
	def getPosition(self) :
		pos = d_monster.getPosition(self.eno)[1]
		return Lazy.Vector3(*pos)
