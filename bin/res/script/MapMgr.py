#-*- coding: utf-8 -*-

import traceback
import Lazy
from data import d_map

from GData import GData


##################################################
#
##################################################
class MapMgr(object) :
	def __init__(self) :
		self.mapNO = 0
		self.map = Lazy.getMap()
		self.map.setShowLevel(3)
		
	#加载地图
	def loadMap(self, no, urgent=False) :
		print("loadMap", no, self.mapNO)
		if self.mapNO == no :
			return GData.player.onMapChange(self.mapNO)
		path = d_map.getMapPath(no)
		if not path :
			return
		self.mapNO = no
		
		if urgent:
			self.doLoadMap()
			self.doLoadFinish()
		else:
			Lazy.getApp().startGameLoading(2) #必须放到这里，以通知主线程停止走常规更新逻辑
			Lazy.addLoadingTask(self.doLoadMapInThread, self.doLoadFinishInThread)
		
	def doLoadMap(self):
		print("---doLoadMap")
		
		print("---clear entities")
		GData.entityMgr.clearEntities()
		
		GData.musicPlayer.stop()
		
		print("---real load")
		path = d_map.getMapPath(self.mapNO)
		self.map.loadMap(path)
		
		print("---load scene music")
		music = d_map.getMapMusic(self.mapNO)
		if not GData.musicPlayer.loadMusic(music) :
			print("----faild to load music try to load default music", music)
			GData.musicPlayer.loadMusic(d_map.DEFAULT_MUSIC)
		if music :
			GData.musicPlayer.setRepeat(True)
		GData.musicPlayer.play()
		
		print("---create entities")
		GData.entityMgr.createEntities(self.mapNO)
		
		print("---set sky")
		GData.skyBox.setSkyImage(d_map.getMapSky(self.mapNO));
		
	def doLoadFinish(self) :
		print("---doLoadFinish")
		self.onMapChanged(self.mapNO)
		
		if GData.player:
			GData.player.onMapChange(self.mapNO)
		
	def doLoadMapInThread(self):
		
		try:
			self.doLoadMap()
		except:
			print(traceback.print_exc())
		
	def doLoadFinishInThread(self):
		try:
			self.doLoadFinish()
		except:
			print(traceback.print_exc())
			
		Lazy.getApp().stopGameLoading()
		
	def onMapChanged(self, no) :
		print("onMapChanged", no)
		self.setSenceLighting(no)
		
	def setSenceLighting(self, no) :
		Lazy.setAmbientLight(d_map.getAmbintColor(no))
		
		lightDir = d_map.getLightDir(no)
		color = d_map.getLightColor(no)
		
		Lazy.setDirectionLight(Lazy.VECTOR3(*lightDir), *color)
		
		fog = d_map.getFogColor(no)
		GData.fog.setColor(fog)
			
		if hasattr(GData, "gui") :
			GData.guiMgr.config.initData()
		
	
