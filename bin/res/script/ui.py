#-*- coding: utf-8 -*-

#此模块定义了游戏ui框架，以及一些常量。

import lui
from share import share


##################################################
### ui 窗口
##################################################

class Window(lui.IControl):
	def __init__(self, parent):
		super(Window, self).__init__(parent)
		self.enableChangeChildOrder = False
		self.enableHandleSelfMsg = False

##################################################
###
##################################################
#最先创建的会显示在最上

root = lui.IControl(lui.root()) #整个ui的根节点
root.enableChangeChildOrder = False
root.enableHandleSelfMsg = False
root.name = "PyUIRoot"

uiRoot     = Window(root)
uiRoot.name = "uiRoot"

editorUILayer = lui.Form(root)
editorUILayer.name = "editorUILayer"
editorUILayer.size = (640, 480)
editorUILayer.position = (200, 50)
editorUILayer.bgColor = 0xff7f7f7f

editorRoot = lui.IControl(root)
editorRoot.name = "editorRoot"

def onSize(w, h):
	#lui.root().size = (w, h)
	root.size = (w, h)
	uiRoot.size = (w, h)
	editorRoot.size = (w, h)

def onClientSize(w, h):
	uiRoot.size = (w, h)

##################################################
### 消息类型
##################################################


msg_mouseEnter      = 7
msg_mouseLeave      = 8
msg_mouseMove       = 9
msg_mouseLeftDrag   = 10
msg_mouseRightDrag  = 11
msg_getSelected     = 12
msg_lostSelected    = 13
msg_mouseWheel      = 14
msg_getActived      = 15
msg_lostActived     = 16

##################################################
### ui类型
##################################################

Control     = 1
Form        = 3
Button      = 4
Label       = 5
Image       = 6
Edit        = 7
Slidebar    = 9
CheckBox    = 10
SelectGroup = 11
Slider     = 12
GuiMgr      = 13
EditorCtl   = 14
UIProxy     = 15
ListView    = 16
Menu		= 17
CombBox 	= 18

def isContainerType(type):
	return True

##################################################
### 排版类型
##################################################

align_left      = 1
align_hcenter   = 2
align_right     = 4
align_top       = 8
align_vcenter   = 16
align_bottom    = 32
align_center    = align_hcenter | align_vcenter
align_mutiLine  = 64

##################################################
###
##################################################
ME_LMOUSE_DOWN = 0
ME_RMOUSE_DOWN = 1
ME_MMOUSE_DOWN = 2
ME_LMOUSE_UP = 3
ME_RMOUSE_UP = 4
ME_MMOUSE_UP = 5
ME_MOUSE_MOVE = 6
ME_MOUSE_WHEEL = 7
ME_LMOUSE_DOUBLE_CLICK = 8
ME_RMOUSE_DOUBLE_CLICK = 9
ME_MMOUSE_DOUBLE_CLICK = 10
