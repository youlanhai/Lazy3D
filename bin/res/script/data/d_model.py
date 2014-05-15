#-*- coding: utf-8 -*-
#模型表

RT_MESH = 0
RT_SKINMESH = 1

def exist(no) : return no in sheet
def getPath(no) : return sheet[no]["path"]
def getType(no) : return sheet[no]["type"]
def getScale(no) : return sheet[no].get("scale", 0.025)
def getYaw(no) : return sheet[no].get("yaw", 0)
def getPitch(no) : return sheet[no].get("pitch", 0)
def getRoll(no) : return sheet[no].get("roll", 0)

#玩家从1开始。其他：从1001开始
#注意在d_action表中添加动作。
sheet = {
	1: {"path":"model/jingtian/jingtian.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":3.14},
	1001: {"path":"model/法师.x", "type":RT_SKINMESH, "scale" : 0.025,"yaw":-1.57 },
	1002: {"path":"model/传送门.X", "type":RT_MESH, "scale" : 0.025,"yaw":0.0 },
	1003: {"path":"model/箭头.x", "type":RT_MESH, "scale":5.0, "yaw":0.0, "pitch":-1.57},
	1004: {"path":"model/恶魔猎手.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1005: {"path":"model/弓箭手.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1006: {"path":"model/狼.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1007: {"path":"model/农民.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1008: {"path":"model/兽王.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1009: {"path":"model/巫妖.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1010: {"path":"model/小强.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1011: {"path":"model/月女.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1012: {"path":"model/憎恶.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1013: {"path":"model/蜘蛛.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
	1014: {"path":"model/小强.x", "type":RT_SKINMESH, "scale" : 0.025, "yaw":-1.57},
}
