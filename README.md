# FastMathArt
Python library to draw math animations fast

## Description
This library is an attempt at finding a middle ground between the simplicity of Manim and the power of ThreeJS.
This project is at a very early development state, thus too basic to be useful.

It's explicitly made for content creator as a way to create procedural animations in 2d and 3d.

## Install

You first need to have python 3.9 or higher installed.
All the dependencies are managed by **conan**, so you need to have it installed as well.
```
pip install conan
```

then just paste these commands in your terminal.

```
git clone https://github.com/Miolith/FastMathArt.git
cd FastMathArt
mkdir build
conan install . --output-folder build --build=missing
cd build && cmake ..
make fma
```

## Example
```python
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
    width=1.0, height=1.0,
    properties=Properties(
        thickness=0.01,
        color=rgb(0.8, 0.6, 0.8),
        fill=None,
        position=CENTER
    )
)

scene.append(
    Draw(rect, seconds=2.0),
    Wait(seconds=0.5),
    Morph(rect, circle, seconds=2.0),
    Wait(seconds=0.5)
)

render(scene, filename="example.mp4")
```
Run
```
python example.py
```
https://github.com/Miolith/FastMathArt/assets/36192751/181dd109-5562-4e6c-865a-72cff9d87b42
