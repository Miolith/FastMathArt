from ctypes import Structure, c_uint8, c_float, c_int
from typing import Iterable, Union, SupportsFloat as Numeric

class Color(Structure):
    """
    Color class python to C interface
    """
    _fields_ = [
        ("r", c_float),
        ("g", c_float),
        ("b", c_float),
    ]

    def __init__(
        self,
        r: float = 0.0,
        g: float = 0.0,
        b: float = 0.0,
    ):
        self.r = r
        self.g = g
        self.b = b

def rgb(
        r: Union[Numeric, Iterable],
        g: Numeric,
        b: Numeric
    ) -> Color:
    """
    Create a color object from RGB values

    Usage:
    ```
    red = rgb(1.0, 0.0, 0.0)
    red = rgb([1.0, 0.0, 0.0])
    red = rgb(np.array([1.0, 0.0, 0.0]))

    # Wrong usage
    red = rgb([1.0, 0.0]) # <-- Iterable must have 3 values
    red = rgb([1.0, 0.0, 0.0, 0.0]) # <-- Iterable must have 3 values
    ```

    Args:
    r (Numeric | Iterable): Red value or an iterable of RGB values
    g (Numeric): Green value
    b (Numeric): Blue value
    """
    if all(isinstance(i, Numeric) and 0.0 <= float(i) <= 1.0 for i in (r, g, b)):
        return Color(r, g, b)
    
    elif isinstance(r, Iterable) and len(r) == 3:
        return rgb(r[0], r[1], r[2])

    else:
        raise ValueError("Invalid color values, must be numbers between 0.0 and 1.0")
    
def rgb255(
        r: Union[Numeric, Iterable],
        g: Numeric,
        b: Numeric
    ) -> Color:
    """
    Create a color object from RGB values

    Usage:
    ```
    red = rgb255(255, 0, 0)
    red = rgb255([255, 0, 0])
    red = rgb255(np.array([255, 0, 0]))

    # Wrong usage
    red = rgb255([255, 0]) # <-- Iterable must have 3 values
    red = rgb255([255, 0, 0, 0]) # <-- Iterable must have 3 values
    ```

    Args:
    r (Numeric | Iterable): Red value or an iterable of RGB values
    g (Numeric): Green value
    b (Numeric): Blue value
    """
    if (all(isinstance(i, Numeric) and 0 <= float(i) <= 255 for i in [r, g, b])):
        return Color(*[float(i) / 255.0 for i in [r, g, b]])
    
    elif (isinstance(r, Iterable) and len(r) == 3):
        return rgb255(r[0], r[1], r[2])

    else:
        raise ValueError("Invalid color values, must be integers between 0 and 255")


def hex(hex: str) -> Color:
    if hex[0] == "#":
        hex = hex[1:]

    if len(hex) != 6:
        raise ValueError("Invalid hex color, must be 6 characters long, got {}".format(len(hex)))
    
    r = int(hex[:2], 16) / 255.0
    g = int(hex[2:4], 16) / 255.0
    b = int(hex[4:], 16) / 255.0
    return Color(r, g, b)

def hsl(
        h: Numeric,
        s: Numeric,
        l: Numeric
    ) -> Color:

    if h < 0 or h > 360:
        raise ValueError("Invalid hue value, must be between 0 and 360")
    
    if s < 0 or s > 1:
        raise ValueError("Invalid saturation value, must be between 0 and 1")
    
    if l < 0 or l > 1:
        raise ValueError("Invalid lightness value, must be between 0 and 1")
    
    # Convert HSL to RGB
    c = (1 - abs(2 * l - 1)) * s
    x = c * (1 - abs((h / 60) % 2 - 1))
    m = l - c / 2

    if 0 <= h < 60:
        r, g, b = c, x, 0
    elif 60 <= h < 120:
        r, g, b = x, c, 0
    elif 120 <= h < 180:
        r, g, b = 0, c, x
    elif 180 <= h < 240:
        r, g, b = 0, x, c
    elif 240 <= h < 300:
        r, g, b = x, 0, c
    else:
        r, g, b = c, 0, x

    return Color(r + m, g + m, b + m)