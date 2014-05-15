#-*- coding: utf-8 -*-

from . import GUI
#from GData import GData

class EntityInfo(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(EntityInfo, self).__init__(id, parent, image, x, y, w, h)
		self.backColor = 0xffffffff
		self.name = GUI.Label(0, self, 115, 30, "测试名字")
		self.level = GUI.Label(0, self, 80, 43, "1", "宋体/12/0", 0xff00ff00)
		self.hp = GUI.Label(0, self, 115, 43, "100/100", "宋体/12/0", 0xffff0000)
		self.mp = GUI.Label(0, self, 115, 55, "100/100", "宋体/12/0", 0xff0000ff)
		self.entityID = 0
		self.show(False)
		
	def updateInfo(self, entity) :
		if not entity :
			self.show(False)
			self.entityID = 0
			return
		else :
			self.show(True)
			self.entityID = entity.id
			self.name.text = entity.name
			self.level.text = str(entity.level)
			self.hp.text = "%d/%d" % (entity.hp, entity.maxHp)
			self.mp.text = "%d/%d" % (entity.mp, entity.maxMp)
		

class PlayerInfo(EntityInfo) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(PlayerInfo, self).__init__(id, parent, image, x, y, w, h)
	
class TargetInfo(EntityInfo) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(TargetInfo, self).__init__(id, parent, image, x, y, w, h)
		self.show(False)