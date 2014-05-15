#-*- coding: utf-8 -*-

FREE = 1
WALK = 2
ATTACK = 3
BE_HIT = 4
DEATH = 5

def exist(modelNo): return modelNo in sheet
def action(mno, ano) : return sheet.get(mno, {}).get(ano, "")
def free(mno): return action(mno, FREE)
def walk(mno): return action(mno, WALK)
def attack(mno): return action(mno, ATTACK)
def be_hit(mno): return action(mno, BE_HIT)
def death(mno): return action(mno, DEATH)

def needLoop(mno) :
	if mno == DEATH :
		return False
	return True

sheet = {
	1:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1001:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1002:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1003:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1004:{FREE:"stand2", WALK:"walk", ATTACK:"attack2", DEATH:"death"},
	1005:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1006:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1007:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1008:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1009:{FREE:"spell", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1010:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1011:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1012:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1013:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1014:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
	1015:{FREE:"stand", WALK:"walk", ATTACK:"attack", DEATH:"death"},
};