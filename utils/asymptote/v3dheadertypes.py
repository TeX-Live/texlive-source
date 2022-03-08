#!/usr/bin/env python3
# Enum class for v3dheadertypes
""" AUTO-GENERATED from v3dheadertypes.csv """
# Generated at 2022-03-07 12:05:14.330045

class v3dheadertypes:
    v3dheadertypes_canvasWidth=1
    # UINT  Canvas width

    v3dheadertypes_canvasHeight=2
    # UINT  Canvas heighot

    v3dheadertypes_absolute=3
    # BOOL  true: absolute size; false: scale to canvas

    v3dheadertypes_minBound=4
    # TRIPLE  Scene minimum bounding box corners

    v3dheadertypes_maxBound=5
    # TRIPLE  Scene maximum bounding box corners

    v3dheadertypes_orthographic=6
    # BOOL  true: orthographic; false: perspective

    v3dheadertypes_angleOfView=7
    # REAL  Field of view angle (in radians)

    v3dheadertypes_initialZoom=8
    # REAL  Initial zoom

    v3dheadertypes_viewportShift=9
    # PAIR  Viewport shift (for perspective projection)

    v3dheadertypes_viewportMargin=10
    # PAIR  Margin around viewport

    v3dheadertypes_light=11
    # RGB  Direction and color of each point light source

    v3dheadertypes_background=12
    # RGBA  Background color

    v3dheadertypes_zoomFactor=13
    # REAL  Zoom base factor

    v3dheadertypes_zoomPinchFactor=14
    # REAL  Zoom pinch factor

    v3dheadertypes_zoomPinchCap=15
    # REAL  Zoom pinch limit

    v3dheadertypes_zoomStep=16
    # REAL  Zoom power step

    v3dheadertypes_shiftHoldDistance=17
    # REAL  Shift-mode maximum hold distance (pixels)

    v3dheadertypes_shiftWaitTime=18
    # REAL  Shift-mode hold time (milliseconds)

    v3dheadertypes_vibrateTime=19
    # REAL  Shift-mode vibrate time (milliseconds)

# End of File
