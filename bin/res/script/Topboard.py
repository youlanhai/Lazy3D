#-*- coding: utf-8 -*-

import Lazy

class Topboard(Lazy.cTopBoard) :
	def __init__(self, source) :
		super(Topboard, self).__init__(source)
		self.engineManaged = False