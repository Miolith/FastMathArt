from ctypes import c_float, Structure, cast, c_void_p, POINTER, c_int, pointer

WAIT_ID = 1
PLACE_ID = 2
DRAW_ID = 3
MORPH_ID = 4
SIMULTANEOUS_ID = 5

_basic_fields = [
    ("obj_list", POINTER(POINTER(c_void_p))),
    ("obj_types", POINTER(c_int)),
    ("obj_count", c_int)
]

def _basic_init(animation_class, *args):
    # Convert all args to void pointers
    object_list = [cast(pointer(arg), POINTER(c_void_p)) for arg in args]

    animation_class.obj_list = (POINTER(c_void_p) * len(object_list))(*object_list)

    type_num_list = [arg.shape_id if hasattr(arg, "shape_id") else 0 for arg in args]

    animation_class.obj_types = (c_int * len(args))(*type_num_list)
    animation_class.obj_count = len(args)



class AnimationBase:
    anim_id : int = 0

    def __and__(self, other : 'AnimationBase'):
        return Simultaneous(self, other)
    
    def build(self):
        raise NotImplementedError("build method not implemented")



class Simultaneous(AnimationBase):
    anim_id = SIMULTANEOUS_ID
    animations : list

    class SimultaneousBuilder(Structure):
        _fields_ = [
            ("animations", POINTER(POINTER(c_void_p))),
            ("anim_types", POINTER(c_int)),
            ("anim_count", c_int)
        ]

    def __init__(self, *animations):
        self.animations = animations
        self._simultaneous_obj = Simultaneous.SimultaneousBuilder()

    def __and__(self, other: AnimationBase):
        self.animations.append(other)
        return self
    
    def build(self):
        anim_list = [cast(pointer(anim.build()), POINTER(c_void_p)) for anim in self.animations]

        anim_types = [anim.anim_id for anim in self.animations]

        self._simultaneous_obj = Simultaneous.SimultaneousBuilder()
        self._simultaneous_obj.animations = (POINTER(c_void_p) * len(anim_list))(*anim_list)
        self._simultaneous_obj.anim_types = (c_int * len(anim_types))(*anim_types)
        self._simultaneous_obj.anim_count = len(anim_list)

        return self._simultaneous_obj


class Place(AnimationBase):
    anim_id = PLACE_ID
    shapes : list

    class PlaceBuilder(Structure):
        _fields_ = _basic_fields

    def __init__(self, *shapes):
        self.shapes = shapes
        self._place_obj = Place.PlaceBuilder()

    def __and__(self, other: AnimationBase):
        raise RuntimeError("Place is not an animation, it cannot be played simultaneously with other animations")
    
    def build(self):
        _basic_init(self._place_obj, *self.shapes)
        return self._place_obj



class Wait(AnimationBase):
    """
    The Wait class freezes the last rendered frame for a given number of seconds.

    Usage:
    ```
    scene = SceneBuilder()
    
    scene.append(
        Place(Circle(radius=0.5))
        Wait(2.2) # <-- The circle will be displayed for 2.2 seconds 
    )
    """
    anim_id = 1

    class WaitBuilder(Structure):
        _fields_ = [("seconds", c_float)]

    def __init__(self, seconds: float = 1.0):
        self._wait_obj = Wait.WaitBuilder()
        self.seconds = seconds
    
    def build(self):
        self._wait_obj.seconds = self.seconds
        return self._wait_obj



class Draw(AnimationBase):
    anim_id = DRAW_ID

    shapes : list
    seconds : float

    class DrawBuilder(Structure):
        _fields_ = _basic_fields\
                    + [("seconds", c_float)]

    def __init__(self, *args, seconds: float = 1.0):
        self._draw_obj = Draw.DrawBuilder()
        self.shapes = args
        self.seconds = seconds
    
    def build(self):
        _basic_init(self._draw_obj, *self.shapes)
        self._draw_obj.seconds = self.seconds
        return self._draw_obj

class Morph(AnimationBase):
    anim_id = MORPH_ID

    class MorphBuilder(Structure):
        _fields_ = [
            ("src", POINTER(c_void_p)),
            ("dest", POINTER(c_void_p)),
            ("src_type", c_int),
            ("dest_type", c_int),
            ("seconds", c_float)
        ]
    
    def __init__(self, source, target, seconds: float = 1.0):
        self._morph_obj = Morph.MorphBuilder()
        self.source = source
        self.target = target
        self.seconds = seconds

    def build(self):
        self._morph_obj.src = cast(pointer(self.source), POINTER(c_void_p))
        self._morph_obj.dest = cast(pointer(self.target), POINTER(c_void_p))
        
        self._morph_obj.src_type = self.source.shape_id if hasattr(self.source, "shape_id") else 0
        self._morph_obj.dest_type = self.target.shape_id if hasattr(self.target, "shape_id") else 0

        self._morph_obj.seconds = self.seconds
        return self._morph_obj
