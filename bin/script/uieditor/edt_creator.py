# -*- coding: utf-8 -*-

import lui
import ui
import ui_factory


##################################################
###
##################################################

def createForm():
	ctl = lui.Form()
	ctl.size = (300, 200)
	ctl.bgColor = 0xffffffff
	return ctl

def createButton():
	ctl = lui.Button()
	ctl.text = "button"
	ctl.size = (80, 30)
	return ctl

def createLabel():
	ctl = lui.Label()
	ctl.text = "label"
	return ctl

def createImage():
	ctl = lui.Image()
	ctl.size = (100, 100)
	ctl.bgColor = 0xffffffff
	return ctl

def createEdit():
	ctl = lui.Edit()
	ctl.size = (100, 30)
	ctl.text = "edit"
	return ctl

def createSlidebar():
	ctl = lui.Slidebar()
	ctl.size = (100, 30)
	return ctl

def createCheckBox():
	ctl = lui.CheckBox()
	ctl.size = (30, 30)
	return ctl

##################################################
###
##################################################

UI_MAP = {
	ui.Form : 		("窗体", createForm),
	ui.Button : 	("按钮", createButton),
	ui.Image : 		("图片", createImage),
	ui.Label : 		("标签", createLabel),
	ui.Edit : 		("编辑框", createEdit),
	ui.CheckBox : 	("单选框", createCheckBox),
	ui.Slidebar : 	("滚动条", createSlidebar),
}

def get_ui_keys(): return tuple(sorted(UI_MAP.keys()))
def get_ui_name(key): return UI_MAP[key][0]
def get_ui_method(key): return UI_MAP[key][1]

def create_ui(kind):
	if kind in UI_MAP: return get_ui_method(kind)()

	return ui_factory.createUI(kind)

#调用此接口，注册自定义创建方法
def regist_ui(kind, name, method):
	UI_MAP[kind] = (name, method)
