from ctypes import POINTER, c_char_p, cdll
import platform
from fastmathart.scene import Scene
from fastmathart.config import ConfigBinding
from glob import glob

files = []

if platform.uname()[0] == "Windows":
    files = glob("build/Release/fma.dll") or glob("build/Debug/fma.dll")

elif platform.uname()[0] == "Linux":
    files = glob("build/*fma*.so")

elif platform.uname()[0] == "Darwin":
    files = glob("build/*fma*.dylib")

else:
    raise Exception("Unsupported OS")

lib = cdll.LoadLibrary(files[0])

lib.render.argtypes = [POINTER(Scene), POINTER(ConfigBinding), c_char_p]
lib.render.restype = None