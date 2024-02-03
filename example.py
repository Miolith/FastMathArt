import fastmathart as fma

fma.config.load_preset(fma.presets.HD720p_30fps)

scene = fma.SceneBuilder()

scene.add(fma.Wait(seconds=4.0))

fma.render(scene, filename="example.mp4")