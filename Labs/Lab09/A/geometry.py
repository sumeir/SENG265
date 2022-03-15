class Point:

    def __init__(self, *args):
        if len(args) == 2:
            self.x = args[0]
            self.y = args[1]
        elif len(args) == 1:
            self.x = args[0]
            self.y = 0
        else:
            self.x = 0
            self.y = 0
    
    def __repr__(self):
        return "Point(%r, %r)" % (self.x, self.y)
