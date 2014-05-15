#-*- coding: utf-8 -*-

def exist(no): return no in sheet
def getName(no) : return sheet[no]["name"]
def getLevel(no) : return sheet[no]["level"]
def getModel(no): return sheet[no]["model"]
def getPosition(no): return sheet[no]["position"]
def getTaskObj(no) : return sheet[no].get("taskObj", ())
def getFriendState(no) : return sheet[no].get("friend", 1)

def canShow(no, player) :
	tno = sheet[no].get("notshow", 0)
	if tno and player.isTaskCompeted(tno) :
		return False
	
	tno = sheet[no].get("canshow", 0)
	if not tno: return True
	
	return player.isTaskAccepted(tno)

def isBoss(no) : return no < 50000

def getMapBoss(mno) :
	lst = []
	for k, v in sheet.items() :
		if isBoss(k) and v["position"][0] == mno : lst.append(k)
	return lst

def getCenterPos(no) : return sheet_center[no]["position"]
def getCenterMonster(no) : return sheet_center[no]["monster"]
def getCenterAmount(no) : return sheet_center[no]["amount"]
def getCenterLevel(no) : return sheet_center[no]["level"]

def getMapCenter(mno) :
	lst = []
	for k, v in sheet_center.items() :
		if v["map"] == mno : lst.append(k)
	return lst

from .dd_monster import sheet as sheet
from .dd_monster_center import sheet as sheet_center
