# -*- coding: utf-8 -*-

import Lazy

PS_FREE     = 0  #休闲
PS_MOVE     = 1  #移动
PS_ROTATE   = 2  #旋转
PS_ATTACK   = 3  #攻击
PS_SKILL    = 4  #释放技能
PS_BE_HIT   = 5  #受击
PS_DEAD     = 6  #死亡
PS_RUN      = 7  #奔跑
PS_RIDE     = 8  #骑马
PS_JUMP     = 9  #跳跃
PS_SWIM     = 10 #游泳

##################################################
###
##################################################

class AvatarPhysics(Lazy.Physics):

	def onActive(self, active):
		print("AvatarPhysics.onActive", active)

	def onStateChange(self, old):
		print("AvatarPhysics.onStateChange", self.state, old)
		model = self.source.model
		if model is None:
			return

		if self.state == PS_FREE:
			model.playAction("stand", True)

		elif self.state == PS_MOVE:
			model.playAction("walk1", True)

##################################################
###
##################################################
class Avatar(Lazy.Entity):

	def __init__(self):
		super(Avatar, self).__init__()
		#print("Avatar.__init__", self.id)

	def isPlayer(self): return True

	def enterWorld(self):
		print("Avatar.enterWorld")

		self.model = Lazy.loadModel("model/jingtian/jingtian.x", 1)
		self.model.scale = 0.02
		self.model.yaw = 3.14
		self.physics = AvatarPhysics()
		self.speed = (2.0, 2.0, 2.0)

	def leaveWorld(self):
		print("Avatar.leaveWorld")

