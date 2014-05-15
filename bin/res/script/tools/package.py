#-*- coding: utf-8 -*-

from . import packagepy
from . import packagepath
import zipfile

PYTHON_VESION = 27

DEST_ZIP_FILE = "package\\mygame\\python%d.zip" % PYTHON_VESION
DEST_PATH = "package\\python%d\\" % PYTHON_VESION

def pack() :
	packagepy.extractFiles(DEST_PATH, packagepath.paths, packagepath.syspath)

	print("\nstart compress...")

	argv = ("-c", DEST_ZIP_FILE, "src", DEST_PATH)
	zipfile.main(argv)

if __name__ == "__main__" :
	pack()
