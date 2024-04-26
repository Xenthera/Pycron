class Ball:
    def __init__(self):
        self.x = rnd(10, 100)
        self.y = rnd(10, 100)
        self.velX = rnd(-5,5)
        self.velY = rnd(-5,5)
        self.color = rnd(0,64)

    def draw(self):

        self.x += self.velX;
        self.y += self.velY;

        if self.x < 0:
            self.velX = -self.velX;
            self.x = 0
            linecol = rnd(0,64)
        if self.y < 0:
            self.velY = -self.velY;
            self.y = 0
            linecol = rnd(0,64)

        if self.x > 360:
            self.velX = -self.velX;
            self.x = 360
            linecol = rnd(0,64)

        if self.y > 203:
            self.velY = -self.velY;
            self.y = 203
            linecol = rnd(0,64)


        circle(self.x, self.y, 3, self.color)
