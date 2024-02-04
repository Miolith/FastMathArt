from ctypes import POINTER, c_void_p, c_int, c_char_p
from numpy import ctypeslib
from fastmathart.scene import SceneElement
import glob

ElementType = {
    "NOTHING": 0,
    "WAIT": 1,
    "PLACE": 2,
}

lib_file= glob.glob("build/lib*/fastmathart/cbindings*")
lib = ctypeslib.load_library("cbindings", lib_file[0])
lib.render.argtypes = [POINTER(SceneElement), c_int, c_int, c_char_p]
lib.render.restype = None