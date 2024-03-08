from typing import List, Union
from fastmathart.api_bindings import lib
from ctypes import pointer, c_char_p
from fastmathart.config import ConfigBinding
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
        scene.build(),
        pointer(ConfigBinding()),
        c_char_p(filename.encode('utf-8'))
    )