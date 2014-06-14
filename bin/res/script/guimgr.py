#-*- coding: utf-8 -*-

import lui
import ui
import vk
import gui

from uieditor import editor
import edt_plugin
##################################################
###
##################################################
class guimgr(object):
	
	def __init__(self):
		super(guimgr, self).__init__()

		edt_plugin.boot()

		#self.form = MyForm(1, ui.uiRoot)
		self.editor = editor.Editor()
		
	def onKeyEvent(self, isDown, key):

		if not isDown:
			if key == vk.F2:
				ui.editorRoot.visible = not ui.editorRoot.visible
				return True

		if self.editor.visible and self.editor.onKeyEvent(isDown, key):
			return True
		
		return False

##################################################
###测试窗口
##################################################
class MyForm(lui.Form) :
	def __init__(self, id, parent, x=0, y=0, w=212, h=295) :
		super(MyForm, self).__init__(parent)
		
		self.id = id
		self.position = (x, y)
		self.size = (w, h)
		
		self.image = "gui/goods.bmp"
		self.bgColor = (0xffffffff)
#		self.setClipRect(0, 0, 100, 100)
		
		if False:
			self.loadFromFile("layout/layout.lzd")
		
		else:
			self.title = lui.Label()
			self.title.text = ("你好哈哈\naaaaaaaaaaaa")
			self.title.align = ui.align_mutiLine
			self.title.position = (15, 203)
			self.title.size = (180, 60)
			self.title.editable = True
			self.addChild(self.title)
			
			self.btn = lui.Button()
			self.btn.id = 2
			self.btn.position = (52, 263)
			self.btn.size = (100, 30)
			self.btn.text = ("确定")
			self.btn.editable = True
			self.btn.onButtonClick = gui.MethodProxy(self, "onBtnOK")
			self.addChild(self.btn)

			self.saveToFile("layout/layout.lzd")
	
	def __del__(self):
		print("info: MyForm del", self.id)

	def onBtnOK(self):
		print("button click")


class TestForm(lui.Form):

	def __init__(self):
		super(TestForm, self).__init__()

		print("TestForm init.")