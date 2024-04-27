from scene import Scene

fireGradient = [9, 9, 18, 18, 17, 16, 15, 14, 15, 14, 16, 14, 14,14,16, 2, 1]
waterGradient = [9, 9, 43, 43, 42, 41, 40, 39, 40, 39, 41, 39, 39,39,41, 2, 1]

class Particle:
    def __init__(self, x, y, velX, velY):
        self.x = x
        self.y = y
        self.velX = velX
        self.velY = velY
        self.c = fireGradient[0]
        self.size = 5

class Palette(Scene):

    def __init__(self):
        super().__init__()

    def update(self):
        pass

    def draw(self):
        clear(0)

        for i in range(8):
            for j in range(8):
                rectangle(10 + (i * 20), 10 + (j * 20), 18, 18, (j * 8 + i))
                text(str(j * 8 + i), 10 + (i * 20), 10 + (j * 20), 9)


class Main(Scene):

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

            idx = int(p.size / 5.0 * len(fireGradient) - 1)

            p.c = waterGradient[(len(waterGradient) - 1) - idx]

            if(p.size < 1):
                p.size -= 0.01
            else:
                p.size -= 0.05
            if(p.size < 0):
                self.waterParts.remove(p)

        if(key(67)):
            x1 = mouseX + (sin(self.counter * 0.1) * 60)
            y1 = mouseY + (cos(self.counter * 0.1) * 60)

            x2 = mouseX + (sin(self.counter * 0.1 + (3.141592653)) * 60)
            y2 = mouseY + (cos(self.counter * 0.1 + (3.141592653)) * 60)
            for i in range(2):
                self.fireParts.append(Particle(x1, y1, rnd(-1,1) + mouseVelX * 0.25, rnd(-1,1) + mouseVelY * 0.25))
                self.waterParts.append(Particle(x2, y2, rnd(-1,1) + mouseVelX * 0.25, rnd(-1,1) + mouseVelY * 0.25))

    def draw(self):
        clear(44)
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
        text("C to add particles, T to toggle palette", 3, 3, 0)
        text("C to add particles, T to toggle palette", 2, 2, 30)
        text("Num Particles: " + str(len(self.fireParts) + len(self.waterParts)), 3, 16, 0)
        text("Num Particles: " + str(len(self.fireParts) + len(self.waterParts)), 2, 15, 9)


paletteScene = Palette()
mainScene = Main()
current_scene = mainScene

oldMouseX = 0
oldMouseY = 0

mouseVelX = 0
mouseVelY = 0

KEY_T = 84

switch = False

def update():
    global switch
    global current_scene
    global mouseVelX
    global mouseVelY
    global oldMouseX
    global oldMouseY

    mouseVelX = (mouseX - oldMouseX)
    mouseVelY = (mouseY - oldMouseY)

    oldMouseX = mouseX
    oldMouseY = mouseY


    if(keyp(KEY_T)):
        switch = not switch
        if(switch):
            current_scene = paletteScene
        else:
            current_scene = mainScene

    current_scene.update()
    current_scene.draw()    

    circle(mouseX, mouseY, 3, 30)
