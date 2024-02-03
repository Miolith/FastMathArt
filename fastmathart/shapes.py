from fastmathart.const import *
from ctypes import c_float, Structure, cast, POINTER

class Circle(Structure):
    _fields_ = [
        ("radius", c_float),
        ("x", c_float),
        ("y", c_float),
        ("r", c_float),
        ("g", c_float),
        ("b", c_float),
        ("thickness", c_float),
        ("fill", POINTER(c_float))
    ]

    def __init__(
        self,
        radius: float = 1.0,
        color: tuple = WHITE,
        thickness: float = 0.1,
        position: tuple = (0.0, 0.0, 0.0),
        fill: tuple = None
    ):
        self.radius = radius
        self.x = 0.0
        self.y = 0.0
        self.r = color[0]
        self.g = color[1]
        self.b = color[2]
        self.thickness = thickness
        if fill:
            self.fill = (c_float * 3)(*fill)
        self._fill = fill
        self._color = color
        self._thickness = thickness
        self._radius = radius
                 
