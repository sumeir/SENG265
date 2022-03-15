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

    def delta_x(self, dx):
        return Point(self.x + dx, self.y)

    def delta_y(self, dy):
        return Point(self.x, self.y + dy)

    def translate(self, dx, dy):
        return Point(self.x + dx, self.y + dy)