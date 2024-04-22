linecol = 15
def draw_line(x0, y0, x1, y1, c):
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy
    
    while x0 != x1 or y0 != y1:
        pixel(x0, y0, c)
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x0 += sx
        if e2 < dx:
            err += dx
            y0 += sy


class ball:
    def __init__(self):
        self.x = rnd(10, 100)
        self.y = rnd(10, 100)
        self.velX = rnd(-5,5)
        self.velY = rnd(-5,5)
        self.color = rnd(0,64)

    def draw(self):
        global linecol
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

# i = 0
# def update():
    # global i
    # i += 1
    # #clear(43)
    # x = sin(i * 0.071234) * 50 + (360 / 2)
    # y = sin(i * 0.0236) * 20 + (203 / 2)
    # circle(x + sin(i * 0.05) * 30, y + cos(i * 0.05) * 30, 5, 15)
    # circle(x + sin(i * 0.05) * 30, y + cos(i * 0.05) * 30, 3, 11)
    # circle(x + -sin(i * 0.05) * 30, y + -cos(i * 0.05) * 30, 5, 17)
    # circle(x + -sin(i * 0.05) * 30, y + -cos(i * 0.05) * 30, 3, 19)


balls = []

for i in range(2):
    balls.append(ball())

counter = 0

def update():
    global linecol
    global counter
    counter += 1
    #clear(0)
    draw_line(balls[0].x, balls[0].y, balls[1].x, balls[1].y, linecol)

    for ball in balls:
        ball.draw()
    
    for i in range(64):
        for j in range(20):
            if(i == 0 or i == 63 or j == 0 or j == 20):
                pixel(i * 4, j, i)
                pixel(i * 4 + 1, j, i)    
                pixel(i * 4 + 2, j, i)    
                pixel(i * 4 + 3, j, i)    
            else:
                pixel(i * 4, j, 0)
                pixel(i * 4 + 1, j, 0)    
                pixel(i * 4 + 2, j, 0)    
                pixel(i * 4 + 3, j, 0)    
    t = "Hello from python FPS:" + str(fps()) + " X:" + str(mouseX) + " Y:" + str(mouseY)
    for i in range(len(t)):
        text(t[i], 4 + (i * 7), 4, 20 + i)
    
    circle(mouseX, mouseY, 10, counter % 64)


