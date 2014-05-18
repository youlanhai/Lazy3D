# -*- coding: utf-8

import ui
import gui

import edt_const
from uieditor import edt_property, edt_creator

##################################################
###
##################################################

class ExternalScriptItem(edt_property._stringItem):
	def getName(self): return "引用脚本"
	def getCtlValue(self): return self.control.getExternalScript()
	def setCtlValue(self, v): return self.control.setExternalScript(v)

class ExternalLayoutItem(edt_property._stringItem):
	def getName(self): return "引用配置"
	def getCtlValue(self): return self.control.getExternalLayout()
	def setCtlValue(self, v): return self.control.setExternalLayout(v)

class NumTestcaseItem(edt_property._intItem):
	def __init__(self):
		super(NumTestcaseItem, self).__init__()
		self.numTestcase = 0

	def getName(self): return "用例数量"
	def getCtlValue(self): return self.numTestcase
	def setCtlValue(self, v): return self.control.setNumTestcase(v)

class MaxHeightItem(edt_property._intItem):
	def getName(self): return "最大高度"
	def getCtlValue(self): return self.control.getMaxHeight()
	def setCtlValue(self, v): return self.control.setMaxHeight(v)

class MaxWidthItem(edt_property._intItem):
	def getName(self): return "最大宽度"
	def getCtlValue(self): return self.control.getMaxWidth()
	def setCtlValue(self, v): return self.control.setMaxWidth(v)

##################################################
###
##################################################
CUSTEOM_TP_MAP = {
	edt_const.TP_EXTERNAL_SCRIPT: ExternalScriptItem,
	edt_const.TP_EXTERNAL_LAYOUT : ExternalLayoutItem,
	edt_const.TP_NUM_TESTCASE : NumTestcaseItem,
	edt_const.TP_MAX_HEIGHT : MaxHeightItem,
	edt_const.TP_MAX_WIDTH : MaxWidthItem,
}


CUSTOM_UI_MAP = {
	ui.ListView : ("列表", gui.ListView.createUI, ),
	ui.Menu : ("菜单", gui.Menu.createUI, ),
}

##################################################
###
##################################################

def boot():
	for k, v in CUSTEOM_TP_MAP.items():
		edt_property.registerTpMethod(k, v)

	for k, (name, method) in CUSTOM_UI_MAP.items():
		edt_creator.regist_ui(k, name, method)

