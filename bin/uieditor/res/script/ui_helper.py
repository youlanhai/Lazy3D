#-*- coding: utf-8 -*-


def ctlGetDescName(ctl):
	if ctl is None: return "无"

	return ctl.name + "(%s)" % (ctlGetClass(ctl), )

def ctlGetClass(ctl):
	return str(ctl.__class__)

def ctlFindNameInDict(ctl, dic):
	if not dic: return None
	
	for k, v in dic.items():
		if v == ctl: return k
	
	return None
