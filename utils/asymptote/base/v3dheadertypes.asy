// Enum class for v3dheadertypes
// AUTO-GENERATED from v3dheadertypes.csv
// Generated at 2024-03-08 08:14:52

struct v3dheadertypes
{
  int canvasWidth=1;
// UINT  Canvas width

  int canvasHeight=2;
// UINT  Canvas heighot

  int absolute=3;
// BOOL  true: absolute size; false: scale to canvas

  int minBound=4;
// TRIPLE  Scene minimum bounding box corners

  int maxBound=5;
// TRIPLE  Scene maximum bounding box corners

  int orthographic=6;
// BOOL  true: orthographic; false: perspective

  int angleOfView=7;
// REAL  Field of view angle (in radians)

  int initialZoom=8;
// REAL  Initial zoom

  int viewportShift=9;
// PAIR  Viewport shift (for perspective projection)

  int viewportMargin=10;
// PAIR  Margin around viewport

  int light=11;
// RGB  Direction and color of each point light source

  int background=12;
// RGBA  Background color

  int zoomFactor=13;
// REAL  Zoom base factor

  int zoomPinchFactor=14;
// REAL  Zoom pinch factor

  int zoomPinchCap=15;
// REAL  Zoom pinch limit

  int zoomStep=16;
// REAL  Zoom power step

  int shiftHoldDistance=17;
// REAL  Shift-mode maximum hold distance (pixels)

  int shiftWaitTime=18;
// REAL  Shift-mode hold time (milliseconds)

  int vibrateTime=19;
// REAL  Shift-mode vibrate time (milliseconds)

};

v3dheadertypes v3dheadertypes;// End of File
