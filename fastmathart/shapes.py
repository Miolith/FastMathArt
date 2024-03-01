from ctypes import c_float, Structure, POINTER, pointer, c_int
from fastmathart.const import *
from fastmathart.properties import Properties
import math

ShapeType = {
    "NO_TYPE": 0,
    "CIRCLE": 1,
    "RECTANGLE": 2,
    "POLYLINES": 3
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

class Polylines(Structure):
    _fields_ = [
        ("x", POINTER(c_float)),
        ("y", POINTER(c_float)),
        ("n", c_int),
        ("properties", POINTER(Properties))
    ]

    def __init__(
        self,
        x: list,
        y: list,
        properties: Properties = None
    ):
        if len(x) != len(y):
            raise ValueError("x and y must have the same length")
        
        self.x = (c_float * len(x))(*x)
        self.y = (c_float * len(y))(*y)
        self.n = len(x)
        if properties is not None:
            self.properties = pointer(properties)

def Polygon(x: list, y: list, properties: Properties = None):
    x.append(x[0])
    y.append(y[0])
    return Polylines(x, y, properties)


def Line(x: list, y: list, properties: Properties = None):
    return Polylines(x[:2], y[:2], properties)


def RegularPolygon(radius: float, sides: int, properties: Properties = None):
    x_points = []
    y_points = []
    for i in range(sides):
        x_points.append(radius * math.cos(2 * PI * i / sides))
        y_points.append(radius * math.sin(2 * PI * i / sides))
    return Polygon(x_points, y_points, properties)


def Triangle(radius: float, properties: Properties = None):
    return RegularPolygon(radius, 3, properties)


def Pentagon(radius: float, properties: Properties = None):
    return RegularPolygon(radius, 5, properties)


def Hexagon(radius: float, properties: Properties = None):
    return RegularPolygon(radius, 6, properties)


def Star(radius: float, spikes: int, properties: Properties = None):
    x_points = []
    y_points = []
    for i in range(spikes * 2):
        angle = PI * i / spikes
        r = radius if i % 2 == 0 else radius / 2
        x_points.append(r * math.cos(angle))
        y_points.append(r * math.sin(angle))
    return Polygon(x_points, y_points, properties)

def Cross(radius: float, properties: Properties = None):
    x_points = [radius, -radius, 0, radius, -radius]
    y_points = [radius, -radius, 0, -radius, radius]
    return Polygon(x_points, y_points, properties)

