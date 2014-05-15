#-*- coding: utf-8 -*-

#import Lazy
from . import GUI

import CallBack
from GData import GData

from data import d_map

W_1024 = 200
W_512  = 100
W_256  = 50

MAP_AREA = {
	1001: ("昆都仑区",     100, 300, W_1024, W_1024, 0xffff0000),
	1002: ("青山区",       300, 300, W_512,  W_512,  0xff00ff00),
	1003: ("九原区",       400, 350, W_512,  W_512,  0xffff0000),
	1004: ("东河",         500, 400, W_512,  W_512,  0xff00ff00),
	1005: ("土默特右旗",   600, 350, W_512,  W_512,  0xffff0000),
	1006: ("石拐区",       500, 300, W_512,  W_512,  0xff0000ff),
	1007: ("达尔罕茂旗",   500, 200, W_512,  W_512,  0xffff0000),
	1008: ("固阳县",       400, 250, W_512,  W_512,  0xff00ff00),
	1009: ("白云鄂博矿区", 200, 100, W_1024, W_1024, 0xff0000ff),
}

##################################################
#地图
##################################################
class Map(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(Map, self).__init__(id, parent, image, x, y, w, h)
		self.show(False)
		self.enableDrag(False)
		self.enableClickTop(False)
		self.backColor = 0xdf000000
		self.btnClose = GUI.Button(1, self, "X", "", w-30, 10, 20, 20)
		self.maps = []
		for k, v in MAP_AREA.items() :
			self.maps.append(MapNode(k, self, v[0], v[5], "", v[1], v[2], v[3], v[4]))

	def onButton(self, id, isDown) :
		if not isDown :
			if id == 1 :
				self.show(False)


##################################################
#子地图
##################################################
def convertPos(pos, width, height) :
	w = pos[0] - GData.map.xMin()
	h = pos[2] - GData.map.zMin()
	dx = w/GData.map.width()
	dz = h/GData.map.height()
	return (int(dx*width), int(dz*height))


class MapNode(GUI.Form) :
	def __init__(self, id, parent, text, color, image, x, y, w, h) :
		super(MapNode, self).__init__(id, parent, image, x, y, w-2, h-2)
		self.enableDrag(False)
		self.backColor = 0xafdfdfdf
		self.label = GUI.Label(0, self, 0, (h-30)/2, text)
		self.label.color = color
		self.label.setSize(w, 30)
		self.label.setAlign("center")
		self.label.setFont("宋体/20/0/b")
		self.trans = d_map.getMapTransport(id) #控件ID就是mapID
		self.buttons = []
		for no in self.trans :
			pos = d_map.getTransPosition(no)
			pos = convertPos(pos, w, h)
			self.buttons.append(TransButton(no, self, pos[0], pos[1], 30, 30))
			
	def onButton(self, id, isDown) :
		if not isDown :
			GData.guiMgr.map.show(False)
			CallBack.callBack(0.1, GData.player.onTransport, self.id, id)



##################################################
#传送按钮
##################################################
g_btnColors = (0xdf800000, 0xdfd00000, 0xdf600000, 0xdf400000, 0xdfa0000);

class TransButton(GUI.Button) :
	def __init__(self, id, parent, x, y, w, h) :
		super(TransButton, self).__init__(id, parent, "", "res/gui/ui/a%d.png", x, y, w-2, h-2)
		self.enablePosMove(False)
		self.enableColor(False)
		for i, cr in enumerate(g_btnColors) :
			self.setStateColor(i, cr)
		self.backColor = g_btnColors[0]


##################################################
