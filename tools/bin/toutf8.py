# -*- coding: utf-8 -*-

import sys
import os

USAGE = """usage:
python toutf8.py path [-r]
  path    path can be a directory or a file.
  -r      replace eol in sub directory rescusively.
"""

UTF8_BOM = "\xef\xbb\xbf"

FILE_IGNORE = frozenset((
	".git",
	".svn",
	"debug",
	"release",
))
FILE_EXTS = frozenset( (
	".h",
	".hpp",
	".ipp",
	".cpp",
	".c",
	".inl",
	".py",
))

def get_file_ext(path):
	if len(path) == 0: return path

	pos = path.find(".")
	if pos < 0: return ""
	return path[ pos: ]

def usage(): print USAGE

def replace_in_file(fpath):
	handle = open(fpath, "rb")
	source = handle.read()
	handle.close()

	existBom = len(source) >= 3 and source[:3] == UTF8_BOM

	content = None
	while 1:
		try:
			content = source.decode("utf8")
			break
		except:
			pass

		try:
			content = source.decode("gbk") #Chinese
			break
		except:
			pass

		try:
			content = source.decode("cp1251") #Russia
			break
		except:
			pass
			
		print "error", fpath
		return False

	content = content.encode("utf8")
	if existBom and content == source:
		return False
	
	print "replaced: ", fpath

	handle = open(fpath, "wb")
	if not existBom: handle.write(UTF8_BOM)
	handle.write(content)
	handle.close()

	return True

def replace_in_path(path, rescu):
	path = path.replace('\\', '/')
	if path[-1] != '/': path += '/'

	total = 0
	replaced = 0

	files = os.listdir(path)
	for fname in files:
		if fname.lower() in FILE_IGNORE: continue

		fname_ext = get_file_ext(fname)
		fname = path + fname
		#print"wwwww", fname, rescu, os.path.isdir(fname)
		
		if rescu and os.path.isdir(fname):
			t, r = replace_in_path(fname, rescu)
			total += t
			replaced += r

		elif fname_ext.lower() in FILE_EXTS:
			if replace_in_file(fname): replaced += 1
			total += 1
	return (total, replaced)

def main():

	num_arg = len(sys.argv)

	if num_arg < 2: return usage()

	path = None
	rescu = False

	i = 0
	while i < num_arg:
		cmd = sys.argv[i]
		if cmd == "-r":
			rescu = True

		elif cmd[0] == "-":
			print "invalid cmd ", cmd
			return usage()

		else:
			path = cmd

		i += 1


	if path is None or not os.path.exists(path):
		print "invalid path"
		return usage()

	total, replaced = replace_in_path(path, rescu)
	print "\nPath: ", path
	print "Total file %d, Replaced %d" % (total, replaced)

	return

if __name__ == "__main__":
	main()

