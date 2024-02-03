from fastmathart import *

config.load_preset(presets.HD720p_30fps)

scene = SceneBuilder()

circle = Circle(
    radius = 0.5,
    thickness = 0.1,
    color=RED,
    fill=None,
    position=CENTER
)

scene.append(
    Place(circle),
    Wait(seconds=4.0)
)

render(scene, filename="example.mp4")