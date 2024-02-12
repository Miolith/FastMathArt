from fastmathart import *

config.load_preset(presets.HD720p_30fps)

scene = SceneBuilder()

circle = Circle(
    radius = 0.5,
    properties = Properties(
        thickness = 0.1,
        color=rgb(0.3, 0.6, 0.8),
        fill=None,
        position=CENTER
    )
)

scene.append(
    Place(circle),
    Wait(seconds=4.0)
)

render(scene, filename="example.mp4")