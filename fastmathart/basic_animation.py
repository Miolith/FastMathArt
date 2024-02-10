from ctypes import c_float, Structure, cast, c_void_p, POINTER, c_int, pointer

class Place(Structure):
    _fields_ = [
        ("obj_list", POINTER(POINTER(c_void_p))),
        ("obj_types", POINTER(c_int)),
        ("obj_count", c_int)
    ]

    def __init__(self, *args):
        # Convert all args to void pointers
        args = [cast(pointer(arg), POINTER(c_void_p)) for arg in args]

        self.obj_list = (POINTER(c_void_p) * len(args))(*args)
        self.obj_types = (c_int * len(args))(*[1 for _ in range(len(args))])
        self.obj_count = len(args)


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
        