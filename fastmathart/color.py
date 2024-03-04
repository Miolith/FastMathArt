from ctypes import Structure, POINTER, c_void_p, c_uint8, c_float, cast, pointer, c_int
from typing import Iterable, Union

NOTYPE = 0
RGB_3_FLOATS = 1
RGB_3_BYTES = 2
HEX_STRING = 3
HSL = 4

class Color(Structure):
    """
    Color class python to C interface
    """
    _fields_ = [
        ("value", POINTER(c_void_p)),
        ("type", c_int)
    ]

    def __init__(
        self,
        r: Union[float, str] = None,
        g: float = None,
        b: float = None,
        type: int = NOTYPE
    ):
        self.type = type

        if type == RGB_3_FLOATS:
            self.value = cast(pointer((c_float * 3)(r, g, b)), POINTER(c_void_p))

        elif type == RGB_3_BYTES:
            self.value = cast(pointer((c_uint8 * 3)(r, g, b)), POINTER(c_void_p))

        elif type == HEX_STRING:
            self.value = cast(pointer(c_void_p(hex)), POINTER(c_void_p))

        else:
            self.value = None


def rgb(
        r: Union[float , int , Iterable],
        g: Union[float , int],
        b: Union[float , int]
    ) -> Color:
    """
    Create a color object from RGB values

    Usage:
    ```
    red = rgb(1.0, 0.0, 0.0)
    red = rgb(r=255, g=0, b=0)
    red = rgb([1.0, 0.0, 0.0])
    red = rgb([255, 0, 0])
    red = rgb(np.array([1.0, 0.0, 0.0]))

    # Wrong usage
    red = rgb(1.0, 0, 0) # <-- All values must be of the same type
    red = rgb(300, 0, 0) # <-- Int values must be between 0 and 255
    red = rgb([1.0, 0.0]) # <-- Iterable must have 3 values
    red = rgb([1.0, 0.0, 0.0, 0.0]) # <-- Iterable must have 3 values
    ```

    Args:
    r (float | int | Iterable): Red value or an iterable of RGB values
    g (float | int): Green value
    b (float | int): Blue value
    """
    if (isinstance(r, float) and isinstance(g, float) and isinstance(b, float)
            and 0.0 <= r <= 1.0
            and 0.0 <= g <= 1.0
            and 0.0 <= b <= 1.0):
        return Color(r, g, b, type=RGB_3_FLOATS)

    elif (isinstance(r, int) and isinstance(g, int) and isinstance(b, int)
            and 0 <= r <= 255
            and 0 <= g <= 255
            and 0 <= b <= 255):
        
        return Color(r, g, b, type=RGB_3_BYTES)
    
    elif (isinstance(r, Iterable) and len(r) == 3):
        return rgb(r[0], r[1], r[2])

    else:
        raise ValueError("Invalid color values, must be either floats between 0.0 and 1.0 or integers between 0 and 255.")
    

def hex(hex: str) -> Color:
    if hex[0] == "#":
        hex = hex[1:]
    
    if len(hex) != 6:
        raise ValueError("Invalid hex string, must be 6 characters long.")
    c = Color(hex, type=HEX_STRING)
    return c

def hsl(h: int, s: int, l: int) -> Color:
    c = Color(h, s, l, type=HSL)
    return c