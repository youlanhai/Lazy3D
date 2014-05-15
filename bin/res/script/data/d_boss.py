#-*- coding: utf-8 -*-

def getBossLevel(no) : return sheet[no]["level"]
def getTaskObj(no) : return sheet[no]["taskObj"]

sheet = {
	2001:{"level":2, "task":13, "taskObj":(100001,),},
	2002:{"level":100, "taskObj":(100001, 100002),},
}