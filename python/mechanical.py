from scene import Scene

s = 128
s2 = 120-12
l = 15
a = 80
t = 0

# Palette class (inherits from scene)
class Mechanical(Scene):

    def __init__(self):
        super().__init__()
        self.title = "Squares"


    def update(self):
        pass

    def draw(self):
        clear(0)
        rect(120 - s/2, 68 - s/2, s, s, 14)
        triangle(120-s/2, 68 - s/2, 120-s/2, 68+s/2,
                 120 + s/2, 68+s/2, 15)
        line(119 + s / 2, 68 + s / 2, 119 + s2 / 2, 68 - s2/2, 15)




