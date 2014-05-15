#-*- coding: utf-8 -*-
import Lazy
import Define

import lui

##################################################
#标签类
##################################################
class Label(Lazy.cLabel) :
	def __init__(self, id, parent, x, y, text="", font=font.standard_font, color=0xffffffff):
		super(Label, self).__init__(id, parent, text, x, y)
		self.setFont(font)
		self.color = color
		
##################################################
#图像
##################################################
class Image(Lazy.cImage) :
	def __init__(self, id, parent, text, x, y, w, h):
		super(Image, self).__init__(id, parent, text, x, y, w, h)
		self.color  = 0xffffffff

##################################################
#面板
##################################################
class Panel(Lazy.cPanel) :
	def __init__(self, id, parent, x, y) :
		super(Panel, self).__init__()
		self.parent = parent
		self.create(id, parent, x, y)
	
	def onMessageProc(self, msg, wParam, lParam):
		pass
		
	def onButton(self, id_, isDown) :
		pass

	def onScroll(self, id_, pos) :
		print("onScroll", id_, pos)
##################################################
#窗体
##################################################
class Form(Lazy.cForm) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(Form, self).__init__()
		self.parent = parent
		self.create(id, parent, image, x, y, w, h)
		self.setSize(w, h)
		self.image = self.getImagePtr()
	
	def onMessageProc(self, msg, wParam, lParam):
		pass

	def onButton(self, id_, isDown) :
		print("onButton", id_, isDown)

	def onScroll(self, id_, pos) :
		print("onScroll", id_, pos)
		
	def show(self, show_) :
		if show_ == self.isVisible() :
			return
		super(Form, self).show(show_)
		self.onShow(show_)
		
	def toggle(self) :
		self.show(not self.isVisible())
		
	def onShow(self, show_ ) :
		pass

##################################################
#按钮
##################################################
class Button(Lazy.cButton) :
	def __init__(self, id, parent, text, image, x, y, w, h) :
		super(Button, self).__init__()
		self.create(id, parent, text, image, x, y, w, h)
		self.parent = parent
		self.label = self.getLabelPtr()
		
	def getText(self) :
		return self.label.text
		
	def setText(self, text) :
		self.label.text = text
		
	def setColor(self, cr) :
		self.label.color = cr
		
	def setFont(self, font) :
		self.label.font = font
		
	def onMessageProc(self, msg, wParam, lParam):
		pass


##################################################
#滚动条
##################################################
class Scroll(Lazy.cScroll) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(Scroll, self).__init__()
		self.create(id, parent, image, x, y, w, h)
		self.parent = parent
		self.silider = self.getSilider()
		self.setVertical(False)
	
	
	def onButton(self, id_, isDown) :
		pass
		
	def onMessageProc(self, msg, wParam, lParam):
		pass

##################################################
#listView
##################################################
#结点数据接口。必须具备getText方法
class ListNode(object) :
	def getText(self) : return "测试item"
	
#列表项。必须具备setData方法
class ListItem(Button) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(ListItem, self).__init__(id, parent, "测试item", image, x, y, w, h)
		self.label.setAlign("left")
		self.label.setPosition(10, 0)
		
	def onMessageProc(self, msg, wParam, lParam) :
		if msg == Define.WM_MOUSEWHEEL :
			self.parent.onRoll(Lazy.getWheelDelta(wParam))
			
	def setData(self, data) :
		self.data = data
		self.text = data.getText()
	
	def setTextColor(self, cr) :
		self.label.color = cr
	
#列表视图
class ListView(Form) :
	def __init__(self, id, parent, image, x, y, w, h) :
		super(ListView, self).__init__(id, parent, image, x, y, w, h)
		self.enableDrag(False)
		self.biasIndex = 0 	#偏移索引
		self.data = None	#数据列表
		self.dataNums = 0	#数据个数
		self.scroll = Scroll(1, self, "", w-24, 0, h, 20)
		self.scroll.setVertical(True)
		self.selectedIndex = -1
		
	def getSelectedIndex(self) : return self.selectedIndex
	
	def getSelectedItem(self) :
		if self.selectedIndex>=0 and self.selectedIndex<self.dataNums :
			return self.data[self.selectedIndex]
		else :
			return None
	
	#创建子项
	def createItems(self, dh, ItemClass = ListItem) :
		dt = 2
		self.itemHeight = dh #列表项高度
		size = self.getSize()
		self.itemNums = size.h//(dh+dt) #列表项个数
		self.items = [] #列表项
		for i in range(self.itemNums) :
			self.items.append(ItemClass(i+100, self, "", 10, i*(dh+dt)+10, size.w-40, dh))
			
	#设置数据列表。data为数据列表
	def setData(self, data) :
		self.data = data
		self.dataNums = len(data)
		self.biasIndex = 0
		self.selectedIndex = -1
		self.updateData()
			
	#更新数据项
	def updateData(self) :
		for idx, item in enumerate(self.items) :
			index = idx+self.biasIndex
			if index < self.dataNums :
				item.setData(self.data[index])
				item.show(True)
				if index == self.selectedIndex :
					item.setTextColor(0xffff0000)
				else :
					item.setTextColor(0xffffffff)
			else :
				item.show(False)
		self.scroll.show(self.itemNums < self.dataNums)
		
	#矫正偏移索引
	def correctBiasIndex(self) :
		if self.biasIndex < 0 or self.itemNums >= self.dataNums:
			self.biasIndex = 0
		elif self.biasIndex+self.itemNums > self.dataNums :
			self.biasIndex = self.dataNums-self.itemNums
			
	def onMessageProc(self, msg, wParam, lParam) :
		if msg == Define.WM_MOUSEWHEEL :
			self.onRoll(Lazy.getWheelDelta(wParam))

	def onButton(self, id, isDown) :
		if not isDown :
			itemNo = id - 100
			if itemNo < 0:
				return
			itemNo += self.biasIndex
			self.onItemClick(itemNo)
	
	def onItemClick(self, itemNo) :
		self.selectedIndex = itemNo
		self.updateData()
		if hasattr(self.parent, "onItemSelect") :
			self.parent.onItemSelect(self.id, itemNo)
	
	#滚动消息响应
	def onRoll(self, dt) :
		print("onRoll", dt)
		if dt < 0 :
			self.biasIndex += 1
		elif dt > 0 :
			self.biasIndex -= 1
		self.correctBiasIndex()
		self.updateData()
		if self.itemNums < self.dataNums :
			self.scroll.setRate(self.biasIndex/float(self.dataNums-self.itemNums))
	
	def onScroll(self, id_, pos) :
		self.biasIndex = int(pos*(self.dataNums-self.itemNums))
		self.correctBiasIndex()
		self.updateData()

##################################################
#GUI管理器
##################################################
class GUI(Lazy.cGUIManager) :
	def __init__(self) :
		super(GUI, self).__init__(True)
		global gui
		gui = self
		print("GUI", self)
		self.topPanel = Panel(1, None, 0, 0)
		self.normalPanel = Panel(1, None, 0, 0)
		self.bottomPanel = Panel(1, None, 0, 0)
		self.initLayoutPanel(self.topPanel, self.normalPanel, self.bottomPanel)
		