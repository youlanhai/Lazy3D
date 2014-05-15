#-*- coding: utf-8 -*-

from data import d_task

class Task(object) :
	def __init__(self, no) :
		self.no = no
		self.npc = d_task.getTaskNpc(no)
		self.submitNpc = d_task.getSubmitNpc(no)
		self.talk = d_task.getTaskTalk(no)
		self.submitTalk = d_task.getSubmitTalk(no)
		self.condition = d_task.getCondition(no)
		self.work = d_task.getTaskWork(no)

###########################################################
#任务管理器
###########################################################
class TaskMgr(object) :
	def __init__(self) :
		self.objectStock = {} #物品库存。编号-已获得个数
		self.taskTarget = {} #任务目标物品。编号-需要个数
		self.listAccept = [] #已接任务表
		self.listCompeted = [] #已完成任务表
		
	def save(self) :
		return tuple(self.listCompeted)
	
	def load(self, data) :
		self.objectStock.clear()
		self.taskTarget.clear()
		self.listAccept = []
		
		if not data :
			self.listCompeted = []
		else:
			self.listCompeted = list(data)
		
	def isAccepted(self, no) :
		return no in self.listAccept
	
	def isCompeted(self, no):
		return no in self.listCompeted
	
	def getAcceptTasks(self) :
		return tuple(self.listAccept)
	
	def getCompeteTasks(self) :
		return tuple(self.listCompeted)
	
	def canAccept(self, no):
		if not d_task.isExist(no):
			return False
		if self.isAccepted(no):
			return False
		if self.isCompeted(no) :
			return False
		
		condTasks = d_task.getCondition(no)
		for no in condTasks:
			if not self.isCompeted(no):
				return False
			
		return True
	
	def canCompete(self, no):
		if not self.isAccepted(no):
			return False
		if self.isCompeted(no):
			return False
		
		works = d_task.getTaskWork(no)
		for k, v in works.items() :
			if self.getStockAmount(k) < v :
				return False
			
		return True
	
	def acceptTask(self, no) :
		if not self.canAccept(no) :
			return False
		
		self.listAccept.append(no)
		self.addTaskTarget(no)
		return True
		
	def abandonTask(self, no) :
		if not self.isAccepted(no):
			return False
		
		self.listAccept.remove(no)
		self.delTaskTarget(no)
		return True
	
	def competeTask(self, no) :
		if not self.canCompete(no):
			return False
		
		self.listAccept.remove(no)
		self.listCompeted.append(no)
		self.recycleTaskObjectStock(no)
		self.delTaskTarget(no)
		return True
	
	def deleteTask(self, no):
		if self.isAccepted(no):
			self.abandonTask(no)
		elif self.isCompeted(no):
			self.listCompeted.remove(no)
	
	#############################################
	#任务目标
	#############################################
	
	def addTaskTarget(self, no) :
		works = d_task.getTaskWork(no)
		for k, v in works.items() :
			self._addTaskTarget(k, v)
	
	def delTaskTarget(self, no):
		works = d_task.getTaskWork(no)
		for k, v in works.items() :
			self._delTaskTarget(k, v)
			
	
	def _addTaskTarget(self, objNo, amount) :
		if objNo not in self.taskTarget :
			self.taskTarget[objNo] = 0
		self.taskTarget[objNo] += amount
		
	def _delTaskTarget(self, objNo, amount) :
		if objNo not in self.taskTarget :
			return False
		if self.taskTarget[objNo] < amount :
			return False
		self.taskTarget[objNo] -= amount
		if self.taskTarget[objNo] == 0 :
			self.taskTarget.pop(objNo)
		return True
	
	#获得需要的任务物品数
	def getTargetAmount(self, objNo) :
		return self.taskTarget.get(objNo, 0)
	
	#############################################
	#库存物品
	#############################################
	
	def recycleTaskObjectStock(self, no):
		works = d_task.getTaskWork(no)
		for k, v in works.items() :
			self.delObjectStock(k, v)
		
	def addObjectStock(self, objNo, amount) :
		if not self.isTaskObject(objNo):
			return
		
		if objNo not in self.objectStock :
			self.objectStock[objNo] = 0
		self.objectStock[objNo] += amount
		
	def delObjectStock(self, objNo, amount) :
		if objNo not in self.objectStock :
			return False
		if self.objectStock[objNo] < amount :
			return False
		self.objectStock[objNo] -= amount
		if self.objectStock[objNo] == 0 :
			self.objectStock.pop(objNo)
		return True
	
	#获得任务物品个数
	def getStockAmount(self, objNo) :
		return self.objectStock.get(objNo, 0)
		
		
	#是否是任务物品
	def isTaskObject(self, objNo) :
		return self.getTargetAmount(objNo) > self.getStockAmount(objNo)
	
	def getCanAcceptTask(self):
		tasks = []
		for no in d_task.taskKeys():
			if self.canAccept(no) :
				tasks.append(no)
				
		return tasks
