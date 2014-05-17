# -*- coding: utf-8 -*-

#此模块为ui编辑器辅助模块

import lui
import gui
import ui_helper

import weakref

##################################################
###
##################################################
class BaseItem(gui.ListItem):
	def __init__(self):
		super(BaseItem, self).__init__()
		self.height = 20
		
		self.control = None
		self._lblName = gui.Label(self, 0, 0, self.getName())
		
	def clear(self):
		self.control = None

	def getName(self): return ""
	def getCtlValue(self): return ""
	def setCtlValue(self, text): pass
	
	def getBiasX(self): return 80

	def setControl(self, control):
		self.control = weakref.proxy(control)
	
	def updateData(self): pass


class _labelItem(BaseItem):
	def __init__(self):
		super(_labelItem, self).__init__()
		
		x = self.getBiasX()
		self.value = gui.Label(self, x, 0, "")
		self.value.size = (200, 20)
	
	def updateData(self):
		self.value.text = (self.getCtlValue())

class _stringItem(BaseItem):
	def __init__(self):
		super(_stringItem, self).__init__()
		
		x = self.getBiasX()
		self.value = gui.Edit(self)
		self.value.mutiline = (False)
		self.value.position = (x, 0)
		self.value.size = (200, 20)
		self.value.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
	
	def updateData(self):
		self.value.text = self.getCtlValue()
	
	def onEditLostFocus(self):
		self.setCtlValue(self.value.text)

class _boolItem(BaseItem):
	def __init__(self):
		super(_boolItem, self).__init__()
		
		x = self.getBiasX()
		self.value = gui.CheckBox(self, x, 0)
		self.value.size = (20, 20)
		self.value.onButtonCheck = gui.MethodProxy(self, "onCheck")
		
	def updateData(self):
		self.value.check = bool(self.getCtlValue())
	
	def onCheck(self, check):
		self.setCtlValue(check)

class _vector2Item(BaseItem):
	def __init__(self):
		super(_vector2Item, self).__init__()
		
		x = self.getBiasX()
		self.x = gui.Edit(self)
		self.x.mutiline = (False)
		self.x.position = (x, 0)
		self.x.size = (50, 20)
		self.x.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
		
		self.y = gui.Edit(self)
		self.y.mutiline = (False)
		self.y.position = (x+60, 0)
		self.y.size = (50, 20)
		self.y.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")

	def updateData(self):
		x, y = self.getCtlValue()
		self.x.text = (str(x))
		self.y.text = (str(y))
	
	def onEditLostFocus(self):
		x = y = 0
		try:
			x = int(self.x.text)
		except:
			pass
		
		try:
			y = int(self.y.text)
		except:
			pass
		
		self.setCtlValue( (x, y) )


class _vector2fItem(BaseItem):
	def __init__(self):
		super(_vector2fItem, self).__init__()
		
		x = self.getBiasX()
		self.x = gui.Edit(self)
		self.x.mutiline = (False)
		self.x.position = (x, 0)
		self.x.size = (50, 20)
		self.x.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
		
		self.y = gui.Edit(self)
		self.y.mutiline = (False)
		self.y.position = (x+60, 0)
		self.y.size = (50, 20)
		self.y.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
		
	def updateData(self):
		x, y = self.getCtlValue()
		self.x.text = ("%.3f" % x)
		self.y.text = ("%.3f" % y)
	
	def onEditLostFocus(self):
		x = y = 0
		try:
			x = float(self.x.text)
		except:
			pass
		
		try:
			y = float(self.y.text)
		except:
			pass
		
		self.setCtlValue( (x, y) )


class _hexItem(BaseItem):
	def __init__(self):
		super(_hexItem, self).__init__()
		x = self.getBiasX()
		self.value = gui.Edit(self)
		self.value.mutiline = (False)
		self.value.position = (x, 0)
		self.value.size = (200, 20)
		self.value.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
	
	def updateData(self):
		v = self.getCtlValue()
		try:
			v = hex(v)
		except:
			v = "0"
		self.value.text = (v)
	
	def onEditLostFocus(self):
		v = self.value.text
		try:
			v = int(v, 16)
		except:
			v = 0
		self.setCtlValue(v)

class _intItem(BaseItem):
	def __init__(self):
		super(_intItem, self).__init__()
		x = self.getBiasX()
		self.value = gui.Edit(self)
		self.value.mutiline = (False)
		self.value.position = (x, 0)
		self.value.size = (100, 20)
		self.value.onFocusLost = gui.MethodProxy(self, "onEditLostFocus")
	
	def updateData(self):
		v = self.getCtlValue()
		try:
			v = str(v)
		except:
			v = "0"
		self.value.text = (v)
	
	def onEditLostFocus(self):
		v = self.value.text
		try:
			v = int(v)
		except:
			v = 0
		self.setCtlValue(v)
		

##################################################
###
##################################################
class NameItem(_stringItem):
	def getName(self): return "名称"
	def getCtlValue(self): return self.control.name
	def setCtlValue(self, v): self.control.name = v

class ClassItem(_labelItem):
	def getName(self): return "类"
	def getCtlValue(self): return ui_helper.ctlGetClass(self.control)

class ParentItem(_labelItem):
	def getName(self): return "父亲"
	def getCtlValue(self): return ui_helper.ctlGetDescName(self.control.parent)

class IDItem(_intItem):
	def getName(self): return "ID"
	def getCtlValue(self): return self.control.id
	def setCtlValue(self, v): self.control.id = v		

class TextItem(_stringItem):
	def getName(self): return "文本"
	def getCtlValue(self): return self.control.text
	def setCtlValue(self, v): self.control.text = v

class FontItem(_stringItem):
	def getName(self): return "字体"
	def getCtlValue(self): return self.control.font
	def setCtlValue(self, v): self.control.font = v

class ImageItem(_stringItem):
	def getName(self): return "图片"
	def getCtlValue(self): return self.control.image
	def setCtlValue(self, v): self.control.image = v

class ShowItem(_boolItem):
	def getName(self): return "显示"
	def getCtlValue(self): return self.control.visible
	def setCtlValue(self, v): self.control.visible = v

class EnableItem(_boolItem):
	def getName(self): return "启用"
	def getCtlValue(self): return self.control.enable
	def setCtlValue(self, v): self.control.enable = v

class DragItem(_boolItem):
	def getName(self): return "拖拽"
	def getCtlValue(self): return self.control.enableDrag
	def setCtlValue(self, v): self.control.enableDrag = v
	
class MessageItem(_boolItem):
	def getName(self): return "自己消息"
	def getCtlValue(self): return self.control.enableSelfMsg
	def setCtlValue(self, v): self.control.enableSelfMsg = v

class ClickTopItem(_boolItem):
	def getName(self): return "置顶"
	def getCtlValue(self): return self.control.enableClickTop
	def setCtlValue(self, v): self.control.enableClickTop = v

class PositionItem(_vector2Item):
	def getName(self): return "位置"
	def getCtlValue(self): return self.control.position
	def setCtlValue(self, v): self.control.position = v

class SizeItem(_vector2Item):
	def getName(self): return "尺寸"
	def getCtlValue(self): return self.control.size
	def setCtlValue(self, v): self.control.size = v

class RelativeItem(_boolItem):
	def getName(self): return "启用相对"
	def getCtlValue(self): return self.control.relative
	def setCtlValue(self, v): self.control.relative = v

class RelativePosItem(_vector2fItem):
	def getName(self): return "相对坐标"
	def getCtlValue(self): return self.control.relativePos
	def setCtlValue(self, v): self.control.relativePos = v

class RelativeAlignItem(_intItem):
	def getName(self): return "相对排版"
	def getCtlValue(self): return self.control.relativeAlign
	def setCtlValue(self, v): self.control.relativeAlign = v

class ColorItem(_hexItem):
	def getName(self): return "颜色"
	def getCtlValue(self): return self.control.color
	def setCtlValue(self, v): self.control.color = v

class BgColorItem(_hexItem):
	def getName(self): return "底色"
	def getCtlValue(self): return self.control.bgColor
	def setCtlValue(self, v): self.control.bgColor = v

class ScriptKeyItem(_stringItem):
	def getName(self): return "脚本"
	def getCtlValue(self): return self.control.script
	def setCtlValue(self, v): self.control.script = v

class MutilineItem(_boolItem):
	def getName(self): return "多行"
	def getCtlValue(self): return self.control.mutiline
	def setCtlValue(self, v): self.control.mutiline = v


class EditableItem(_boolItem):
	def getName(self): return "可编辑"
	def getCtlValue(self): return self.control.editable
	def setCtlValue(self, v): self.control.editable = v

##################################################
### 选项类型
##################################################

TP_TEXT = 1
TP_FONT = 2
TP_IMAGE = 3
TP_COLOR = 4
TP_BGCOLOR = 5
TP_POSITION = 6
TP_SIZE = 7
TP_SHOW = 8
TP_ENABLE = 9
TP_CLICKTOP = 10
TP_HANDEL_MSG = 11
TP_NAME = 12
TP_CLASS = 13
TP_PARENT = 14
TP_ID = 17
TP_DRAG = 18
TP_RELATIVE = 19
TP_RELATIVE_POS = 20
TP_RELATIVE_ALIGN = 21
TP_MUTILINE = 22
TP_SCRIPT = 23
TP_EDITABLE = 24

#用户自定义格式的编号，从TP_CUSTOM往后排
TP_CUSTOM = 10000

##################################################
###
##################################################
TP_CLASS_MAP = {
	TP_NAME		: NameItem,
	TP_CLASS 	: ClassItem,
	TP_PARENT 	: ParentItem,
	TP_TEXT 	: TextItem,
	TP_FONT 	: FontItem,
	TP_IMAGE 	: ImageItem,
	TP_POSITION : PositionItem,
	TP_SIZE 	: SizeItem,
	TP_SHOW 	: ShowItem,
	TP_ENABLE 	: EnableItem,
	TP_CLICKTOP : ClickTopItem,
	TP_HANDEL_MSG : MessageItem,
	TP_COLOR 	: ColorItem,
	TP_BGCOLOR	: BgColorItem,
	TP_ID 		: IDItem,
	TP_DRAG		: DragItem,
	TP_RELATIVE : RelativeItem,
	TP_RELATIVE_POS: RelativePosItem,
	TP_RELATIVE_ALIGN: RelativeAlignItem,
	TP_MUTILINE : MutilineItem,
	TP_SCRIPT 	: ScriptKeyItem,
	TP_EDITABLE : EditableItem,
}

COMMON_TYPES = (
	TP_NAME, TP_CLASS, 
	TP_PARENT, TP_ID,
	TP_SCRIPT, TP_EDITABLE,
	TP_POSITION, TP_SIZE,
	TP_TEXT, TP_IMAGE, TP_FONT,
	TP_SHOW, TP_ENABLE, TP_CLICKTOP, TP_HANDEL_MSG, TP_DRAG,
	TP_COLOR, TP_BGCOLOR, TP_RELATIVE, TP_RELATIVE_POS, TP_RELATIVE_ALIGN,
)

#内建ui的属性列表写在这里，非内建类型，写在类属性UI_EDITOR_PROPERTY中
GUI_TYPES_MAP = {
	lui.Label : (TP_MUTILINE, ),
	lui.Edit : (TP_MUTILINE, ),
}


def class2properties(cls):
	properties = list(COMMON_TYPES)

	mro = list(cls.__mro__)
	mro.reverse()
	for cc in mro:
		if cc.__module__ == "builtins":
			v = GUI_TYPES_MAP.get(cls, ())
		else:
			v = getattr(cc, "UI_EDITOR_PROPERTY", ())
		properties.extend(v)

	ret = []
	for prop in properties:
		if prop not in ret: ret.append(prop)
	return ret


ITEM_CACHE = {}

def class2propertyItems(cls):
	items = []
	properties = class2properties(cls)

	for prop in properties:
		item = ITEM_CACHE.get(prop)
		if item is None:
			item = TP_CLASS_MAP[prop]()
			ITEM_CACHE[prop] = item
		items.append(item)

	return items

def registerTpMethod(tp, method): TP_CLASS_MAP[tp] = method

##################################################
###
##################################################
