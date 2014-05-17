#-*- coding:utf-8 -*-

#ui编辑器
#import traceback

import lui
import ui
import vk
import gui
import ui_helper
from . import edt_property
from . import edt_creator

from share import share

##################################################
###
##################################################
class Selector(lui.EditorCtl):
	def __init__(self, parent):
		super(Selector, self).__init__()
		if parent: parent.addChild(self)

		self.visible = False
		
		self.edgeSize = 8
		self.activeCtl = None
	
	def setRect(self, left, top, right, bottom):
		self.position = (left, top)
		self.size = (right-left, bottom-top)
	
	def applyCtl(self, ctl):
		self.activeCtl = ctl

		if not ctl:
			self.visible = False
			return
		
		self.visible = True
		self.globalRect = ctl.globalRect
	
	def onMouseEvent(self, msg, x, y):
		#print("onMouseEvent", msg, x, y, self)
		if msg == ui.ME_MOUSE_MOVE:
			self.onMouseMove(x, y)
		
		elif msg == ui.ME_LMOUSE_DOWN:
			self.onMouseDown(x, y)

		return True
		
	def onGuiEvent(self, msg, wparam, lparam):
		#print("onGuiEvent", msg, wparam, lparam, self)
		if msg == ui.msg_mouseLeftDrag:
			self.onMouseDrag()
	
	def onMouseMove(self, x, y):
		x, y = self.localToParent(x, y)
		
		self.parent.onMouseMove(x, y)
	
	def onMouseDown(self, x, y):
		x, y = self.localToParent(x, y)
		
		self.parent.onMouseDown(x, y)
	
	def onMouseDrag(self):
		if not self.activeCtl: return
		
		self.activeCtl.globalRect = self.globalRect
		self.parent.refreshProperty()

	def moveDelta(self, dx, dy):
		x, y = self.getPosition()
		self.position = (x + dx, y + dy)
		self.onMouseDrag()

	def resizeDelta(self, dw, dh):
		w, h = self.size
		self.size = (w + dw, h + dh)
		self.onMouseDrag()


##################################################
### ui编辑器
##################################################
class Editor(lui.IControl):
	
	def __init__(self):
		super(Editor, self).__init__(ui.editorRoot)

		self.size = (1000, 1000)

		self.doInit()
		
		print("Editor dict", self.__dict__)

	def doInit(self):
		self.host = None
		self.reference = None
		self.target = None

		self.enableChangeChildOrder = False

		self.focus = Selector(self)
		self.focus.enableHandleMsg = False
		self.focus.color = 0
		self.focus.bgColor = 0xff0000ff

		self.selector = Selector(self)
		self.selector.bgColor = 0xffff0000

		self.floatBar = FloatBar(self, 600, 10, 300, 450)
		self.floatBar.relativeAlign = ui.align_right|ui.align_vcenter
		self.floatBar.relativePos = (1.0, 0.5)
		self.floatBar.relative = True

		self.workTree = gui.loadUIFromFile("layout/editor/worktree.lzd", self)

		self.resculySearch = True
		self.workStack = []
		self.setReference(None)

	def clearState(self):
		self.setReference(None)
		self.workStack = []

		self.selector.applyCtl(None)
		self.focus.applyCtl(None)
		self.refreshProperty()
		

	def enableResculySearch(self, enable):
		self.resculySearch = enable

	def pushReference(self):
		if self.selector.activeCtl == self.reference: return

		reference = self.selector.activeCtl

		if not isinstance(reference, lui.Form):
			raise RuntimeError("Please select one lui Form")

		if not reference.parent:
			raise RuntimeError("This ui control can't be set to reference!")

		self.workStack.append( self.reference )

		self.setReference(reference)

	def popReference(self):
		if len(self.workStack) == 0:
			raise RuntimeError("Current stack doen't exist any reference!")

		reference = self.workStack.pop(-1)
		self.setReference(reference)

	def searchChild(self, x, y):
		if self.reference is None: return None

		x, y = self.reference.globalToLocal(x, y)
		return self.reference.findChildByPos(x, y, self.resculySearch)
	
	def setSize(self, w, h):
		self.size = (w, h)
	
	def onMouseEvent(self, msg, x, y):
		#print("onMouseEvent", msg, x, y)
		
		if msg == ui.ME_MOUSE_MOVE:
			self.onMouseMove(x, y)
		
		elif msg == ui.ME_LMOUSE_DOWN:
			self.onMouseDown(x, y)

		return True
	
	def onMouseMove(self, x, y):
		ctl = self.searchChild(x, y)
		if ctl == self.selector.activeCtl:
			ctl = None
		
		self.focus.applyCtl(ctl)
	
	def onMouseDown(self, x, y):
		ctl = self.searchChild(x, y)

		print("onMouseDown", ctl)

		self.selector.applyCtl(ctl)
		self.refreshProperty()
	
	def onKeyEvent(self, isDown, key):
		#print("onKeyMessage", isDown, key)
		if isDown:
			fun = self.selector.resizeDelta if lui.isVkDown(vk.CONTROL) else self.selector.moveDelta
			if key == vk.LEFT:
				fun(-1, 0)
				return True
			elif key == vk.RIGHT:
				fun(1, 0)
				return True
			elif key == vk.UP:
				fun(0, -1)
				return True
			elif key == vk.DOWN:
				fun(0, 1)
				return True
		else:
			if key == vk.ESCAPE:
				self.visible = False
				return True
		
		return False

	def setHost(self, host, clearOld=False):
		if clearOld:
			if self.host is not None:
				self.host.destroy()
				self.host = None

		self.clearState()
		self.host = host
		self.setReference(host)
		self.setTarget(host)
	
	def setReference(self, reference):
		self.reference = reference
		self.refreshReference()

	def refreshReference(self):
		self.workTree.applyCtl(self.reference)
		self.floatBar.setReference(self.reference)

	def setTarget(self, target):
		self.selector.applyCtl(target)
		self.refreshProperty()

	def refreshProperty(self):
		self.floatBar.applyCtl(self.selector.activeCtl)

	def onNewItem(self, kind):
		item = edt_creator.create_ui(kind)
		print("onNewItem", kind, item)

		if not item: return

		if not self.host:
			self.host = item
			self.reference.addChild(item)
		else:
			self.host.addEditorChild(item)

		self.refreshReference()
		self.setTarget(item)

##################################################
### 浮动工具条
##################################################
class FloatBar(lui.Form):
	def __init__(self, parent, x, y, w, h):
		super(FloatBar, self).__init__(parent)

		self.bgColor = 0xaf000000
		self.position = (x, y)
		self.size = (w, h)

		self.title = gui.Label(self, 0, 0, "aaaaaaaaaaaaaaaaaaaaaaa")
		self.title.color = (0xffff0000)

		self.tabButton = []
		texts = ("菜单", "属性", "工具")
		for i, text in enumerate(texts):
			button = gui.Button(self)
			button.id = i
			button.text = text
			button.position = (10 + i*100, 30)
			button.size = (80, 20)
			button.onButtonClick = gui.MethodProxy(self, "onButtonClick", i)

			self.tabButton.append(button)

		rect = (10, 50, w-20, 400)
		self.menuBar = MenuBar(self, *rect)
		self.propBar = PropertyBar(self, *rect)
		self.toolBar = ToolBar(self, *rect);

		self.panels = (self.menuBar, self.propBar, self.toolBar)

		self.turnToPage(0)

	def applyCtl(self, ctl):
		self.propBar.applyCtl(ctl)
		#self.turnToPage(1)

	def setReference(self, target):
		text = "参考系：" + ui_helper.ctlGetDescName(target)
		self.title.text = (text)

	def onButtonClick(self, id):
		self.turnToPage(id)

	def turnToPage(self, pageIdx):
		for i, p in enumerate(self.panels):
			p.visible = (i == pageIdx)

##################################################
### 工具条
##################################################
class ToolBar(lui.IControl):
	def __init__(self, parent, x, y, w, h):
		super(ToolBar, self).__init__()
		if parent: parent.addChild(self)

		self.bgColor = (0xaf000000)
		self.position = (x, y)
		
		self.title = gui.Label(self, 0, 10, "工具条")
		self.title.color = (0xff00ff00)
		
		self.toolList = ToolList(self, 0, 30)

	def onNewItem(self, kind):
		share.gui.editor.onNewItem(kind)

	def applyCtl(self, control):
		pass


class ToolItem(gui.ListItem):
	def __init__(self, text, callback):
		super(ToolItem, self).__init__()
		self.height = 30

		self.button = gui.Button(self)
		self.button.text = text
		self.button.size = (100, self.height)
		self.button.onButtonClick = callback

class ToolList(gui.ListPanel):
	def __init__(self, parent, x, y):
		super(ToolList, self).__init__(parent)
		self.position = (x, y)
		self.createList()
	
	def createList(self):
		for kind in edt_creator.get_ui_keys():
			text = edt_creator.get_ui_name(kind)
			self.addItem( ToolItem(text, gui.MethodProxy(self, "onItemClick", kind)) )

		self.layout()

	def onItemClick(self, kind):
		share.gui.editor.onNewItem(kind)


##################################################
### 属性条
##################################################
class PropertyBar(lui.IControl):
	def __init__(self, parent, x, y, w, h):
		super(PropertyBar, self).__init__(parent)
		
		if not self.loadFromFile("layout/propertybar.lzd"):
			print("Failed load PropertyBar from file.")
			return

		self.position = (x, y)
		self.size = (w, h)
		self.title = self.getChildByName("title")

		self.btnPushTarget = self.getChildByName("pushTarget")
		self.btnPushTarget.onButtonClick = gui.MethodProxy(self, "onBtnPush")

		self.btnPopTarget = self.getChildByName("popTarget")
		self.btnPopTarget.onButtonClick = gui.MethodProxy(self, "onBtnPop")

		self.btnSearchResculy = self.getChildByName("searchResculy")
		self.btnSearchResculy.onButtonCheck = gui.MethodProxy(self, "onBtnResculy")

		self.propList = PropertyList(self, 0, 45, w, 300)
		
	def applyCtl(self, control):
		self.propList.applyCtl(control)

		isPanel = isinstance(control, lui.Form)
		self.btnPushTarget.visible = isPanel
		self.btnPopTarget.visible = not isPanel

	def onBtnPush(self):
		try:
			share.gui.editor.pushReference()
		except RuntimeError as e:
			print("pop target failed!", e)

	def onBtnPop(self):
		try:
			share.gui.editor.popReference()
		except RuntimeError as e:
			print("push target failed!", e)

	def onBtnResculy(self, isCheck):
		share.gui.editor.enableResculySearch(isCheck)

class PropertyList(gui.ListControl):
	def __init__(self, parent, x, y, w=300, h=300):
		super(PropertyList, self).__init__(parent)

		self.control = None

		self.position = (x, y)
		self.size = (w, h)
	
	def applyCtl(self, ctl):
		if ctl != self.control:
			self.control = ctl
			self.createList(self.control)

		self.layout()
	
	def createList(self, control):
		self.clear()
		if control is None: return

		items = edt_property.class2propertyItems(control.__class__)
		
		for item in items:
			item.setControl(control)
			self.addItem( item )

##################################################
### 菜单
##################################################
class MenuBar(lui.IControl):
	def __init__(self, parent, x, y, w, h):
		super(MenuBar, self).__init__(parent)

		self.loadFromFile("layout/editor/menu.lzd")

		self.position = (x, y)
		self.size = (w, h)

		self.layoutFile = ""

		self.edtFile = self.getChildByName("edt_file")
		self.edtFile.text = "layout/layout.lzd"

		self.btnOpen = self.getChildByName("btn_open")
		self.btnOpen.onButtonClick = gui.MethodProxy(self, "onBtnOpen")

		self.btnSave = self.getChildByName("btn_save")
		self.btnSave.onButtonClick = gui.MethodProxy(self, "onBtnSave")

		self.btnClose = self.getChildByName("btn_close")
		self.btnClose.onButtonClick = gui.MethodProxy(self, "onBtnClose")

	def onBtnOpen(self):
		path = self.edtFile.text
		if len(path) == 0:
			print("Path is None!")
			return

		if path == self.layoutFile: return
		self.layoutFile = path


		host = gui.loadUIFromFile(self.layoutFile)
		if not host:
			print("Failed to load layout ", self.layoutFile)
			return
		
		ui.uiRoot.addChild(host)
		share.gui.editor.setHost(host)

	def onBtnSave(self):
		path = self.edtFile.text
		if len(path) == 0:
			print("Path is None!")
			return

		editor = share.gui.editor
		if editor.host is None:
			print("Host is None")
			return

		if editor.host.saveToFile(path):
			print("Succed to save layout ", path)
		else:
			print("Failed to save layout ", path)

	def onBtnClose(self):
		share.gui.editor.setHost(None)
		self.layoutFile = ""


##################################################
### 
##################################################
class WorkTreeBar(lui.Form):

	def __init__(self, parent=None):
		super(WorkTreeBar, self).__init__(parent)

	def onLoadLayout(self, config):
		#self.tree = gui.ITree(self)
		#self.tree.loadFromFile("layout/worktree_tree.lzd")
		self.tree = self.getChildByName("root")

	def applyCtl(self, ctl):
		infos = self.getControlInfo(ctl) if ctl else None
		self.tree.setInfo(infos)
		self.tree.layout()

	def getControlInfo(self, ctl):
		childInfo = []
		children = ctl.getChildren()
		for child in children:
			childInfo.append(self.getControlInfo(child))

		return (ctl, childInfo)

class WorkTree(gui.ITree):
	pass

class WorkTreeItem(gui.ITreeItem):

	def _onCreate(self):
		super(WorkTreeItem, self)._onCreate()
		self.control = None
		self._title.onButtonClick = gui.MethodProxy(self, "_onBtnClick")

	#info is a lui control
	def _updateInfo(self, info):
		self.control = info
		self._title.text = info.name if info else ""

	def _onBtnClick(self):
		share.gui.editor.setTarget(self.control)
