#-*- coding: utf-8 -*-

DEFAULT_MUSIC = "res/music/desert.mp3"

def get_name(no): return sheet[no]["name"]
def get_desc(no): return sheet[no]["desc"]
def get_path(no) :
	name = get_name(no)
	return "map/%s/map.lzd" % name

def getMapMusic(no) : return sheet[no].get("music", DEFAULT_MUSIC)
def getMapSky(no) : return sheet[no]["sky"]
def getAmbintColor(no) : return sheet[no]["ambient"]
def getLightDir(no) : return sheet[no]["ldir"]
def getLightColor(no) : return sheet[no]["lcolor"]
def getFogColor(no) : return sheet[no]["fog"]

sheet = {
	1: {"name":"test", "dest":"登陆", "music":"res/music/desert.mp3", "sky":"res/gui/sky/凌晨/%d.jpg", "ambient":0xffcccccc, "ldir":(-1, -1, 0), "lcolor":(1.0, 0.8, 0.8, 0.8), "fog":0xffe0e0e0},
	1001: {"name":"kunqu","music":"res/music/happy.mp3","sky":"res/gui/sky/太阳初升/%d.jpg", "ambient":0xffffffff, "ldir":(-0.1, -0.2, -1), "lcolor":(1.0, 1.0, 1.0, 1.0), "fog":0xfff1f1c3},
	1002: {"name":"qingshan","music":"res/music/p38.mp3","sky":"res/gui/sky/太阳初升/%d.jpg", "ambient":0xff8d8d8d, "ldir":(0.4, -0.6, 0.7), "lcolor":(1.0, 0.87, 0.87, 0.5), "fog":0xffbababa},
	1003: {"name":"jiuyuan","music":"res/music/p43.mp3","sky":"res/gui/sky/多云/%d.jpg", "ambient":0xffafafaf, "ldir":(0, -1, 0), "lcolor":(1.0, 0.7, 0.7, 0.7), "fog":0xffa0abbb},
	1004: {"name":"donghe","music":"res/music/23.mp3","sky":"res/gui/sky/多云/%d.jpg", "ambient":0xffafafaf, "ldir":(0, -1, 0), "lcolor":(1.0, 0.7, 0.7, 0.7), "fog":0xffa0abbb},
	1005: {"name":"tumote","music":"res/music/p20.mp3","sky":"res/gui/sky/雨天/%d.jpg", "ambient":0xff8b8b8b, "ldir":(0, -1, 0), "lcolor":(1.0, 0.51, 0.51, 0.51), "fog":0xff161616},
	1006: {"name":"shiguai","music":"res/music/desert.mp3","sky":"res/gui/sky/雨天/%d.jpg", "ambient":0xff8b8b8b, "ldir":(0, -1, 0), "lcolor":(1.0, 0.51, 0.51, 0.51), "fog":0xff161616},
	1007: {"name":"daerhan","music":"res/music/p47.mp3","sky":"res/gui/sky/傍晚/%d.jpg", "ambient":0xff9e8766, "ldir":(0, -1, 0), "lcolor":(1.0, 0.84, 0.71, 0.61), "fog":0xffffaf49},
	1008: {"name":"guyang","music":"res/music/p04.mp3","sky":"res/gui/sky/傍晚/%d.jpg", "ambient":0xff6c6c6c, "ldir":(0, -1, 0), "lcolor":(1.0, 1.0, 0.9, 0.8), "fog":0xff383838},
	1009: {"name":"baiyunebo","music":"res/music/P39.mp3","sky":"res/gui/sky/太空/%d.jpg", "ambient":0xff1b1b1b, "ldir":(0.1, -0.2, -1), "lcolor":(1.0, 0.78, 0.78, 0.78), "fog":0xff0f0f0f},
}
