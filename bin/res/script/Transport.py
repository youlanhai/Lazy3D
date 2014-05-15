#-*- coding: utf-8 -*-

import Npc

class Transport(Npc.Npc) :
	
	@staticmethod
	def isTransport() : return True
	
	def canMove(self) : return False

	def onTargetActive(self) :
		return
