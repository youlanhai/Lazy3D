# -*- coding: utf-8 -*-

def map2all_eno(mapno): return sheet_keys #test
def exist_eno(eno): return eno in sheet_keys

def get_data(eno): return sheet[eno]
def get_kno(eno): return sheet[eno]["kno"]
def get_name(eno) : return sheet[eno]["name"]
def get_sculpt(eno): return sheet[eno]["sculpt"]
def get_position(eno): return sheet[eno]["position"]

def all_kno(): return ALL_KNO
def exist_kno(kno): return kno in ALL_KNO
def kno2class(kno): return CLASS_MAP[kno]

K_ENTITY = 1
K_NPC    = 2
K_AVATAR = 3
K_TASKNPC = 5
K_TRANSPORT = 6
K_MONSTER = 100
K_BOSS    = 101

CLASS_MAP = {
	K_ENTITY : "Entity",
	K_NPC : "Npc",
	K_AVATAR : "Player",
	K_MONSTER : "Monster",
	K_TASKNPC : "TaskNpc",
	K_TRANSPORT : "Transport",
	K_BOSS : "Boss",
}

ALL_KNO = frozenset(list(CLASS_MAP.keys()))


#entity attribute
sheet = {
    1:{"name":"游蓝海", "sculpt": 1, "kno":K_AVATAR, "map": 1, 'position': ( 0, 0, 0), },
    1001:{"name":"法师", "sculpt":1001, "kno":K_NPC, "map": 1, 'position': ( -20, 0, 10), },
    1002:{"name":"传送阵", "sculpt":1002, "kno":K_TRANSPORT, "map": 1, 'position': ( -40, 0, 30), },
    1003:{"name":"鼠标", "sculpt":1003, "kno":K_ENTITY, "map": 1, 'position': ( -60, 0, 50), },
    1004:{"name":"恶魔法师", "sculpt": 1001, "kno":K_MONSTER,  "map": 1, 'position': ( -80, 0, 10), }
}

sheet_keys = frozenset(sheet)
