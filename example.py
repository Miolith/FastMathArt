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

rect = Rectangle(
    width=1.0,
    height=1.0,
    properties=Properties(
        thickness=0.01,
        color=rgb(0.8, 0.6, 0.8),
        fill=None,
        position=CENTER
    )
)

scene.append(
    Draw(circle, seconds=2.0),
    Wait(seconds=0.5),
    Morph(circle, rect, seconds=2.0),
    Wait(seconds=0.5)
)

render(scene, filename="example.mp4")