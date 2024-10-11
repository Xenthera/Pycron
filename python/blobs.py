from scene import Scene

# Palette class (inherits from scene)
class Blobs(Scene):

    def __init__(self):
        super().__init__()
        self.title = "Squares"
        self.t = 0
        self.dir = True

    def update(self):
        pass

    def draw(self):
        clear(0)

        for i in range(128):
            for j in range(128):
                c = cos(i / 10) + (sin(j / 10) + 2)
                #c2 = cos(i / 5) + sin(j / 5)
                pixel(i, j, 2 * c + self.t)

        self.t += (1/5) * (1 if self.dir else - 1)

        if(self.t > 64):
            self.t = 64
            self.dir = not self.dir

        if(self.t < 0):
            self.t = 0
            self.dir = not self.dir






