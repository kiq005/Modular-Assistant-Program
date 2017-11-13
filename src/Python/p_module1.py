def input(inp):
	inp = inp.lower();
	if inp == "is python there?":
		return "$CONF$Do you really wanna know?"
	return ""

def respond(inp):
	if inp == "yes":
		return "Yep! Python is here!"
	return ""

def destroy():
	print "Goodbye from Python!"
