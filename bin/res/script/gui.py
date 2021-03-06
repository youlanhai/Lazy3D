#-*- coding: utf-8 -*-

#此模块是对lui的二次封装


import lui
import ui
import copy
import weakref
import lzd
import ui_factory
import edt_const

def loadScript(script):
	split = script.rfind('.')
	if split < 0 :
		print("Invalid script name '%s'" % script)
		return None

	modName = script[ : split]
	clsName = script[split+1 : ]

	try:
		mod = __import__(modName, fromlist=(clsName, ))
	except ImportError:
		print("Failed loadScript '%s', module '%s' doesn't exist!" % (script, modName))
		return None

	try:
		cls = getattr(mod, clsName)
	except AttributeError:
		print("Failed loadScript '%s', attr '%s' doesn't exist!" % (script, clsName))
		return None

	return cls()


def loadUIScript(script, config):
	obj = loadScript(script)
	if obj is None: return None

	if not obj.loadFromFile(config):
		obj.destroy()
		return None

	return obj


def createUIFromStream(stream):
	assert(stream is not None)

	obj = None
	script = stream.readString("script")
	if len(script) > 0:
		obj = loadScript(script)
	
	if obj is None:
		type = stream.readInt("type")
		obj = ui_factory.createUI(type)
	
	return obj


def loadUIFromFile(configFile, parent=None):
	root = lzd.openLzd(configFile)
	if root is None: return None

	config = root.getFirstChild()
	if config is None: return None

	obj = createUIFromStream(config)
	if obj is None: return None

	if not obj.loadFromFile(configFile):
		obj.destroy()
		return None

	if parent: parent.addChild(obj)
	return obj

##################################################
### 方法不能直接使用弱引用。通过i型下面的方式来模拟。
##################################################
class MethodProxy(object):

	def __init__(self, obj, method, *args):
		super(MethodProxy, self).__init__()
		self._object = weakref.proxy(obj)
		self._method = method
		self._args = args

	def __call__(self, *vargs, **karg):
		args = self._args + vargs
		method = getattr(self._object, self._method)
		return method(*args, **karg)

##################################################
###
##################################################
class BaseScript(object):
	def __init__(self):
		super(BaseScript, self).__init__()
		self.ui = None

	def onBind(self, ui):
		self.ui = ui

	def onUnbind(self, ui):
		self.ui = None

##################################################
### 列表
##################################################

class ListItem(lui.IControl):
	def __init__(self):
		super(ListItem, self).__init__()
		self.height = 20
	
	def getHeight(self): return self.height
	
	def updateData(self): pass

	def clear(self): pass


class ListPanel(lui.IControl):
	def __init__(self, parent):
		super(ListPanel, self).__init__()
		if parent: parent.addChild(self)
		
		self.items = []
		self.height = 0
		self.lineSpace = 2

	def getHeight(self): return self.height

	def addItem(self, item):
		self.addChild(item)
		self.items.append(item)
	
	def delItem(self, item):
		if not item in self.items: return
		
		self.delChild(item)
		self.items.remove(item)
		item.clear()
	
	def clear(self):
		for item in copy.copy(self.items):
			self.delItem(item)
		
		self.items = []
		self.height = 0
	
	def layout(self):
		x = 0
		y = 0
		self.height = 0
		
		for i, item in enumerate(self.items):
			item.position = (x, y)
			self.updateItem(i, item)
			h = item.getHeight()
			y += h + self.lineSpace
		
		self.height = max(0, y - self.lineSpace)


#		print("Layout", len(self.items), self.height)
	
	def updateItem(self, i, item):
		item.updateData()


class ListControl(lui.Form):

	def __init__(self, parent):
		self.slidebar = None

		super(ListControl, self).__init__(parent)
		
		self.enableDrag = False
		self.enableClip = True

		self._createSlidebar()
		self._createList()
		
	def _createSlidebar(self):
		self.slidebar = lui.Slidebar(self)
		self.slidebar.vertical = True
		self.slidebar.onSlide = MethodProxy(self, "onSlide")

	def _createList(self):
		self.list = ListPanel(self)

	def onSizeChange(self):
		if self.slidebar is None: return

		w, h = self.size
		self.slidebar.position = (w - 20, 0)
		self.slidebar.size = (20, h)

		self.layout()

	def addItem(self, item):
		self.list.addItem(item)
	
	def delItem(self, item):
		self.list.delItem(item)
	
	def clear(self):
		self.list.clear()
		self.slidebar.rate = 0

	def layout(self):
		self.list.layout()
		self.layoutPosition()

	def layoutPosition(self):
		height = self.list.getHeight()  + 50

		w, h = self.size
		if height <= h:
			self.slidebar.visible = False
			self.list.position = (0, 0)
		else:
			self.slidebar.visible = True
			y = -(height - h) * self.slidebar.rate
			self.list.position = (0, int(y))

	def onSlide(self, pos):
		#print("onSlide", pos)

		self.layoutPosition()

	def onMouseWheel(self, x, y, z):
		if self.slidebar is None: return False
		self.slidebar.slideByWheel(z)

		return True

##################################################
### 只用于item类型相同的结构
##################################################
class IListItem(lui.IControl):

	SCRIPT_NAME = "gui.IListItem"

	def __init__(self, parent=None):
		super(IListItem, self).__init__(parent)
		self.layoutHeight = 20
		self.msgHandler = None
		self.label = None
		self.key = 0
	
	def setMsgHandler(self, msgHandler): self.msgHandler = msgHandler

	def getHeight(self): return self.layoutHeight
	
	def setInfo(self, info):
		if self.label is not None:
			self.label.text = str(info)

	def layout(self): self.layoutHeight = self.size[1]

	def onMouseEvent(self, msg, x, y):
		if msg == ui.ME_LMOUSE_UP:
			self.onSelect()
		return True

	def onSelect(self):
		print("onSelect", self.key)

		if self.msgHandler:
			self.msgHandler.onItemSelect(self.key)

	def getText(self):
		if self.label is not None: return self.label.text
		return ""

	def onLoadLayout(self, config):
		self.label = self.getChildByName("label")

	@classmethod
	def createUI(cls):
		e = cls()
		e.script = cls.SCRIPT_NAME
		e.name = "IListItem"
		e.size = (50, 30)

		e.label = lui.Label(e)
		e.label.name = "label"
		e.label.text = "label"
		e.label.enable = False

		return e


class IListView(IListItem):

	SCRIPT_NAME = "gui.IListView"

	def __init__(self, parent=None):
		super(IListView, self).__init__(parent)

		self.items = []
		self.itemCache = []
		self.layoutHeight = 0
		self.maxItemCache = 4
		self.lineSpace = 0
		self.itemCreateMethod = None

	def setItemCreateMethod(self, method):
		self.itemCreateMethod = method

	def setMsgHandler(self, msgHandler):
		self.msgHandler = msgHandler
		for item in self.items:
			item.setMsgHandler(msgHandler)

	##############################################
	### internal method
	##############################################

	def _createItem(self):
		item = None

		if len(self.itemCache) > 0:
			item = self.itemCache.pop()
		else:
			item = self.itemCreateMethod()
			self.addChild(item)

		item.visible = True
		item.key = len(self.items)
		item.setMsgHandler(self.msgHandler)
		self.items.append(item)
		return item

	def _releaseItem(self, item):
		if item not in self.items: return

		item.visible = False
		self.items.remove(item)
		self.itemCache.append(item)

	def _clearItems(self):
		items = copy.copy(self.items)
		for it in items: self._releaseItem(it)

	def _clearItemCache(self):
		n = len(self.itemCache) - self.maxItemCache
		while n > 0:
			item = self.itemCache.pop()
			item.destroy()
			n -= 1

		return

	##############################################
	### exposed method
	##############################################
	
	#info = (info1, info2, ...)
	def setInfo(self, info):
		self._clearItems()

		if info is not None:
			for v in info:
				item = self._createItem()
				item.setInfo(v)

		self._clearItemCache()

	def layout(self):
		x, y = 0, 0
		
		for i, item in enumerate(self.items):
			item.position = (x, y)
			item.layout()
			y += item.getHeight() + self.lineSpace
		
		self.layoutHeight = max(0, y - self.lineSpace)

	def getHeight(self): return self.layoutHeight

	def getNumItem(self): return len(self.items)


##################################################
###
##################################################
class ListView(lui.Form):

	SCRIPT_NAME = "gui.ListView"
	UI_EDITOR_PROPERTY = (
		edt_const.TP_EXTERNAL_LAYOUT,
		edt_const.TP_NUM_TESTCASE,
	)

	def getExternalLayout(self): return self.itemConfigFile
	def setExternalLayout(self, value): self.itemConfigFile = value


	def __init__(self, parent=None):
		self.slidebar = None

		super(ListView, self).__init__(parent)
		self.enableClip = True
		self.enableDrag = False

		self.root = None
		self.itemCreateMethod = IListItem.createUI
		self.itemConfigFile = ""

	def onLoadLayout(self, config):
		self.itemConfigFile = config.readString("itemConfig")

		self.root = IListView(self)
		self.root.setItemCreateMethod(MethodProxy(self, "_createItem"))

		self.slidebar = self.getChildByName("slidebar")
		if self.slidebar: self.slidebar.onSlide = MethodProxy(self, "onSlide")

	def onSaveLayout(self, config):
		if len(self.itemConfigFile) > 0:
			config.writeString("itemConfig", self.itemConfigFile)

	def _createItem(self):
		item = None

		if len(self.itemConfigFile) > 0:
			item = loadUIFromFile(self.itemConfigFile)
		elif self.itemCreateMethod:
			item = self.itemCreateMethod()
		
		return item

	def getItem(self, key):
		return self.root.items[key]

	def setItemCreateMethod(self, method):
		self.itemCreateMethod = method

	def setMsgHandler(self, msgHandler):
		self.root.setMsgHandler(msgHandler)

	def clear(self):
		self.setInfo(None)

	def setInfo(self, info):
		self.root.setInfo(info)
		self.layout()

	def layout(self):
		self.root.layout()
		self.layoutPosition()

	def onSizeChange(self):
		if self.slidebar is None: return

		w, h = self.size
		self.slidebar.position = (w - 20, 0)
		self.slidebar.size = (20, h)

		self.layoutPosition()

	def layoutPosition(self):
		height = self.root.getHeight()

		w, h = self.size
		if height <= h:
			self.slidebar.visible = False
			self.root.position = (0, 0)
		else:
			self.slidebar.visible = True
			y = -(height - h) * self.slidebar.rate
			self.root.position = (0, int(y))

	def onSlide(self, pos):
		self.layoutPosition()

	def onMouseWheel(self, x, y, z):
		if self.slidebar is None: return False
		self.slidebar.slideByWheel(z)

		return True

	@classmethod
	def createUI(cls):
		e = cls()

		e.slidebar = lui.Slidebar(e)
		e.slidebar.vertical = True
		e.slidebar.onSlide = MethodProxy(e, "onSlide")
		e.slidebar.name = "slidebar"
		e.slidebar.editable = True

		e.root = IListView(e)
		e.root.setItemCreateMethod(MethodProxy(e, "_createItem"))
		e.root.name = "item_root"

		#e.setItemCreateMethod(IListItem)
		e.size = (200, 300)
		e.script = cls.SCRIPT_NAME
		return e

	def setNumTestcase(self, n):
		info = [("list %d" % x, None) for x in range(n)]
		self.setInfo( info )

##################################################
###
##################################################
class Menu(ListView):

	SCRIPT_NAME = "gui.Menu"
	UI_EDITOR_PROPERTY = (edt_const.TP_MAX_HEIGHT, )

	def getMaxHeight(self): return self.maxHeight
	def setMaxHeight(self, h): self.maxHeight = h

	def __init__(self, parent=None):
		super(Menu, self).__init__(parent)

		self.maxHeight = 200

	def onLoadLayout(self, config):
		super(Menu, self).onLoadLayout(config)
		self.maxHeight = config.readInt("maxHeight", 200)

	def onSaveLayout(self, config):
		super(Menu, self).onSaveLayout(config)
		config.writeInt("maxHeight", self.maxHeight)

	def onSizeChange(self):
		if self.slidebar is None: return

		w, h = self.size
		self.slidebar.position = (w - 20, 0)
		self.slidebar.size = (20, h)

	def layoutPosition(self):
		height = self.root.getHeight()
		w, h = self.size
		h = min(self.maxHeight, height)
		self.size = (w, h)

		if height <= h:
			self.slidebar.visible = False
			self.root.position = (0, 0)
		else:
			self.slidebar.visible = True
			y = -(height - h) * self.slidebar.rate
			self.root.position = (0, int(y))

##################################################
###
##################################################
class ComboBox(lui.Form):

	SCRIPT_NAME = "gui.ComboBox"

	def __init__(self, parent=None):
		super(ComboBox, self).__init__(parent)
		self.onSelectChange = lambda x: None

	def onLoadLayout(self, config):
		self.button = self.getChildByName("button")
		self.menu = self.getChildByName("menu")

		self.bindScript()

	def onBtnDropdown(self):
		self.menu.visible = not self.menu.visible

	def bindScript(self):
		self.button.onButtonClick = MethodProxy(self, "onBtnDropdown")
		self.menu.setMsgHandler(weakref.proxy(self))

	def onItemSelect(self, key):
		self.setSelect(key)
		self.menu.visible = False
		self.onSelectChange(key)

	def setInfo(self, info):
		self.menu.setInfo(info)

	def setSelect(self, key):
		item = self.menu.getItem(key)
		self.button.text = item.getText()

	@classmethod
	def createUI(cls):
		w, h = 120, 20

		e = cls()
		e.name = "ComboBox"
		e.size = (w, h)
		e.enableDrag = False
		e.bgColor = 0xff7f7f7f
		e.enableLimitInRect = False
		e.script = cls.SCRIPT_NAME

		e.button = lui.Button(e)
		e.button.position = (0, 0)
		e.button.size = (w, h)
		e.button.name = "button"
		e.button.text = "combobox"
		e.button.posMovable = False
		e.button.editable = True

		e.menu = Menu.createUI()
		e.addChild(e.menu)
		e.menu.visible = False
		e.menu.position = (0, h)
		e.menu.size = (w, h*3)
		e.menu.setMaxHeight(h*4)
		e.menu.name = "menu"
		e.menu.editable = True
		e.menu.bgColor = 0xffafafaf

		e.bindScript()
		return e

##################################################
###
##################################################
class ITreeItem(IListItem):

	SCRIPT_NAME = "gui.ITreeItem"

	def __init__(self):
		super(ITreeItem, self).__init__()

		self.lineSpace = 5
		self.tableSpace = 25
		self.isExpand = True

	def onLoadLayout(self, config):
		self.lineSpace = config.readInt("lineSpace", 5)
		self.tableSpace = config.readInt("tableSpace", 25)

		self._list = self._createList()
		self._list.maxItemCache = config.readInt("maxItemCache", 0)
		self.addChild(self._list)

		self._title = self.getChildByName("title")
		
		self._btnExpand = self.getChildByName("btnExpand")
		self._btnExpand.onButtonClick = MethodProxy(self, "setExpand", True)

		self._btnShrink = self.getChildByName("btnShrink")
		self._btnShrink.onButtonClick = MethodProxy(self, "setExpand", False)

		self.setExpand(True)
		self._onCreate()

	##############################################
	### inner method
	##############################################

	def _onCreate(self): pass

	def _createList(self): return IListView()

	def _updateInfo(self, info): self._title.text = str(info)

	def _getInnerHeight(self): return self._title.size[1]

	def _updateChildInfo(self, info): self._list.setInfo(info)

	##############################################
	### exposed method
	##############################################

	def setExpand(self, expand):
		self.isExpand = expand
		self._btnShrink.visible = expand
		self._btnExpand.visible = not expand
		self._list.visible = expand
	
	#info = (self_info, info)
	def setInfo(self, info):

		if info is None: info = (None, None)

		selfInfo, childInfo = info
		self._updateInfo(selfInfo)
		self._updateChildInfo(childInfo)

	def layout(self):
		hasChild = self._list.getNumItem() != 0
		self._btnExpand.visible = hasChild and not self.isExpand
		self._btnShrink.visible = hasChild and self.isExpand

		x = self.tableSpace if hasChild else 0
		y = self._getInnerHeight() + self.lineSpace

		self._title.position = (x, 0)
		if self._list.visible:
			self._list.layout()
			self._list.position = (x, y)
			y += self._list.getHeight()
		
		self.layoutHeight = y

	def getHeight(self): return self.layoutHeight

	def setItemCreateMethod(self, method): self._list.itemCreateMethod = method


class ITree(lui.Form):

	SCRIPT_NAME = "gui.ITree"
	UI_EDITOR_PROPERTY = (
		edt_const.TP_EXTERNAL_LAYOUT,
		edt_const.TP_NUM_TESTCASE,
	)

	def getExternalLayout(self): return self.itemConfigFile
	def setExternalLayout(self, value): self.itemConfigFile = value

	def setNumTestcase(self, n):
		info = [("tree %d" % x, None) for x in range(n)]
		self.setInfo( ("root", info) )

	@property
	def root(self): return self.getRoot()

	def __init__(self, parent=None):

		self.slidebar = None

		super(ITree, self).__init__(parent)

		self.enableClip = True
		self.enableDrag = False

		self.__root = None
		self.itemCreateMethod = None

		self.itemConfigFile = ""

	def onLoadLayout(self, config):
		self.slidebar = self.getChildByName("slidebar")
		self.slidebar.onSlide = MethodProxy(self, "onSlide")

		self.itemConfigFile = config.readString("itemConfig")

		w, h = self.size
		self.clipRect = (0, 0, w, h)

	def onSaveLayout(self, config):
		if len(self.itemConfigFile) > 0:
			config.writeString("itemConfig", self.itemConfigFile)

	def _createItem(self):
		item = None

		if len(self.itemConfigFile) > 0:
			item = loadUIFromFile(self.itemConfigFile)
		elif self.itemCreateMethod:
			item = self.itemCreateMethod()
		
		if item: item.setItemCreateMethod(MethodProxy(self, "_createItem"))
		return item

	def setItemCreateMethod(self, method):
		self.itemCreateMethod = method

	def clear(self):
		self.__root.destroy()
		self.__root = None

	def getRoot(self):
		if self.__root is None:
			root = self._createItem()
			self.addChild(root)
			self.__root = root

		return self.__root

	def setInfo(self, info):
		self.root.setInfo(info)
		self.layout()

	def layout(self):
		self.root.layout()
		self.layoutPosition()

	def onSizeChange(self):
		if self.slidebar is None: return

		w, h = self.size
		
		self.slidebar.position = (w - 20, 0)
		self.slidebar.size = (20, h)

		self.layoutPosition()

	def layoutPosition(self):
		height = self.root.getHeight()

		w, h = self.size
		if height <= h:
			self.slidebar.visible = False
			self.root.position = (0, 0)
		else:
			self.slidebar.visible = True
			y = -(height - h) * self.slidebar.rate
			self.root.position = (0, int(y))

	def onSlide(self, pos):
		self.layoutPosition()
