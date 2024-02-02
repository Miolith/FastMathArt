import fastmathart as fma

fma.config.width = 1920
fma.config.height = 1080
fma.config.frames_per_second = 60

scene = fma.SceneBuilder()

scene.add(fma.Wait(seconds=4.0))

fma.render(scene, filename="example.mp4")