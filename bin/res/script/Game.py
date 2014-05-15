#-*- coding: utf-8 -*-

import Lazy

from tools import packagepy

import EntityMgr
import MonsterMgr
import Player

import Define
import MapMgr
import Save

from gui import GUI
from gui import GUIMgr
from GData import GData

g_player = None


#初始化脚本
def initScript() :
	print("---initScript")
	
	GData.gameState = Define.GS_INIT

	initGame()
	login()

#初始化游戏资源
def initGame() :
	
	#entity池子
	GData.entityMgr = EntityMgr.EntityMgr()
	
	GData.monsterMgr = MonsterMgr.MonsterMgr()
	
	#音乐播放器
	GData.musicPlayer = Lazy.cMusicPlayer()
	
	#天空盒子
	GData.skyBox = Lazy.getApp().getSkyBox()
	GData.skyBox.setSkyRange(
		Lazy.VECTOR3(-50000,-20000,-50000), 
		Lazy.VECTOR3(50000,30000,50000))
		
	#雾化
	GData.fog = Lazy.cFog()
	GData.fog.setFogType(Define.FT_LINEAR|Define.FT_PIXEL)
	GData.fog.setStartEnd(1000, 5000)
	GData.fog.setDensity(0.5)
	
	#雪花
	GData.snow = Lazy.cSnow(1024, "res/model/texture/snowflake.dds", None)
	GData.snow.setRange(Lazy.VECTOR3(-500, 100, -500), Lazy.VECTOR3(500, 500, 500))
	GData.snow.show(False)
	
	#地图
	GData.mapMgr = MapMgr.MapMgr()
	GData.map = GData.mapMgr.map #engine map
	
	GData.mapMgr.loadMap(1, True) #load login map
		
	#拾取
	GData.pick = Lazy.getPick()
		
	#gui管理器
	GData.gui = GUI.GUI()
	
	#相机
	GData.camera = Lazy.getCamera()
	#GData.camera.enableLockHeight(True)
	GData.camera.setDistRange(100, 800) #temp
	GData.camera.setNearFar(1, 100000) #temp
	GData.camera.distToPlayer = 500
	
	#gui系统
	GData.guiMgr = GUIMgr.GuiMgr()
	
	#玩家及相关
	player = Player.Player()
	GData.entityMgr.setPlayer(player)
	GData.camera.setSource(GData.player)
	GData.map.setSource(GData.player)
	GData.skyBox.setSource(GData.player)
	GData.snow.setSource(GData.player)
	
	GData.pick.setTerrainObjEnable(False)
	
	

#登陆游戏
def login() :
	GData.guiMgr.fadeDoor.gameStartShow() #渐变方式启动
	#GData.guiMgr.mainMenu.show(True)
	sound = Lazy.loadSound("res/sound/lighten.wav")
	if sound :
		sound.play()

#结束游戏
def endGame() :
	print("----endGame")
	#GData.guiMgr.mainMenu.show(True)
	Lazy.getApp().quitGame()
	
#消息循环
def messageProc(msg, wParam, lParam) :
	if GData.gui.messageProc(msg, wParam, lParam) :
		return 1
		
	if GData.gameState != Define.GS_RUNNING:
		return 0
		
	if msg == Define.WM_KEYDOWN :
		onKeyMessage(wParam, True)
	elif msg == Define.WM_KEYUP :
		onKeyMessage(wParam, False)
		
	if GData.camera.handleMessage(msg, wParam, lParam) :
		return 1
		
	elif GData.pick.handleMouseEvent(msg, wParam, lParam) :
		return 1
		
	GData.map.handeMouseEvent(msg, wParam, lParam)
	GData.guiMgr.debug.mapEdit.handleMessage(msg, wParam, lParam)
	
		
	if GData.entityMgr.handleMouseEvent(msg, wParam, lParam) :
		return 1
		
	cursorEntityMsg(msg, wParam, lParam)
	return 0
	
def onKeyMessage(key, isDown) :
	#print "onKeyMessage", key, isDown
	GData.guiMgr.onKey(key, isDown)
	if GData.player.handleKeyEvent(key, isDown) :
		return
	if not isDown :
		if key == Define.VK_ESCAPE:
			GData.guiMgr.mainMenu.showMenu(Define.MENU_WHY_PLAY)


#鼠标模型消息
def cursorEntityMsg(msg, wParam, lParam):
	if msg == Define.WM_LBUTTONUP:
		if GData.camera.isDraged() :
			return 1
		if GData.pick.isIntersectWithTerrain() :
			GData.player.physics.breakAutoMove()
			pos = GData.pick.getPosOnTerrain()
			entity = GData.entityMgr.entity(GData.cursorEntID)
			if entity :
				entity.position = pos
				entity.show(True)
			GData.player.physics.searchPath(pos)
		
#场景变化
def onChangeSence() :
	pass

#脚本释放
def releaseScript() :
	Save.save()
	
	GData.musicPlayer.stop()
	GData.musicPlayer = None
	GData.entityMgr.clear()
	GData.entityMgr.setPlayer(None)
	GData.entityMgr = None
	GData.guiMgr = None
	
	paths = packagepy.getModulesPath()
	#packagepy.extractFiles("package/python27", paths)
	packagepy.savePaths("res/entities/tools/packagepath.py", paths)
	
	print("---releaseScript")

###数据交互
def getFog() :
	return GData.fog
