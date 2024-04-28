from scene import Scene
from triangles import draw_line

white = 63
fire1 = 6
fire2 = 5
fire3 = 4
fire4 = 3
fire5 = 2
fire6 = 56

water1 = 31
water2 = 30
water3 = 29
water4 = 28

fireGradient = [white, white, fire1, fire1, fire2, fire3, fire4, fire3, fire4, fire5, fire6, fire6, fire6]
waterGradient = [white, white, water1, water2, water3, water4, water4]

oldMouseX = 0
oldMouseY = 0

mouseVelX = 0
mouseVelY = 0


def tri(x1,y1,x2,y2,x3,y3,c):
    draw_line(x1,y1,x2,y2,c)
    draw_line(x2,y2,x3,y3,c)
    draw_line(x1,y1,x3,y3,c)

class Particle:
    def __init__(self, x, y, velX, velY):
        self.x = x
        self.y = y
        self.velX = velX
        self.velY = velY
        self.c = fireGradient[0]
        self.size = 5

class Particles(Scene):

    def __init__(self):
        super().__init__()
        self.fireParts = []
        self.waterParts = []
        for i in range(10):
            self.fireParts.append(Particle(width/2, height/2, rnd(-1, 1), rnd(-1, 1)))
            self.waterParts.append(Particle(width/2, height/2, rnd(-1, 1), rnd(-1, 1)))

        self.counter = 0


    def update(self):
        global mouseVelX
        global mouseVelY

        global oldMouseX
        global oldMouseY

        mouseVelX = (mouseX - oldMouseX)
        mouseVelY = (mouseY - oldMouseY)

        oldMouseX = mouseX
        oldMouseY = mouseY

        self.counter += 1

        for p in self.fireParts[:]:
            p.x += p.velX
            p.y += p.velY
            p.velX += rnd(-0.3,0.3)
            p.velY += rnd(-0.3,0.3)

            idx = int(p.size / 5.0 * len(fireGradient) - 1)

            p.c = fireGradient[(len(fireGradient) - 1) - idx]

            if(p.size < 1):
                p.size -= 0.01
            else:
                p.size -= 0.05
            if(p.size < 0):
                self.fireParts.remove(p)

        for p in self.waterParts[:]:
            p.x += p.velX
            p.y += p.velY
            p.velX += rnd(-0.3,0.3)
            p.velY += rnd(-0.3,0.3)

            idx = int(p.size / 5.0 * len(waterGradient) - 1)

            p.c = waterGradient[(len(waterGradient) - 1) - idx]

            if(p.size < 1):
                p.size -= 0.01
            else:
                p.size -= 0.05
            if(p.size < 0):
                self.waterParts.remove(p)

        

        if(mouse(0)):
            rad = sin(self.counter * 0.01) * 50
            x1 = mouseX + (sin(self.counter * 0.1) * rad)
            y1 = mouseY + (cos(self.counter * 0.1) * rad)

            x2 = mouseX + (sin(self.counter * 0.1 + (3.141592653)) * rad)
            y2 = mouseY + (cos(self.counter * 0.1 + (3.141592653)) * rad)
            for i in range(10):
                self.fireParts.append(Particle(x1, y1, rnd(-1,1) + mouseVelX * 0.25, rnd(-1,1) + mouseVelY * 0.25))
                self.waterParts.append(Particle(x2, y2, rnd(-1,1) + mouseVelX * 0.25, rnd(-1,1) + mouseVelY * 0.25))

    def draw(self):
        clear(0)
        for p in self.fireParts:
            if(p.size <= 1):
                pixel(p.x, p.y, p.c)
            else:
                circle(p.x, p.y, int(p.size), p.c)

        for p in self.waterParts:
            if(p.size <= 1):
                pixel(p.x, p.y, p.c)
            else:
                circle(p.x, p.y, int(p.size), p.c)
        text("C to add particles, T to toggle palette", 3, 3, 59)
        text("C to add particles, T to toggle palette", 2, 2, 31)
        text("Num Particles: " + str(len(self.fireParts) + len(self.waterParts)), 3, 16, 59)
        text("Num Particles: " + str(len(self.fireParts) + len(self.waterParts)), 2, 15, 30)
        text("FPS: " + str(fps()), 3, 29, 59)
        text("FPS: " + str(fps()), 2, 28, 29)


