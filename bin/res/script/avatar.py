# -*- coding: utf-8 -*-

import Lazy

##################################################
###
##################################################

class AvatarPhysics(Lazy.Physics):

	def onActive(self, active):
		print("AvatarPhysics.onActive", active)


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
		self.model.scale = 0.05
		self.physics = AvatarPhysics()
		self.speed = (2.0, 2.0, 2.0)

	def leaveWorld(self):
		print("Avatar.leaveWorld")

