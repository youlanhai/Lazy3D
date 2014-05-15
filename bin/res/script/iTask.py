#-*- coding: utf-8 -*-

from data import d_task
from GData import GData

class iTask(object) :
	
	def canShow(self) :
		return super(iTask, self).canShow() and d_task.canNpcShow(self.eno, GData.player)
	
	#获得可接任务
	def getTasks(self) :
		tasks = d_task.getNpcTasks(self.eno)
		if not tasks or len(tasks) <=0 :
			return []
		lst = []
		for no in tasks :
			if GData.player.canTaskAccept(no):
				lst.append(no)
		return lst
		
	#获得可完成任务
	def getSubmitTasks(self) :
		tasks = d_task.getNpcSubmitTasks(self.eno)
		if not tasks or len(tasks) <=0 :
			return []
		lst = []
		for no in tasks :
			if GData.player.canTaskCompete(no):
				lst.append(no)
		return lst
	