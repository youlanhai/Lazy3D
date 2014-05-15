#-*- coding: utf-8 -*-

import Lazy

import Npc
import iTask

from data import d_task

from GData import GData

class TaskNpc(iTask.iTask, Npc.Npc) :
	def __init__(self) :
		super(TaskNpc, self).__init__()
		
	def updateData(self) :
		self.position = self.getPosition()
		self.setName(d_task.getNpcName(self.eno))
		self.loadModel(d_task.getNpcModel(self.eno))
		self.moveable = d_task.getNpcMove(self.eno)
		if not self.moveable and self.physics:
			self.physics.enableAI(False)
			
		self.updateShow()
		
	def getPosition(self) :
		pos = d_task.getNpcPosition(self.eno)[1]
		return Lazy.Vector3(*pos)
		
	def onTargetActive(self) :
		if self.distToPlayer() > Lazy.getMaxDistToNpc() :
			GData.player.chatClientMsg("距离过远")
			return
		super(TaskNpc, self).onTargetActive()
		GData.guiMgr.talkForm.onUpdate()
		