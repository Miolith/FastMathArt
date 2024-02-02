from ctypes import c_float, Structure, cast

class Wait(Structure):
    _fields_ = [
        ("seconds", c_float)
    ]

    def __init__(self, seconds: float = 1.0):
        self.seconds = seconds