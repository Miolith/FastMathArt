from typing import List, Union
from fastmathart.cbindings import lib
import ctypes
from fastmathart.config import config
from fastmathart.scene import SceneBuilder

def render_frames(
        scene,
        frames: Union[List[int], int],
        filenames: Union[List[str], str] = None
    ):
    """Render only selected frames of a scene to a file."""

    if filenames is None:
        filenames = [f"frame_{frame}.png" for frame in frames]

    if isinstance(frames, int):
        frames = [frames]
    
    if isinstance(filenames, str):
        filenames = [filenames]


def render(
        scene: SceneBuilder,
        filename: str,
    ):
    """Render a scene to a video file."""
    lib.render(
        scene.get_scene(),
        ctypes.c_int(config.width),
        ctypes.c_int(config.height),
        ctypes.c_char_p(filename.encode('utf-8'))
    )