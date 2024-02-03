from ctypes import c_float, Structure, cast, c_void_p, POINTER, c_int, pointer

class Place(Structure):
    _fields_ = [
        ("obj", POINTER(POINTER(c_void_p))),
        ("obj_types", POINTER(c_int)),
        ("obj_count", c_int)
    ]

    def __init__(self, *args):
        args = [cast(pointer(arg), POINTER(c_void_p)) for arg in args]
        self.obj = (POINTER(c_void_p) * len(args))(*args)
        self.obj_types = (c_int * len(args))(*[1 for _ in range(len(args))])
        self.obj_count = len(args)
        