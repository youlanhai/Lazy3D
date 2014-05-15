#-*- coding: utf-8 -*-

import Lazy
import Define
import Helper

from gui import GUI

from GData import GData

class ListNode(GUI.ListNode) :
	def __init__(self, data) :
		self.data = data
		self.text = "%d-" % self.data.id
		self.text += Lazy.getTerrainRes().getPath(self.data.id)
		
	def getText(self) :
		return self.text
		
	def getModelID(self) :
		if self.data :
			return self.data.id
		else :
			return 0

##################################################
#带标签的滚动条
##################################################
class ScrollWithLabel(object) :
	def __init__(self, id, parent, text, x, y, w, h, rMin=-10, rMax=10) :
		self.id = id
		self.parent = parent
		self.name = GUI.Label(0, parent, x, y, text)
		self.scroll = GUI.Scroll(id, parent, "", x+50, y, w, h)
		self.scroll.setVertical(False)
		self.label = GUI.Label(0, parent, x+w+60, y, "")
		self.setValueRange(rMin, rMax)
		
	def setValueRange(self, min_, max_) :
		self.rangeMin = min_
		self.rangeMax = max_
		self.range = float(max_ - min_)
			
	def onScroll(self, pos) :
		num = self.getValue()
		self.label.text = str(num)
		return num
		
	def setScroll(self, num) :
		pos = (num-self.rangeMin)/self.range
		self.scroll.setRate(pos)
		self.label.text = str(num)
		
	def getValue(self) :
		return self.scroll.getRate() * self.range + self.rangeMin

##################################################
#地图编辑器
##################################################
class MapEdit(GUI.Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(MapEdit, self).__init__(id, parent, image, x, y, w, h)
		self.selectedObj = None
		self.pos = None
		self.selectedPos = None
		
		self.backColor = 0xa0000000
		
		self.btnClose = GUI.Button(1, self, "X", "", w-30, 10, 20, 20)
		self.lstView = GUI.ListView(3, self, "", 10, 50, 200, 300)
		self.lstView.createItems(20)
		self.lablPos = GUI.Label(0, self, 10, h-20, "")
		
		self.lblSelectObj = GUI.Label(0, self, 230, 30, "未选中")
		
		self.scale = ScrollWithLabel(10, self, "缩放", 230, 60, 100, 20, 0.1, 10.0)
		self.angelX = ScrollWithLabel(11, self, "look", 230, 90, 100, 20, -3.14, 3.14)
		self.angelY = ScrollWithLabel(12, self, "up", 230, 120, 100, 20, -3.14, 3.14)
		self.angelZ = ScrollWithLabel(13, self, "right", 230, 150, 100, 20, -3.14, 3.14)
		self.x = ScrollWithLabel(14, self, "x", 230, 180, 100, 20, -100, 100)
		self.y = ScrollWithLabel(15, self, "y", 230, 210, 100, 20, -100, 100)
		self.z = ScrollWithLabel(16, self, "z", 230, 240, 100, 20, -100, 100)
		
		self.btnSave = GUI.Button(5, self, "保存", "", 150, h-40, 60, 24)
		self.btnReload = GUI.Button(6, self, "重新加载", "", 300, h-40, 60, 24)
		self.btnNew = GUI.Button(7, self, "新建", "", 220, 270, 60, 24)
		self.btnMove = GUI.Button(8, self, "移动到", "", 220, 300, 60, 24)
		self.btnRemove = GUI.Button(9, self, "删除", "", 220, 330, 60, 24)
		
		self.initData()
		self.show(False)
		
	def initData(self) :
		tRes = Lazy.getTerrainRes()
		resPool = tRes.getResPool()
		data = []
		for item in resPool :
			data.append(ListNode(item))
		self.lstView.setData(data)
		
	def onScroll(self, id, pos) :
		print("onScroll", id, pos)
		
		if not self.selectedObj :
			return
		elif id == 10 :
			self.selectedObj.setScale(self.scale.onScroll(pos))
		elif id == 11 :
			self.selectedObj.addLookAngle(self.angelX.onScroll(pos))
		elif id == 12 :
			self.selectedObj.addUpAngle(self.angelY.onScroll(pos))
		elif id == 13 :
			self.selectedObj.addRightAngle(self.angelZ.onScroll(pos))
		elif id == 14 :
			print("x", self.selectedObj.position.x)
			self.selectedObj.position.x = self.pos.x+self.x.onScroll(pos)
		elif id == 15 :
			print("y", self.selectedObj.position.y)
			self.selectedObj.position.y = self.pos.y+self.y.onScroll(pos)
		elif id == 16 :
			print("z", self.selectedObj.position.z)
			self.selectedObj.position.z = self.pos.z+self.z.onScroll(pos)
		
	def onButton(self, id, isDown) :
		if not isDown :
			if id == 1 :
				self.show(False)
			elif id == 5:
				self.onSave()
			elif id == 6 :
				self.onReload()
			elif id == 7 :
				self.onNew()
			elif id == 8:
				self.onMoveTo()
			elif id == 9:
				self.onRemove()
				
	def onItemSelect(self, id, itemNo) :
		print("onItemSelect", id, itemNo)
		
	def updateSelectedObj(self, obj) :
		print("updateSelectedObj", obj)
		if self.selectedObj == obj :
			return
		self.selectedObj = obj
		if self.selectedObj :
			self.lblSelectObj.text = "已选中[%d]" % obj.getID()
			self.scale.setScroll(obj.scale.x)
			angle = obj.getAngle()
			self.angelX.setScroll(angle.x)
			self.angelY.setScroll(angle.y)
			self.angelZ.setScroll(angle.z)
			self.pos = Lazy.Vector3(obj.position)
			self.x.setScroll(0)
			self.y.setScroll(0)
			self.z.setScroll(0)
		else :
			self.lblSelectObj.text = "未选中"
			self.scale.setScroll(0)
			
			
	def onSave(self) :
		if GData.map.saveObj() :
			GData.player.chatClientMsg("保存成功")
		else :
			GData.player.chatClientMsg("保存失败")
		
	def onReload(self) :
		if GData.map.loadObj("") :
			GData.player.chatClientMsg("加载成功")
		else :
			GData.player.chatClientMsg("加载失败")
		self.updateSelectedObj(None)
		
	def onNew(self) :
		item = self.lstView.getSelectedItem()
		if not item :
			GData.player.chatClientMsg("请选择模型")
			return
		if not self.selectedPos :
			GData.player.chatClientMsg("请选择位置")
			return
		obj = GData.map.createObj()
		obj.setModelID(item.getModelID())
		obj.position = self.selectedPos
		scale = self.scale.getValue()
		if scale > 0.1 :
			obj.setScale(scale)
		GData.map.addObj(obj)
		self.updateSelectedObj(obj)
		GData.player.chatClientMsg("新建成功")
		
	def onMoveTo(self) :
		if not self.selectedObj :
			GData.player.chatClientMsg("请选择物体")
			return
		if not self.selectedPos:
			GData.player.chatClientMsg("请选择位置")
		self.selectedObj.position = self.selectedPos
		GData.player.chatClientMsg("移动成功")
			
	def onRemove(self) :
		if not self.selectedObj :
			GData.player.chatClientMsg("请选择物体")
			return
		GData.map.removeObj(self.selectedObj)
		self.updateSelectedObj(None)
		GData.player.chatClientMsg("移除成功")
			
	
	def handleMessage(self, msg, wParam, lParam) :
		if not self.isVisible() :
			return
		if msg == Define.WM_RBUTTONUP:
			self.updateSelectedObj(GData.map.getSelectObj())
		elif msg == Define.WM_LBUTTONUP:
			if GData.pick.isIntersectWithTerrain() :
				self.selectedPos = GData.pick.getPosOnTerrain()
				self.lablPos.text = Helper.formatVector3Ex("click", self.selectedPos)