#-*- coding: utf-8 -*-

from . import GUI

from GData import GData
from data import d_task

##################################################
#任务追踪
##################################################
class TaskTrace(GUI.Form) :
	def __init__(self, id, parent, x, y) :
		super(TaskTrace, self).__init__(id, parent, "", x, y, 240, 140)
		self.enableClickTop(False)
		
		self.lblTitle = GUI.Label(0, self, 80, 0, "任务追踪")
		
		self.lblCurInfo = GUI.Label(10, self, 0, 20, "当前任务：",  color=0xff00ff00)
		self.lblCurTitle = GUI.Label(10, self, 100, 20, "", color=0xff007fff)
		self.lblCurContent = GUI.Label(0, self, 10, 40, "", "宋体/12")
		self.lblCurContent.setMutiLine(True)
		self.lblCurContent.setSize(200, 50)
		
		self.lblAcceptInfo = GUI.Label(10, self, 0, 80, "可接任务：", color=0xff00ff00)
		self.lblAcceptTitle = GUI.Label(10, self, 100, 80, "无", color=0xff007fff)
		self.lblAcceptContent = GUI.Label(0, self, 10, 100, "", "宋体/12")
		self.lblAcceptContent.setMutiLine(True)
		self.lblAcceptContent.setSize(200, 50)
		
	def updateData(self):
		
		curTasks = GData.player.taskMgr.getAcceptTasks()
		tno = 0
		if len(curTasks) > 0:
			tno = curTasks[0]
		self.updateCurrent(tno)
		
		canAcceptTasks = GData.player.taskMgr.getCanAcceptTask()
		tno = 0
		if len(canAcceptTasks) > 0:
			tno = canAcceptTasks[0]
		self.updateAccept(tno)
	
	def updateCurrent(self, tno):
		if not tno:
			self.lblCurTitle.text = "无"
			self.lblCurContent.text = ""
			return
		
		text = d_task.getTaskName(tno)
		if GData.player.canTaskCompete(tno):
			text += "(可完成)"
		self.lblCurTitle.text = text
		self.lblCurContent.text = d_task.getTaskDesc(tno)
		
	
	def updateAccept(self, tno):
		if not tno:
			self.lblAcceptTitle.text = ""
			self.lblAcceptContent.text = "无"
			return
		
		self.lblAcceptTitle.text = d_task.getTaskName(tno)
		self.lblAcceptContent.text = d_task.getTaskDesc(tno)
