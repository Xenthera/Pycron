from scene import Scene

cols = [(6,8),(16,17),(31,30),(5,4),(41,40),(51,50)]

def draw_line(x0, y0, x1, y1, c):
    x0 = int(x0)
    y0 = int(y0)
    x1 = int(x1)
    y1 = int(y1)

    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    x, y = x0, y0
    sx = -1 if x0 > x1 else 1
    sy = -1 if y0 > y1 else 1
    if dx > dy:
        err = dx / 2.0
        while x != x1:
            pixel(x, y, c)
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
    else:
        err = dy / 2.0
        while y != y1:
            pixel(x, y, c)
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy        
    pixel(x, y, c)

def lineTri(x1,y1,x2,y2,x3,y3,c):
    draw_line(x1,y1,x2,y2,c)
    draw_line(x2,y2,x3,y3,c)
    draw_line(x1,y1,x3,y3,c)



class spinTri():
    def __init__(self):
        self.x = rnd(0,width)
        self.y = 240
        self.r = 4 + rnd(0, 4)
        self.t = rnd(0, 1)
        self.a = rnd(0, 1)
        self.vy = -0.5-rnd(0,1)
        self.c = int(rnd(0, len(cols)))
        self.colidx = 0
        self.count = rnd(0,50)


class Triangles(Scene):
    def __init__(self):
        super().__init__()
        self.p = []
        self.t = 0.0
        self.count = 0
        self.tpo3 = (2 * 3.141592654)/3
        self.skip = False
        self.title = "Flying Triangles"



    def draw(self):
        self.skip = not self.skip
        if(self.skip):
            return
        self.t += 0.01
        self.count += 1

        for i in range(999):
            x = rnd(0, width)
            y = rnd(0, height)
            c = 0
            if(rnd(0,9) < 5):
                c = get_pixel(x -3 ,y-3)
            else:
                c = 0

            circle(x, y, 1, c)

        for pt in self.p[:]:
            pt.y += pt.vy
            pt.t += 0.05
            pt.a += 0.1
            pt.count += 1
            
            r = (0.8 + 0.2 * cos(pt.t * 8)) * pt.r
            x1 = pt.x + r * cos(pt.a)
            y1 = pt.y + r * sin(pt.a)
            x2 = pt.x + r * cos(pt.a + self.tpo3)
            y2 = pt.y + r * sin(pt.a + self.tpo3)
            x3 = pt.x + r * cos(pt.a + self.tpo3 * 2)
            y3 = pt.y + r * sin(pt.a + self.tpo3 * 2)

            if(int(pt.count) % 5 == 0):
                pt.colidx += 1
                if(pt.colidx >= len(cols)):
                    pt.colidx = 0
            light, dark = cols[pt.colidx]
            triangle(x1, y1, x2, y2, x3, y3, light)
            lineTri(x1, y1, x2, y2, x3, y3, dark)

            if(pt.y < - 40):
                self.p.remove(pt)

        #if(self.count % 3 == 0):
        #for i in range(1):
        self.p.append(spinTri())
        # if(mousep(0)):
        #     self.p.append(spinTri())
        
        
        