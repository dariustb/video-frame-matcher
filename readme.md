<!-- Project Shields -->
[![Gtest][gtest-svg]][gtest-url]
![GitHub repo size][repo-size]

<!-- Readme Text -->
# Video Frame Matcher

## Overview
Video Frame Matcher is a command-line tool that detects occurrences of a reference image within a video file. It scans video frames at selected intervals and reports matching frame numbers and timestamps based on a similarity threshold. The tool is designed for deterministic, scriptable use in automated workflows.

## Features
- Exact-match and template-based image detection
- Frame-index reporting
- Deterministic, scriptable CLI output
- Configurable similarity threshold
- Configurable frame-step scanning

## Installation
Build:
```sh
cmake -S . -B build
cmake --build build
```

Install vfm to ~/.local/bin (make sure this is on your PATH):
```sh
cmake --install build --prefix ~/.local
```

## How to Use
Basic Detection:
```sh
vfm --video clip.mp4 --image ref.png
```

Tuned Search:
```sh
vfm \
  --video clip.mp4 \
  --image ref.png \
  --threshold 0.85 \
  --frame-step 2 \
  --start-time 0.0 \
  --end-time 12.0
```

## Output Format
Stdout:
```sh
video-frame-matcher$ vfm --video clip.mp4 --image ref.jpg --output-json ./output.json
Found 16 match(es)
Best match at frame # 148 (2.96s) with confidence of 99.9494%.
JSON output written to: ./output.json
```

Json Output:
```json
{
    "image": {
        "channels": 3,
        "height": 1366,
        "path": "ref2.jpg",
        "width": 720
    },
    "matches": [
        {
            "bbox": {
                "h": 1366,
                "w": 720,
                "x": 0,
                "y": 0
            },
            "frame_index": 139,
            "has_bbox": true,
            "score": 0.8710225820541382,
            "time_seconds": 2.78
        },
        {
            "bbox": {
                "h": 1366,
                "w": 720,
                "x": 0,
                "y": 0
            },
            "frame_index": 140,
            "has_bbox": true,
            "score": 0.8710888028144836,
            "time_seconds": 2.8
        },
...

    ],
    "status": "SUCCESS",
    "video": {
        "duration_sec": 20.04,
        "fps": 50.0,
        "frame_count": 1002,
        "height": 1366,
        "path": "test.mp4",
        "width": 720
    }
}
```

## Testing
All tests are built as part of the normal build

Run the full test suite:
```sh
cd build
ctest --output-on-failure
```

(Optional) Run the test binary directly:
```sh
cd build/tests
./test_vfm
```

## Limitations
- Template matching may fail under heavy lighting variation
- No GPU acceleration
- No multi-image batch mode
- Assumes constant FPS streams

<!-- CI Test Badges -->
[gtest-svg]:  https://github.com/dariustb/video-frame-matcher/actions/workflows/gtest.yml/badge.svg
[gtest-url]:  https://github.com/dariustb/video-frame-matcher/actions/workflows/gtest.yml
[repo-size]:  https://img.shields.io/github/repo-size/dariustb/video-frame-matcher
