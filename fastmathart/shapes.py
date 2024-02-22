from ctypes import c_float, Structure, POINTER, pointer, c_int
from fastmathart.const import *
from fastmathart.properties import Properties

ShapeType = {
    "NO_TYPE": 0,
    "CIRCLE": 1,
    "RECTANGLE": 2
}

class Circle(Structure):
    _fields_ = [
        ("radius", c_float),
        ("properties", POINTER(Properties))
    ]

    def __init__(
        self,
        radius: float = 1.0,
        properties: Properties = None
    ):
        self.radius = radius
        if properties is not None:
            self.properties = pointer(properties)
                 

class Rectangle(Structure):
    _fields_ = [
        ("width", c_float),
        ("height", c_float),
        ("properties", POINTER(Properties))
    ]

    def __init__(
        self,
        width: float = 1.0,
        height: float = 1.0,
        properties: Properties = None
    ):
        self.width = width
        self.height = height
        if properties is not None:
            self.properties = pointer(properties)