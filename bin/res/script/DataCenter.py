#-*- coding: utf-8 -*-

import traceback
import Lazy

from data import d_entity
from data import d_map

ENTITY_CLASS_MAP = {}

def loadEntityClass() :
	for kno in d_entity.all_kno():
		name = d_entity.kno2class(kno)
		mod = __import__(name)
		cls = getattr(mod, name)
		ENTITY_CLASS_MAP[name] = cls

def getEntityClass(name):
	return ENTITY_CLASS_MAP.get(name)

##################################################
#
##################################################
class DataCenter(object) :
	
	def __init__(self) :
		super(DataCenter, self).__init__()

		self.mapno = 0
		loadEntityClass()
		
	def createEntity(self, eno) :
		print("createEntity", eno)
		
		kno = d_entity.get_kno(eno)
		classname = d_entity.kno2class(kno)
		cls = getEntityClass(classname)

		if not cls: return 0

		data = d_entity.get_data(eno)
		data["eno"] = eno

		try:
			id = Lazy.createEntity(cls, data)
		except :
			print(traceback.print_exc())
			return 0

		return id
		
	def loadEntities(self, mapno) :
		self.clearEntities()

		print("createEntities start", mapno)

		self.mapno = mapno
		allenos = d_entity.map2all_eno(mapno)
		for eno in allenos:
			self.createEntity(eno)
		
		print("createEntities over")
		
	def clearEntities(self) :
		entities = Lazy.entities()
		for ent in entities.values():
			if not ent.isAvatar():
				Lazy.destroyEntity(ent)

		return
	
	def findEntityByEno(self, eno) :
		for ent in Lazy.entities().values():
			if ent.eno == eno : return ent
		
		return None

	def loadMap(self, mapno):
		path = d_map.get_path(mapno)
		Lazy.map().loadMap(path)
		self.loadEntities(mapno)
