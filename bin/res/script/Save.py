#-*- coding: utf-8 -*-

import pickle
import zlib
import traceback
from GData import GData

ARCHIVE_FILE = "save.dat"

def save() :
	data = GData.player.save()
	data = pickle.dumps(data, -1)
	data = zlib.compress(data)
	print("-----save", type(data), data)
	
	try:
		file = open(ARCHIVE_FILE, "wb")
	except IOError :
		print(traceback.print_exc())
		return False
		
	ret = True
	try:
		file.write(data)
	except :
		print(traceback.print_exc())
		ret = False
		
	file.close()
	return ret

def _readData() :
	data = ""
	
	try:
		file = open(ARCHIVE_FILE, "rb")
	except IOError :
		print(traceback.print_exc())
		return data
	
	try:
		data = file.read(-1)
	except:
		print("ERROR load faild!")
		
	file.close()
		
	return data

def load(reset=False):
	data = ""
	
	if not reset:
		data = _readData()
	
	print("-----load", type(data), data)
	data = str(data)
	
	if data :
		data = zlib.decompress(data)
		data = pickle.loads(data)
	else:
		data = {}
	
	GData.player.load(data)
	return True
