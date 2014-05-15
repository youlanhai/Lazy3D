#coding = utf-8

import random

import Lazy

import Define
import iData
import Physics

from data import d_model
from data import d_npc
from data import d_action

FADE_DISTANCE_DELTA = 1.0

##################################################
#实体基类
##################################################
class Entity(Lazy.Entity, iData.iData) :

	@staticmethod
	def isEntity() : return True
	
	@staticmethod
	def isNpc() : return False
	
	@staticmethod
	def isAvatar() : return False
		
	@staticmethod
	def isTransport() : return False
		
	@staticmethod
	def isMonster() : return False
	
	def __init__(self) :
		iData.iData.__init__(self)
		
		self.topboard = None
	
	def enterWorld(self) :
		print("enterWorld", self.eno, self)

		self.fadeDistance = 30.0 #显示距离
		self.lockHeight = 0.2

		self.createPhysics()
		self.createTopBoard()

		self.setName(self.name)
		self.loadModel(self.sculpt)
		self.updateData()

		self.physics.enterWorld()
		
	def leaveWorld(self) :
		print("leaveWorld", self.eno, self)

		self.show(False)
		self.model = None
		self.physics = None
		self.topboard = None
		
	#设置任务名称
	def setName(self, name) :
		self.name = name
		self.refreshTopBoard()
	
	#创建物理
	def createPhysics(self) :
		self.physics = Physics.Physics()
	
	#显示实体
	def show(self, show) :
		super(Entity, self).show(show)
#		self.refreshTopBoard()

	#获得随机位置
	def genRandomPos(self) :
		return Lazy.Vector3(random.uniform(-500, 500), 0, random.uniform(-500, 500))
	
	#根据eno更新数据。
	def updateData(self) :
		self.updateShow()
	
	def setPosition(self, x, y, z) :
		self.position = Lazy.Vector3(x, y, z)

	#########################################################
	#玩家与NPC交互
	#########################################################
	
	#【引擎回调】是否可鼠标选择。返回false，将不可接受鼠标消息
	def canSelect(self) :
		return self.isAlive()
	
	def canShow(self):
		return True

		dist = self.distToPlayer()
		
		if dist > self.fadeDistance + FADE_DISTANCE_DELTA: return False
		if dist <= self.fadeDistance: return True
		
		return True
	
	def updateShow(self):
		show = self.canShow()
		self.show(show)
		
	#【引擎回调】收到鼠标消息。
	def onFocusCursor(self, msg) :
		if msg == Define.CM_LUP :
			self.targetSelect()
		elif msg == Define.CM_RUP :
			self.targetActive()

	#选择目标
	def targetSelect(self) :
		self.onTargetSelect()
	
	#激活目标
	def targetActive(self) :
		if self.isInRange() :
			self.onTargetActive()
		else :
			self.onTargetTooFar()
	
	#目标被选择
	def onTargetSelect(self) :
		print("i was selected!")
		
	#是否在玩家活动范围
	def isInRange(self) :
		return self.distToPlayer() <= Lazy.getMaxDistToNpc()
	
	#目标过远
	def onTargetTooFar(self) :
		pass
	
	#目标被激活
	def onTargetActive(self) :
		print("i was hited!", self)
		
	#########################################################
	#头顶板
	#########################################################
	#创建头顶板
	def createTopBoard(self) :
		self.topboard = Lazy.Topboard("Entity", "def|24", 0xff0000ff, 2.0)
		self.topboard.source = self
		
	#重置头顶板
	def resetTopboard(self) :
		if not self.topboard: return

		if self.model:
			y = self.model.getHeight()
			scale = self.model.scale
			self.topboard.biasHeight = y * scale.y
	
	#刷新头顶板内容
	def refreshTopBoard(self) :
		if not self.topboard: return

		self.topboard.text = self.name
#		self.topboard.show(self.visible())
	
	#加载模型
	def loadModel(self, id) :
		if not d_model.exist(id): return

		self.modelID = id
		self.model = Lazy.loadModel(d_model.getPath(id), d_model.getType(id))
		if not self.model:
			print("model not found", id, self)
			return

		scale = d_model.getScale(id)
		self.model.scale = (scale, scale, scale)
		self.model.yaw = d_model.getYaw(id)
		self.model.pitch = d_model.getPitch(id)
		self.model.roll = d_model.getRoll(id)
		self.onModelLoaded()
	
	#模型被加载
	def onModelLoaded(self):
		self.resetTopboard()
		if d_action.exist(self.modelID) :
			self.model.playAction(d_action.free(self.modelID), True)
	
	#播放动作
	def playAction(self, actionID):
		if self.model and d_action.exist(self.modelID) :
			self.model.playAction(d_action.action(self.modelID, actionID), d_action.needLoop(actionID))
	
	#切换显示
	def toggle(self) :
		self.show(not self.visible())
	
	#########################################################
	#升级
	#########################################################
	def onLevelUp(self, old) :
		print("onLevelUp", old, self.level)
	
	#死亡回调
	def onDead(self) :
		if self.physics :
			self.physics.onAITrigger()
		