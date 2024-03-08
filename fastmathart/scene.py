from ctypes import POINTER, pointer, c_void_p, Structure, c_int, cast
from fastmathart.basic_animation import AnimationBase

class SceneElement(Structure):
    _fields_ = [
        ('type', c_int),
        ('elem', POINTER(c_void_p)),
    ]

class Scene(Structure):
    _fields_ = [
        ('elements', POINTER(SceneElement)),
        ('element_count', c_int),
    ]

class SceneBuilder:
    _list : list[AnimationBase] = []

    def __init__(self):
        self.scene_obj = Scene()

    def append(self, *args : AnimationBase):
        self._list += args
        return self

    
    def build(self):
        elem_list = []
        for animation in self._list:
            element = SceneElement()
            anim_obj = animation.build()

            element.elem = cast(pointer(anim_obj), POINTER(c_void_p))
            element.type = animation.anim_id if hasattr(animation, "anim_id") else 0
            elem_list.append(element)
        
        self.scene_obj.elements = (SceneElement * len(elem_list))(*elem_list)
        self.scene_obj.element_count = len(elem_list)

        return self.scene_obj

    def get_scene(self):
        return tuple(self._list)
    
    def __getitem__(self, index: int):
        return self._list[index]
    
