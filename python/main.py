#  import ball

# linecol = 15
# def draw_line(x0, y0, x1, y1, c):
#     dx = abs(x1 - x0)
#     dy = abs(y1 - y0)
#     sx = 1 if x0 < x1 else -1
#     sy = 1 if y0 < y1 else -1
#     err = dx - dy

#     while x0 != x1 or y0 != y1:
#         pixel(x0, y0, c)
#         e2 = 2 * err
#         if e2 > -dy:
#             err -= dy
#             x0 += sx
#         if e2 < dx:
#             err += dx
#             y0 += sy


# balls = []

# for i in range(2):
#     balls.append(ball.Ball())

# counter = 0

# def update():
#     global linecol
#     global counter
#     counter += 1
#     #clear(0)
#     draw_line(balls[0].x, balls[0].y, balls[1].x, balls[1].y, linecol)

#     for ball in balls:
#         ball.draw()

#     for i in range(64):
#         for j in range(20):
#             pixel(i * 4, j, i)
#             pixel(i * 4 + 1, j, i)
#             pixel(i * 4 + 2, j, i)
#             pixel(i * 4 + 3, j, i)

#     t = "Hello from python FPS:" + str(fps()) + " X:" + str(mouseX) + " Y:" + str(mouseY)
#     for i in range(len(t)):
#         text(t[i], 4 + (i * 7), 4, 20 + i)

#     circle(mouseX, mouseY, 10, counter % 64)


# clear(0)
# text("test", 2,2, 32)
import easing
import time
funcs = [easing.InSine, easing.OutSine, easing.InOutSine, easing.InQuad, easing.OutQuad, easing.InOutQuad, easing.InBounce, easing.OutBounce, easing.InOutBounce, easing.InElastic, easing.OutElastic, easing.InOutElastic]

test = {"1" : 0, "2" : 4}
counter = 0
x = 0
def update():
    cls()
    global x
    global counter
    counter += 0.008
    x += 0.5
    if(counter > 1):
        counter = 0
    
    for i in range(len(funcs)):
        pos = funcs[i](counter)
        circle(100 + (pos * 200), 10 + (14 * i), 5, i + 35)

    text("The FPS: " + str(fps()), 2, 2, 9)
    

def cls():
    for i in range(width):
        for j in range(height):
            pixel(i , j, int(i + j + x) % 64)
    