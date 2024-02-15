from fastmathart import *

config.load_preset(presets.HD1080p_60fps)

scene = SceneBuilder()

circle = Circle(
    radius = 0.5,
    properties = Properties(
        thickness = 0.01,
        color=rgb(0.3, 0.6, 0.8),
        fill=None,
        position=CENTER
    )
)

scene.append(
    Draw(circle),
    Wait(seconds=4.0)
)

render(scene, filename="example.mp4")