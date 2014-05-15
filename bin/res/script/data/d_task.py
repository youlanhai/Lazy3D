#-*- coding: utf-8 -*-
#任务表


#npc，接任务NPC
#work，要做的事情。怪物编号:个数
#cond，接任务条件。必须完成规定的任务才可接
#talk，接任务对白。0-玩家说话，1-npc说话，1001-接受，1002-放弃
from data.dd_task import sheet as sheet

task_keys = frozenset( list(sheet.keys()) )

#npc，交任务npc
#talk，交任务对白。
from data.dd_task_submit import sheet as sheet_submit


#position，位置(地图编号，坐标x,y,z)
from data.dd_task_npc import sheet as sheet_npc

from data.dd_task_trigger import sheet as sheet_trigger



def isExist(no): return no in sheet
def taskKeys(): return task_keys
def getTaskName(no) : return sheet[no]["name"]
def getTaskNpc(no) : return sheet[no]["npc"]
def getTaskTalk(no) : return sheet[no]["talk"]
def getCondition(no) : return sheet[no].get("cond", ())
def getTaskWork(no) : return sheet[no].get("work", {})
def getTaskObj(no) : return sheet[no].get("taskObj", ())
def getTaskDesc(no): return sheet[no].get("describe", "无描述")
def isNeedSubmit(no) : return getSubmitNpc(no)

def getSubmitNpc(no): 
	v = sheet_submit.get(no, None)
	if not v :
		return None
	return v["npc"]
def getSubmitTalk(no): return sheet_submit[no]["talk"]

def isExistNpc(no): return no in sheet_npc
def getNpcName(no) : return sheet_npc[no]["name"]
def getNpcModel(no) : return sheet_npc[no]["model"]
def getNpcTalk(no) : return sheet_npc[no]["talk"]
def getNpcPosition(no) : return sheet_npc[no]["position"]
def getNpcMove(no) : return sheet_npc[no].get("move", 0)

def canNpcShow(no, player) :
	tno = sheet_npc[no].get("notshow", 0)
	if tno and player.isTaskAccepted(tno) :
		return False
	
	tno = sheet_npc[no].get("notshow2", 0)
	if tno and player.isTaskCompeted(tno) :
		return False
	
	tno = sheet_npc[no].get("canshow", 0)
	if not tno: return True
	
	return player.isTaskCompeted(tno)

#是否是boss
def isBoss(no) : return no > 8000

#获得一个npc的任务
def getNpcTasks(no) :
	tasks = []
	for k, v in sheet.items() :
		try :
			if v["npc"] == no :
				tasks.append(k)
		except KeyError:
			print(("traceback, getNpcTasks KeyError", k, v))
	return tasks
	
#获得一个npc的完成任务
def getNpcSubmitTasks(no) :
	tasks = []
	for k, v in sheet_submit.items() :
		if v["npc"] == no :
			tasks.append(k)
	return tasks
	
#获得一张地图上的npc
def getMapNpc(no) :
	npc = []
	for k, v in sheet_npc.items() :
		if no == v["position"][0] :
			npc.append(k)
	return npc
	
##################################################
#触发器相关
##################################################
TRIGGER_ACCEPT   = 1
TRIGGER_COMPETE  = 2
TRIGGER_DEAD     = 3

#获得任务触发器
def getTaskTrigger(no):
	return sheet[no].get("trigger", ())

def getTriggerType(no) :
	return sheet_trigger[no]["type"]

def getTriggerNew(no) :
	return sheet_trigger[no].get("new", None)

def getTriggerDel(no) :
	return sheet_trigger[no].get("del", None)

def getTriggerMov(no) :
	return sheet_trigger[no].get("mov", None)
