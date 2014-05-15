#-*- coding: utf-8 -*-

import sys
import os
import shutil
import traceback

##################################################
#提取模块路径信息
##################################################

#获得程序中所有模块的路径
def getModulesPath() :
	lst = []
	#sys.modules是一个字典，数据格式如下：
	#{'site': <module 'site' from 'D:\Python27\lib\site.pyc'>,
	for v in sys.modules.values() :
		s = str(v)
		if "from" in s: #有些模块内置在了解释器里，不用提取。
			data = s.split("'")
			path = data[-2]
			if path[-3:] == ".py" : #将py文件换成pyc
				path += "c"
			lst.append(path)
		else :
			pass
			#print "module : ", s
	return lst


#保存路径到py文件中
def savePaths(fname, paths) :
	try :
		fobj = open(fname, "w")
	except IOError :
		print("创建文件%s失败" % fname)
		return
	
	fobj.write("#-*- coding: utf-8 -*-\n")
	
	try :
		saveTuplePath(fobj, "syspath", sys.path)
		saveTuplePath(fobj, "paths", paths)
	except:
		print(traceback.print_exc())
	
	fobj.close()
	print("---保存路径完毕")

#将一个tuple保存到py文件
def saveTuplePath(fobj, tag, tupleData) :
	fobj.write("%s = (\n" % tag)

	for s in tupleData :
		s = s.replace("\\", "/")
		fobj.write("\t'%s',\n" % s)

	fobj.write(")\n\n")

##################################################
#将所有的py拷贝到一个目录中去
##################################################

#从给定文件列表中，抽取出文件
def extractFiles(destDir, files, syspath) :
	if len(destDir) == 0: destDir = "./"

	destDir = destDir.replace("\\", "/")
	if destDir[-1] != '/' : destDir += '/'
		
	errors = []
	for f in files :
		dest = filiterPath(destDir, f, syspath)
		if not copyF(dest, f) :
			errors.append(f)

	print("----------------------")
	print("error: ", errors)
	print("copyed ", len(files), " failed ", len(errors))

#过滤路径 去掉最大绝对路径
def filiterPath(destDir, srcFile, syspath) :
	dest = destDir
	maxLen = 0
	for path in syspath :
		lenp = len(path)
		if lenp < len(srcFile) and path == srcFile[:lenp] :
			if maxLen < lenp :
				dest = destDir + srcFile[lenp+1:]
				maxLen = lenp
			
	dest = dest.replace("\\", "/")
	if '.' in dest : #去掉文件名，仅保留目录
		p = dest.rfind('/')
		if p >= 0 :
			dest = dest[:p]
	return dest

#拷贝文件
#如果目标路径不存在，则创建
def copyF(destDir, srcFile) :
	if not os.path.isfile(srcFile) :
		print("error : file %s not exist!" % srcFile)
		return False

	if not os.path.isdir(destDir) :
		os.mkdir(destDir)
		print("make dir:", destDir)

	try :
		shutil.copy2(srcFile, destDir)
		print("copy file : %s to %s" % (srcFile, destDir))
	except IOError:
		print("error : copy %s to %s faild" % (srcFile, destDir))
		return False

	return True

##################################################
#测试
##################################################
def test() :
	modules = getModulesPath()
	extractFiles("testpg\\", modules, sys.path)

	#打包成zip文件
	argv = ("-c", "python27.zip", "src", "testpg\\")

	import zipfile
	zipfile.main(argv)
