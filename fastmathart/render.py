from typing import List, Union
from fastmathart.cbindings import lib
import ctypes

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
        scene,
        filename: str,
    ):
    """Render a scene to a video file."""
    lib.render(scene._head, 50, 50, ctypes.c_char_p(filename.encode('utf-8')))