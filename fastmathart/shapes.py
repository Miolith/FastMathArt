from ctypes import c_float, Structure, POINTER, pointer
from fastmathart.const import *
from fastmathart.properties import Properties

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
                 
