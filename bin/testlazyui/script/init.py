# -*- coding: utf-8 -*-

g_testForm = None

def doTest():
	import lui
	
	print("lui", dir(lui))
	print("Form", dir(lui.Form))
	
	global g_testForm
	g_testForm = doTestForm()

def doTestForm():
	import lui
	
	form = lui.Form()
	lui.root().addChild(form)
	
	form.x = 400
	form.y = 100
	form.width = 200
	form.height = 100
	form.bgColor = 0x8fff0000
	
	a = lui.Button()
	a.x = 100
	a.width = 50
	a.text = "close"
	a.color = 0xff00ff00
	form.addChild(a)
	form.a = a
	
	b = lui.Label()
	b.text = "abcd中文字体"
	b.color = 0xffff00ff
	form.addChild(b)
	form.b = b
	
	script = MsgHanlder()
	form.script = script
	a.script = script
	
	print("script:", a.script, form.b.script)
	
	return form

class MsgHanlder(object):

	def __init__(self):
		super(MsgHanlder, self).__init__()
		
	def onButtonClick(self, ctl, isDown):
		print("onButtonClick", ctl, isDown)
