from ctypes import c_float, Structure, cast, c_void_p, POINTER, c_int, pointer
from fastmathart.shapes import ShapeType

_basic_fields = [
    ("obj_list", POINTER(POINTER(c_void_p))),
    ("obj_types", POINTER(c_int)),
    ("obj_count", c_int)
]

def _basic_init(animation, *args):
    # Convert all args to void pointers
    object_list = [cast(pointer(arg), POINTER(c_void_p)) for arg in args]

    animation.obj_list = (POINTER(c_void_p) * len(object_list))(*object_list)

    type_num_list = [ShapeType.get(arg.__class__.__name__.upper(), 0) for arg in args]

    animation.obj_types = (c_int * len(args))(*type_num_list)
    animation.obj_count = len(args)

class Place(Structure):
    _fields_ = _basic_fields

    def __init__(self, *args):
        _basic_init(self, *args)


class Wait(Structure):
    """
    The Wait class freezes the last rendered frame for a given number of seconds.

    Usage:
    ```
    scene = SceneBuilder()
    
    scene.append(
        Place(Circle(radius=0.5))
        Wait(2.2) # <-- The circle will be displayed for 2.2 seconds 
    )
    """
    _fields_ = [
        ("seconds", c_float)
    ]

    def __init__(self, seconds: float = 1.0):
        self.seconds = seconds
        
class Draw(Structure):
    _fields_ = _basic_fields + [("seconds", c_float)]

    def __init__(self, *args, seconds: float = 1.0):
        _basic_init(self, *args)
        self.seconds = seconds

class Morph(Structure):
    _fields_ = [
        ("src", POINTER(c_void_p)),
        ("dest", POINTER(c_void_p)),
        ("src_type", c_int),
        ("dest_type", c_int),
        ("seconds", c_float)
        ]

    def __init__(self, source, target, seconds: float = 1.0):
        self.src = cast(pointer(source), POINTER(c_void_p))
        self.dest = cast(pointer(target), POINTER(c_void_p))
        
        self.src_type = ShapeType.get(source.__class__.__name__.upper(), 0)
        self.dest_type = ShapeType.get(target.__class__.__name__.upper(), 0)

        self.seconds = seconds
