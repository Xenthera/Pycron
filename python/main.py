from scene import Scene
from particles import Particles
from triangles import Triangles

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
                text(str(j * 8 + i), 11 + (i * 20), 10 + (j * 20), 63)


scenes = [Particles(), Triangles(), Palette()]
current_scene = 0



KEY_T = 84

switch = False

def update():
    global switch
    global current_scene

    if(keyp(KEY_T)):
        current_scene += 1
        if(current_scene >= len(scenes)):
            current_scene = 0

    if(mousep(1)):
        scenes[current_scene].paused = not scenes[current_scene].paused

    if(not scenes[current_scene].paused):
        scenes[current_scene].update()
    scenes[current_scene].draw()    

    text(scenes[current_scene].title, 2, 15, 63)

    




