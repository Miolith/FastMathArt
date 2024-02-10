from ctypes import Structure, c_float, POINTER, c_void_p, pointer
from fastmathart.color import Color

class Properties(Structure):
    _fields_ = [
        # Position
        ("x", c_float),
        ("y", c_float),
        ("z", c_float),

        # Color
        ("color", POINTER(Color)),

        # Stroke thickness
        ("thickness", c_float),

        ("fill", POINTER(Color)),
        ("opacity", c_float),
    ]

    def __init__(
        self,
        position: tuple = (0.0, 0.0, 0.0),
        color: Color = None,
        thickness: float = 0.1,
        fill: Color = None,
        opacity: float = 1.0
    ):
        self.x = position[0]
        self.y = position[1]
        self.z = position[2]

        self.color = pointer(color)
        self.thickness = thickness
        if fill is not None:
            self.fill = pointer(fill)
        self.opacity = opacity