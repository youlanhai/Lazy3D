#-*- coding: utf-8 -*-

import traceback
import copy
import Lazy
from . import GUI

from GData import GData
from data import d_map

def ARGB2DWORD(argb) :
	DCOLOR = 0
	for cr in argb :
		DCOLOR = DCOLOR << 8
		DCOLOR |= int(0xff * cr)
	return DCOLOR

def DWORD2ARGB(dword) :
	colors = []
	for i in range(4) :
		cr = (dword & 0xff) / 255.0
		colors.insert(0, cr)
		dword = dword >> 8
	return colors

class ColorPanel(GUI.Panel) :
	def __init__(self, id, parent, text, x, y) :
		super(ColorPanel, self).__init__(id, parent, x, y)
		self.destColor = [0, 0, 0, 0]
		
		self.lblTittle = GUI.Label(0, self, 0, 0, text)
		self.colorDWORD = GUI.Label(0, self, 120, 0, "")
		
		self.lblA = GUI.Label(0, self, 0, 20, "a")
		self.scrollA = GUI.Scroll(1, self, "", 20, 20, 150, 18)
		self.colorA = GUI.Label(0, self, 180, 20, "0")
		
		self.lblR = GUI.Label(0, self, 0, 40, "r")
		self.scrollR = GUI.Scroll(2, self, "", 20, 40, 150, 18)
		self.colorR = GUI.Label(0, self, 180, 40, "0")
		
		self.lblG = GUI.Label(0, self, 0, 60, "g")
		self.scrollG = GUI.Scroll(3, self, "", 20, 60, 150, 18)
		self.colorG = GUI.Label(0, self, 180, 60, "0")
		
		self.lblB = GUI.Label(0, self, 0, 80, "b")
		self.scrollB = GUI.Scroll(4, self, "", 20, 80, 150, 18)
		self.colorB = GUI.Label(0, self, 180, 80, "0")
		
		self.lblAll = GUI.Label(0, self, 0, 100, "all")
		self.scrollAll = GUI.Scroll(5, self, "", 20, 100, 150, 18)
		self.colorAll = GUI.Label(0, self, 180, 100, "0")
		
		self.labels = (self.colorA, self.colorR, self.colorG, self.colorB, )
		self.scrolls = (self.scrollA, self.scrollR, self.scrollG, self.scrollB, )
		
	def setRate(self, argbs) :
		self.destColor = copy.copy(list(argbs))
		self.colorDWORD.text = "0x%8.8x" % (ARGB2DWORD(self.destColor), )
		for i, cr in enumerate(argbs) :
			self.labels[i].text = "%.4f" % cr
			self.scrolls[i].setRate(cr)
		
	def onScroll(self, id, pos) :
		if id == 5 :
			self.setRate((pos,)*4)
			self.colorAll.text = "%.4f" %  pos
			
		else:
			self.destColor[id-1] = pos
			self.labels[id-1].text = "%.4f" %  pos
			
		self.colorDWORD.text = "0x%8.8x" % (ARGB2DWORD(self.destColor), )
		self.parent.onColorChange(self.id, self.destColor)


##################################################
#游戏配置
##################################################
class Config(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(Config, self).__init__(id, parent, image, x, y, w, h)
		self.show(False)
		self.backColor = 0xbf000000
		
		self.btnClose = GUI.Button(1, self, "X", "", w-30, 10, 20, 20)
		self.cursorLabel = GUI.Label(0, self, 10, 40, "鼠标速度")
		self.cursorScroll = GUI.Scroll(2, self, "", 110, 40, 150, 20)
		self.cursorScroll.setRate(0.5)
		self.cursorLabelShow = GUI.Label(0, self, 270, 40, "0.5")
		
		self.panelAmbientColor = ColorPanel(3, self, "环境光-颜色", 10, 60)
		
		self.panelLightColor = ColorPanel(4, self, "方向光-颜色", 10, 180)
		
		self.panelForgColor = ColorPanel(5, self, "雾化-方向", 10, 300)
		
		self.initData()
		
	def show(self, show) :
		super(Config, self).show(show)
		
		if show :
			print(traceback.extract_stack())
		
	def initData(self) :
		no = GData.mapMgr.mapNO
		ambientColor = d_map.getAmbintColor(no)
		lightDir = d_map.getLightDir(no)
		lightColor = d_map.getLightColor(no)
		fogColor = d_map.getFogColor(no)
		
		self.panelAmbientColor.setRate(DWORD2ARGB(ambientColor))
		self.panelLightColor.setRate(lightColor)
		self.panelForgColor.setRate(DWORD2ARGB(fogColor))
		
		self.lightDir = Lazy.VECTOR3(lightDir[0], lightDir[1], lightDir[2])
		
	def onScroll(self, id, pos) :
		print("onScroll", id, pos)
		if id == 2 :
			GData.camera.rotSpeed = pos
			self.cursorLabelShow.text = "%0.1f" % pos
		
	def onButton(self, id, isDown) :
		if not isDown :
			if id == 1 :
				self.show(False)
				
	def onColorChange(self, id, color) :
		if id == 3 :
			self.onAmbientChange(color)
			
		elif id == 4 :
			self.onLightColorChange(color)
			
		elif id == 5 :
			self.onFogColorChange(color)
		
		
	def onAmbientChange(self, color) :
		DCOLOR = ARGB2DWORD(color)
		Lazy.setAmbientLight(DCOLOR)
	
	def onLightColorChange(self, color) :
		Lazy.setDirectionLight(self.lightDir, *color)
	
	def onFogColorChange(self, color) :
		DCOLOR = ARGB2DWORD(color)
		GData.fog.setColor(DCOLOR)
		