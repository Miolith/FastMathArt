from ctypes import POINTER, c_char_p
from numpy import ctypeslib
from fastmathart.scene import SceneElement
from fastmathart.config import ConfigBinding
import glob

lib_file= glob.glob("build/libfma*")
lib = ctypeslib.load_library("libfma", lib_file[0])

lib.render.argtypes = [POINTER(SceneElement), POINTER(ConfigBinding), c_char_p]
lib.render.restype = None