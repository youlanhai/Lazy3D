#-*- coding: utf-8 -*-

#此模块是对lui的二次封装


import lui
import copy
import weakref
import lzd
import ui_factory
import edt_const

def loadScript(script):
	mods = script.split('.')
	
	mod = mods.pop(0)
	if len(mod) == 0: return None

	try:
		cls = __import__(mod)
	except ImportError:
		print("loadScript '%s' failed, module '%s' doesn't exist!" % (script, mod))
		return None

	for mod in mods:
		try:
			cls = getattr(cls, mod)
		except AttributeError:
			print("loadScript '%s' failed, attr '%s' doesn't exist!" % (script, mod))
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
class Label(lui.Label):
	def __init__(self, parent, x, y, text):
		super(Label, self).__init__(parent)

		self.position = (x, y)
		self.text = text

##################################################
###
##################################################
class Edit(lui.Edit):
	pass
	
##################################################
###
##################################################
class Button(lui.Button):
	pass


class CheckBox(lui.CheckBox):
	def __init__(self, parent, x, y):
		super(CheckBox, self).__init__(parent)
		self.position = (x, y)

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
		height = self.list.getHeight()

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

##################################################
### 只用于item类型相同的结构
##################################################
class IListItem(lui.IControl):
	def __init__(self):
		super(IListItem, self).__init__()
		self.layoutHeight = 20
	
	def getHeight(self): return self.layoutHeight
	
	def setInfo(self, info): pass

	def layout(self): pass


class IListView(lui.IControl):
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
		self.itemCreateMethod = None

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
		if self.itemCreateMethod:
			item = self.itemCreateMethod()
		else:
			item = loadUIFromFile(self.itemConfigFile)
		
		return item

	def setItemCreateMethod(self, method):
		self.itemCreateMethod = method

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

		e.script = "gui.ListView"
		return e

	def setNumTestcase(self, n):
		info = [("list %d" % x, None) for x in range(n)]
		self.setInfo( info )

##################################################
###
##################################################
class ITreeItem(IListItem):
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

	UI_EDITOR_PROPERTY = (
		edt_const.TP_EXTERNAL_LAYOUT,
		edt_const.TP_EXTERNAL_SCRIPT,
		edt_const.TP_NUM_TESTCASE,
	)

	def getExternalScript(self): return self.itemClassFile
	def setExternalScript(self, value): self.itemClassFile = value

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

		self.itemClassFile = ""
		self.itemConfigFile = ""

	def onLoadLayout(self, config):
		self.slidebar = self.getChildByName("slidebar")
		self.slidebar.onSlide = MethodProxy(self, "onSlide")

		self.itemClassFile = config.readString("itemClass")
		self.itemConfigFile = config.readString("itemConfig")

		w, h = self.size
		self.clipRect = (0, 0, w, h)

	def onSaveLayout(self, config):
		if len(self.itemConfigFile) > 0:
			config.writeString("itemConfig", self.itemConfigFile)

		if len(self.itemClassFile) > 0:
			config.writeString("itemClass", self.itemClassFile)

	def _createItem(self):
		if self.itemCreateMethod:
			item = self.itemCreateMethod()
		else:
			item = loadUIScript(self.itemClassFile, self.itemConfigFile)
		item.setItemCreateMethod(MethodProxy(self, "_createItem"))
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

