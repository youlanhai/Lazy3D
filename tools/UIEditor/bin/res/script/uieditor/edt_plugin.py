# -*- coding: utf-8

from . import edt_const
from uieditor import edt_property


class TreeItemConfig(edt_property._stringItem):
	def getName(self): return "item config"
	def getCtlValue(self): return self.control.getItemConfig()
	def setCtlValue(self, v): return self.control.setItemConfig(v)

class TreeItemScript(edt_property._stringItem):
	def getName(self): return "item script"
	def getCtlValue(self): return self.control.getItemScript()
	def setCtlValue(self, v): return self.control.setItemScript(v)


CUSTEOM_TP_MAP = {
	edt_const.TP_TREE_ITEM_SCRIPT: TreeItemScript,
	edt_const.TP_TREE_ITEM_CONFIG: TreeItemConfig,
}


def boot():
	for k, v in CUSTEOM_TP_MAP:
		edt_property.registerTpMethod(k, v)
