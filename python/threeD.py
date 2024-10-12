from scene import Scene
from triangles import draw_line
import math

# Palette class (inherits from scene)
class ThreeD(Scene):


    def __init__(self):
        super().__init__()
        self.title = "Squares"
        self.t = 0

        self.x = 0.01
        self.y = 0
        self.z = 0

        self.a = 10
        self.b = 28
        self.c = 8.0/3.0

        self.scale = 5

        self.focal_length = 800
        self.screen_width = width * 4
        self.screen_height = height * 4
        self.lines = []

        self.points = []

        self.gradient = [4, 8, 6, 15, 18, 19, 26, 25, 24, 23, 16, 23, 22, 7, 5]

    def update(self):
        self.t += 0.00002

        
        if(len(self.points) > 1200):
            return
        
        dx = (self.a * (self.y - self.x)) * self.t
        dy = (self.x * (self.b - self.z) - self.y) * self.t
        dz = (self.x * self.y - self.c * self.z) * self.t
        self.x += dx
        self.y += dy
        self.z += dz

        self.points.append((self.x, self.y, self.z))

    def line3D(self,  x1, y1, z1, x2, y2, z2, c):
        self.lines.append(((x1, y1,  z1), (x2, y2, z2), c))


    def draw(self):
       
        self.lines.clear()
        clear(0)
        for i in range(len(self.points)):
            size = len(self.points)

            if(i < size - 1):
                x, y, z = self.points[i]
                x2, y2, z2 = self.points[i + 1]

                x *= self.scale
                y *= self.scale
                z *= self.scale
                x2 *= self.scale
                y2 *= self.scale
                z2 *= self.scale

                self.line3D(x, y, z, x2, y2, z2, self.gradient[int(i / 5) % len(self.gradient)])

        self.render_scene(self.focal_length, self.screen_width, self.screen_height, 0, self.t*500, self.t * 200, 0, 0, 300)


    def rotate_point(self, x, y, z, angle_x, angle_y, angle_z):
        """Rotates a point around the X, Y, and Z axes."""
        # Rotation around the X axis
        cos_x, sin_x = math.cos(angle_x), math.sin(angle_x)
        y, z = y * cos_x - z * sin_x, y * sin_x + z * cos_x

        # Rotation around the Y axis
        cos_y, sin_y = math.cos(angle_y), math.sin(angle_y)
        x, z = x * cos_y + z * sin_y, -x * sin_y + z * cos_y

        # Rotation around the Z axis
        cos_z, sin_z = math.cos(angle_z), math.sin(angle_z)
        x, y = x * cos_z - y * sin_z, x * sin_z + y * cos_z

        return x, y, z

    def project_point(self, x, y, z, focal_length, screen_width, screen_height, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z):

        x, y, z = self.rotate_point(x, y, z, angle_x, angle_y, angle_z)

        # Apply translation
        x += translate_x
        y += translate_y
        z += translate_z

        """Projects a 3D point onto a 2D screen using perspective projection."""
        if z == 0:
            z = 0.001  # Avoid division by zero

        # Perspective projection formula
        screen_x = (x * focal_length) / z
        screen_y = (y * focal_length) / z

        # Translate to screen coordinates (centered)
        screen_x += screen_width / 2
        screen_y += screen_height / 2

        return screen_x, screen_y

    def z_sort_lines(self, lines):
        """Sorts lines based on their average Z values for depth ordering."""
        return sorted(lines, key=lambda line: (line[0][2] + line[1][2]) / 2, reverse=True)

    def draw_3d_line(self, x1, y1, z1, x2, y2, z2, focal_length, screen_width, screen_height, color, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z):
        """Draws a 3D line projected onto a 2D screen."""
        # Project the endpoints
        screen_x1, screen_y1 = self.project_point(x1, y1, z1, focal_length, screen_width, screen_height, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z)
        screen_x2, screen_y2 = self.project_point(x2, y2, z2, focal_length, screen_width, screen_height, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z)

        # Use the renderer to draw the line on the screen
        draw_line(int(screen_x1) / 4, int(screen_y1) / 4, int(screen_x2) / 4, int(screen_y2) / 4, color)

    def render_scene(self, focal_length, screen_width, screen_height, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z):
        """Renders the 3D lines in the scene with perspective projection and Z-sorting."""
        # Sort the lines by Z value (depth)
        sorted_lines = self.z_sort_lines(self.lines)
        i = 0
        # Draw each line
        for line in sorted_lines:
            (x1, y1, z1), (x2, y2, z2), c = line
            self.draw_3d_line(x1, y1, z1, x2, y2, z2, focal_length, screen_width, screen_height, c, angle_x, angle_y, angle_z,  translate_x, translate_y, translate_z)
            i += 1

# Example usage
# renderer is an object that has a draw_line(x1, y1, x2, y2) function
liines = [
    ((-50, -50, -50), (50, -50, -50)),
    ((50, -50, -50), (50, 50, -50)),
    ((50, 50, -50), (-50, 50, -50)),
    ((-50, 50, -50), (-50, -50, -50)),
    ((-50, -50, 50), (50, -50, 50)),
    ((50, -50, 50), (50, 50, 50)),
    ((50, 50, 50), (-50, 50, 50)),
    ((-50, 50, 50), (-50, -50, 50)),
    ((-50, -50, -50), (-50, -50, 50)),
    ((50, -50, -50), (50, -50, 50)),
    ((50, 50, -50), (50, 50, 50)),
    ((-50, 50, -50), (-50, 50, 50)),
]



# Assuming you have a renderer object

