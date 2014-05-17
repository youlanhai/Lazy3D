#-*- coding:utf-8 -*-

import lui
import ui

UI_FACTORY = {
	ui.Form				: lui.Form,
	ui.Button 			: lui.Button,
	ui.Label 			: lui.Label,
	ui.Image 			: lui.Image,
	ui.Edit 			: lui.Edit,
	ui.Slidebar 		: lui.Slidebar,
	ui.CheckBox 		: lui.CheckBox,
	ui.EditorCtl 		: lui.EditorCtl,
	ui.UIProxy			: lui.UIProxy,
}

def registeUI(type, method):
	UI_FACTORY[type] = method

def createUI(type):
	method = UI_FACTORY[type]
	return method()
