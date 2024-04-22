
i = 0
def update():
    global i
    i += 1
    clear(43)
    x = sin(i * 0.071234) * 50 + 100
    y = sin(i * 0.0236) * 20 + 80
    circle(x + sin(i * 0.05) * 30, y + cos(i * 0.05) * 30, 5, 15)
    circle(x + sin(i * 0.05) * 30, y + cos(i * 0.05) * 30, 3, 11)
    circle(x + -sin(i * 0.05) * 30, y + -cos(i * 0.05) * 30, 5, 17)
    circle(x + -sin(i * 0.05) * 30, y + -cos(i * 0.05) * 30, 3, 19)



clear(44)
circle(20,30, 5, 17)
