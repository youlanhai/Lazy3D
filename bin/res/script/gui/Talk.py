#-*- coding: utf-8 -*-

#import Lazy
from . import GUI
import Define

#from data import d_npc_task
from data import d_task

from GData import GData

#选择选项
class ChoiceItem(object) :
	def __init__(self, astr) :
		lst = astr.split("_", 1)
		self.next = int(lst[0]) #下一对白的编号。
		self.content = "" #选项内容。如果有内容，说明有选项，否则next直接关联下一句对白。
		if len(lst) > 1:
			self.content = lst[1]
	
	#是否有内容
	def hasContent(self) :
		return bool(self.content)
		
	#选项是否是确定
	def isOK(self) :
		return self.__next__ == 1001
		
	#选项是否是其他
	def isCancel(self) :
		return self.__next__ == 1002
	
##################################################
#对话内容
##################################################
class TalkContent(object) :
	def __init__(self, data) :
		self.data = data #对话内容
		self.index = -1 #当前索引
		self.selectedIndex = -1 #玩家选择的索引。
		self.choices = [] #选项
		self.owner = 0 #当前说话者
		self.content = "" #当前说话内容
		self.parse(0) #解析第一条语句。
		
	#开始解析。next为要解析的语句编号。
	def parse(self, next) :
		if self.index == next:
			return False
		if next not in self.data :
			return False
		self.index = next
		d = self.data[self.index]
		self.owner = d[0]
		self.content = d[1]
		self.choices = []
		for i in range(2, len(d)) : #从第2项开始，是后续选择项。
			self.choices.append(ChoiceItem(d[i]))
		return True
	
	#解析下一条语句
	def parseNext(self):
		if self.index < 0 :
			return False
		if len(self.choices) == 0 : #没有选项。即，对话就此完毕。
			return False
		next = self.choices[0].__next__
		if len(self.choices) >= 2 : #有两个选项，则下条语句为玩家选择的。
			next = self.selectedIndex
		return self.parse(next)
		
	#玩家选择
	def selected(self, itemIndex) :
		self.selectedIndex = itemIndex
			
	def getOwner(self): return self.owner
	def getIndex(self) : return self.index
	def getContent(self) : return self.content
	def getChoices(self) : return self.choices

##################################################
#对话窗体
##################################################
class Talk(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(Talk, self).__init__(id, parent, image, x, y, w, h)
		self.enableDrag(False)
		self.enableClickTop(False)
		self.lblContent = GUI.Label(0, self, 150, 10, "这是测试对话框", "宋体/20")
		self.lblContent.setMutiLine(True)
		self.lblContent.setSize(w-160, 100)
		self.btnOk = GUI.Button(1, self, "", "", 150, h-40, 200, 30)
		self.btnCancel = GUI.Button(2, self, "", "", 360, h-40, 200, 30)
		self.entityID = 0
		self.talkContent = None #对话内容
		self.taskNo = 0 #任务编号
		self.submitTaskID = 0
		self.clear()
		self.show(False)
	
	def npc(self) :
		if self.entityID :
			return GData.entityMgr.entity(self.entityID)
		return None
		
	def onMessageProc(self, msg, wParam, lParam):
		if msg == Define.WM_LBUTTONUP :
			self.onLButtonUp()
			
	def onShow(self, show) :
		if not show and self.submitTaskID:
			GData.player.competeTask(self.submitTaskID)
			self.submitTaskID = 0
	
	#按钮消息处理
	def onButton(self, id, isDown) :
		if not isDown :
			if id == 1:
				self.onOK()
			elif id == 2 :
				self.onCancel()
	
	#显示所有按钮
	def showButtons(self, show) :
		self.btnOk.show(show)
		self.btnCancel.show(show)
	
	#确定按钮消息响应
	def onOK(self) :
		self.showButtons(False)
		item = self.talkContent.choices[0]
		if item.isOK() : #是接受任务选项
			self.onAcceptTask()
		else :
			self.talkContent.selected(item.__next__)
			self._parseNext()
	
	#取消按钮消息响应
	def onCancel(self) :
		self.showButtons(False)
		item = self.talkContent.choices[1]
		if item.isCancel() : #是拒绝任务选项
			self.clear()
		else :
			self.talkContent.selected(item.__next__)
			self._parseNext()
				
	#接受任务
	def onAcceptTask(self) :
		if self.taskNo :
			GData.player.acceptTask(self.taskNo)
		self.clear()
		
	#清除所有状态
	def clear(self):
		self.lblContent.text = ""
		self.btnOk.show(False)
		self.btnCancel.show(False)
		self.talkContent = None
		self.show(False)
	
	#鼠标左键弹起消息响应
	def onLButtonUp(self) :
		if self.btnOk.isVisible() or self.btnCancel.isVisible() :
			return
		if not self._parseNext() :
			self.clear()
		
	def _parseNext(self) :
		if self.talkContent :
			if self.talkContent.parseNext() :
				self.updateContent()
				return True
		return False
	
	#更新内容。仅依据当前数据对界面进行更新，不做任何更改。
	def updateContent(self) :
		entity = self.npc()
		if not entity :
			return
		owner = self.talkContent.getOwner()
		text = "测试呀"
		if owner == 0:
			text = GData.player.name
		else :
			text = entity.name
		self.lblContent.text = "%s : %s" % (text, self.talkContent.getContent())
		choices = self.talkContent.getChoices()
		if len(choices) == 0:
			self.btnOk.show(False)
			self.btnCancel.show(False)
			return
		if len(choices) >= 1:
			item = choices[0]
			if item.hasContent() :
				self.btnOk.show(True)
				self.btnOk.setText(item.content)
		if len(choices) >= 2:
			self.btnCancel.show(True)
			self.btnCancel.setText(choices[1].content)
	
	#设置对话。
	def setTalk(self, talk) :
		print("setTalk", talk)
		self.talkContent = TalkContent(talk)
		self.updateContent()
		
		
	#【对外接口】更新数据。
	def onUpdate(self) :
		print("Talk onUpdate")
		if not GData.targetLocked :
			return
		if self.talkContent != None :
			return
		self.show(True)
		entity = GData.targetLocked 
		self.entityID = entity.id
		self.taskNo = 0
		
		#可完成任务
		submits = entity.getSubmitTasks()
		if len(submits) > 0 :
			self.submitTaskID = submits[0]
			d = d_task.getSubmitTalk(submits[0])
			return self.setTalk(d)
			
		#可接任务
		tasks = entity.getTasks()
		if len(tasks) > 0:
			self.taskNo = tasks[0]
			d = d_task.getTaskTalk(self.taskNo)
			return self.setTalk(d)
		
		#无任务
		d = d_task.getNpcTalk(entity.eno)
		self.setTalk(d)
		

##################################################