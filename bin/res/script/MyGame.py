# -*- coding: utf-8 -*-

import traceback

from share import share

import Lazy

RT_MESH = 0
RT_SKINMESH = 1

def init() :
	print("python game init.")
	
	try :
		test()
	except:
		print(traceback.print_exc())
		return False

	return True


def fini() :
	try :
		Lazy.setPlayer(None)
		share.dcenter.clearEntities()
	except:
		print(traceback.print_exc())

	print ("python game fini.")

def test():
	print(dir(Lazy))

	#testArguments()

	#tesetVector()

	#testEntity()

	startScript()

def testArguments():

	print("Test Lazy.TestClass method arguments:")
	a = Lazy.TestClass()
	a.test0()
	a.test1("hello")
	a.test2("hello", "python", a, Lazy)
	a.test2()

	#Test invalid arguments
	#a.test0("wrong")
	#a.test1()
	#a.test1("hello", "wrong")

	print("Test Lazy function arguments:")
	Lazy.test0()
	Lazy.test1("hello")
	Lazy.test2("hello", "python", a, Lazy)
	Lazy.test2()

	#Test invalid arguments
	#Lazy.test0("wrong")
	#Lazy.test1()
	#Lazy.test1("hello", "wrong")

def tesetVector():

	a = Lazy.Vector3(1,2)
	print("Vector3", a, a.x, a.y, a.z)

	b = Lazy.Vector3(1, 1, 1)
	c = a + b
	print("c=a+b", c, a, b)

	print("-a", -a)
	print("a-b", a-b)
	print("a*2", a*2.0)
	#print("a/2", a/2.0)

def testEntity():
	import Entity
	import Player

	#a = Lazy.Entity()
	#print("a", a)

	id = Lazy.createEntity(Entity.Entity, {"position": (4, 0, 0), "eno": 1001, "name": "Demo"})
	#Lazy.destroyEntity(entity)

	id = Lazy.createEntity(Player.Player, {"position": (2, 0, 0), "eno": 1, "name": "游蓝海"})
	entity = Lazy.entity(id)
	Lazy.setPlayer(entity)

	print("entities", Lazy.entities())
	print("map:", Lazy.map())

def startScript():
	import DataCenter

	share.dcenter = DataCenter.DataCenter()

	skyBox = Lazy.getSkyBox()
	skyBox.image = "gui/sky/01/%d.jpg"
	skyBox.source = Lazy.player()
	skyBox.setRange((-5000, -3000, -5000), (5000, 7000, 5000))

	share.dcenter.loadMap(1)
