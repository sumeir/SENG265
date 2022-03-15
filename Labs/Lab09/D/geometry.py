import math

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

class Circle:

    def __init__(self, center, radius):
        self.center = center
        self.radius = radius

    def __repr__(self):
        return "Circle(%r, %r)" % (self.center, self.radius)
    
    def perimeter(self):
        return 2 * math.pi * self.radius
    
    def area(self):
        return math.pi * (self.radius)**2
    
    def translate(self, dx, dy):
        return Circle(self.center.translate(dx, dy), self.radius)

class Rectangle:

    def __init__(self, upper_left, lower_right):
        self.upper_left = upper_left
        self.lower_right = lower_right

    def __repr__(self):
        return "Rectangle(%r, %r)" % (self.upper_left, self.lower_right)
    
    def translate(self, dx, dy):
        return Rectangle(Point(self.upper_left.x+dx, self.upper_left.y+dy), Point(self.lower_right.x+dx, self.lower_right.y+dy))

    def area(self):
        return abs(self.upper_left.x - self.lower_right.x) * abs(self.upper_left.y - self.lower_right.y)
    
    def perimeter(self):
        return 2 * (abs(self.upper_left.x - self.lower_right.x) + abs(self.upper_left.y - self.lower_right.y))