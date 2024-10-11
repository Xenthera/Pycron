from scene import Scene

# Palette class (inherits from scene)
class Squares(Scene):

    def __init__(self):
        super().__init__()
        self.title = "Squares"

    def update(self):
        pass

    def draw(self):
        clear(0)

        for i in range(50):
            for j in range(27):
                rect(8 * i, 8 * j, 8, 8, (i + j) % 64)
                rectBorder(8 * i, 8 * j, 8, 8, (i + j + 1) % 64)






