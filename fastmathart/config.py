from ctypes import Structure, c_int
class ConfigBinding(Structure):
    _fields_ = [
        ('width', c_int),
        ('height', c_int),
        ('frames_per_second', c_int),
    ]

    def __init__(self):
        self.width = config.width
        self.height = config.height
        self.frames_per_second = config.frames_per_second


class config:
    width = 1920
    height = 1080
    frames_per_second = 60

    def load_preset(preset):
        config.width = preset.width
        config.height = preset.height
        config.frames_per_second = preset.frames_per_second

class presets:
    class HD1080p_60fps:
        width = 1920
        height = 1080
        frames_per_second = 60

    class HD720p_60fps:
        width = 1280
        height = 720
        frames_per_second = 60

    class HD1080p_30fps:
        width = 1920
        height = 1080
        frames_per_second = 30

    class HD720p_30fps:
        width = 1280
        height = 720
        frames_per_second = 30
    
    class HDTwitter:
        width = 1280
        height = 720
        frames_per_second = 25
    
    class HDShort:
        width = 1080
        height = 1920
        frames_per_second = 30
    
    class SDShort:
        width = 720
        height = 1280
        frames_per_second = 30
    
    class SD480p_30fps:
        width = 480
        height = 854
        frames_per_second = 30
    
    class HDInstagramSquare:
        width = 1080
        height = 1080
        frames_per_second = 30
