#-*- coding: utf-8 -*-

class _GData(object) :
	def __init__(self) :
		self.gameState = 0
		self.__targetLocked = None
		self.entiyPool = None
		self._player = None
		self.guiMgr = None
		self.entityClassMap = {}
		
	def setTargetLocked(self, target) :
		self.__targetLocked = target
	
	@property
	def targetLocked(self) : return self.__targetLocked
		
	@property
	def player(self) : return self._player
		
	def setPlayer(self, player) : self._player = player
		
	def regEntityClass(self, key, type_) :
		self.entityClassMap[key] = type_
		
	def getEntityClass(self, key) :
		return self.entityClassMap.get(key, None)

GData = _GData()
