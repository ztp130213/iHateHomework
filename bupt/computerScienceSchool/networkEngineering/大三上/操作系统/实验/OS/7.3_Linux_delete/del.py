import os, sys, getopt

def usage():
	print "There could be 3 models selective:"
	print "1.'-h'--help model. You could get the mean to use this python shell"
	print "2.'-f'--full path model. You need could type in the full path including the appedix and filename to del (e.g python del.py -f /root/1.txt)"
	print "3.'-a'--abbrave path model. I'll del the file in the current dir. (e.g python del.py -a 1.txt)"

def del_tree(top):
	for root, dirs, files in os.walk(top, topdown=False):
    		for name in files:
        		os.remove(os.path.join(root, name))
    		for name in dirs:
        		os.rmdir(os.path.join(root, name))
def del_file(path):
	os.remove(path)

def full_model(path):
	filepath = os.path.dirname(path)
	filename = os.path.basename(path)
	os.chdir(filepath)

	if os.path.isdir(path):
		print os.listdir(filepath)
		del_tree(path)
		print os.listdir(filepath)
	elif os.path.isfile(path):
		print os.listdir(path)
		del_file(path)
		print os.listdir(path)
	print "Congradulation"
def abbr_model(name):
	filename = name
	filepath = os.getcwd()
	path = filepath+'/'+filename
	
	if os.path.isdir(path):
		print os.listdir(filepath+'/')
		del_tree(path)
		os.rmdir(path)
		print os.listdir(filepath+'/')
	elif os.path.isfile(path):
		print os.listdir(filepath+'/')
		del_file(path)
		print os.listdir(filepath+'/')
	print "Congradulation"
def main():
	try:
		(opts, args) = getopt.getopt(sys.argv[1:], "a:f:h")
	except getopt.GetoptError, err:
		print str(err)
		usage()
		sys.exit(2)

	action = None

	for (o, a) in opts:
		if o == "-f":
			path = a
			action = "full"
		elif o == "-a":
			path = a
			action = "abbr"
		elif o == "-h":
			action = "help"
	
	if (action):
		if action == "full":
			full_model(path)
		elif action == "abbr":
			abbr_model(path)
		elif action == "help":
			usage()
	else:
		usage()

if __name__ == "__main__":
	main()
