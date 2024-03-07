from ctypes import POINTER, pointer, c_void_p, Structure, c_int, cast
from fastmathart.basic_animation import AnimationBase

class SceneElement(Structure):
    _fields_ = [
        ('type', c_int),
        ('elem', POINTER(c_void_p)),
        ('next', POINTER(c_void_p)),
    ]

class SceneBuilder:
    _head : SceneElement = None
    _tail : SceneElement = None
    _list : list[AnimationBase] = []

    def append(self, *args : AnimationBase):
        prev = None
        self._list += args
        for animation in args:
            element = SceneElement()
            anim_obj = animation.build()

            element.elem = cast(pointer(anim_obj), POINTER(c_void_p))

            element.type = animation.anim_id if hasattr(animation, "anim_id") else 0
            if self._head is None:
                self._head = element
            else:
                self._tail.next = cast(pointer(anim_obj), POINTER(c_void_p))
            self._tail = element

            if prev is not None:
                prev.next = cast(pointer(element), POINTER(c_void_p))
            prev = element
        return self
    
    def get_scene(self):
        return self._head
    
    def __getitem__(self, index: int):
        return self._list[index]