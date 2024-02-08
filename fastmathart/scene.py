import ctypes
import fastmathart.cbindings as cbind

class SceneElement(ctypes.Structure):
    _fields_ = [
        ('type', ctypes.c_int),
        ('elem', ctypes.POINTER(ctypes.c_void_p)),
        ('next', ctypes.POINTER(ctypes.c_void_p)),
    ]

class SceneBuilder:
    _head : SceneElement = None
    _tail : SceneElement = None

    def append(self, *args):
        prev = None
        for arg in args:
            element = SceneElement()
            element.elem = ctypes.cast(ctypes.pointer(arg), ctypes.POINTER(ctypes.c_void_p))

            class_name = arg.__class__.__name__.upper()
            element.type = cbind.ElementType.get(class_name, cbind.ElementType["NOTHING"])
            if self._head is None:
                self._head = element
            else:
                self._tail.next = ctypes.cast(ctypes.pointer(arg), ctypes.POINTER(ctypes.c_void_p))
            self._tail = element

            if prev is not None:
                prev.next = ctypes.cast(ctypes.pointer(element), ctypes.POINTER(ctypes.c_void_p))
            prev = element
        return self
    
    def get_scene(self):
        return self._head