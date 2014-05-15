#-*- coding: utf-8 -*-

import Npc

class CursorEntity(Npc.Npc) :
	def __init__(self) :
		super(CursorEntity, self).__init__()
		self.show(False)
		self.showDistance = 0
		
	#是否可移动
	def canMove(self) : return False
	
	def onTargetActive(self) :
		pass
		
	def canSelect(self) :
		return False
	
	def updateShow(self):
		pass
		
	def createTopBoard(self) :
		pass
