#pragma once
bool WM__InitIVRSearch(const Rect *pMaxRect);
bool WM__GetNextIVR(void);
#define WM_ITERATE_START(pRect)                   \
  {                                               \
    if (WM__InitIVRSearch(pRect))                 \
      do {
#define WM_ITERATE_END()                          \
    } while (WM__GetNextIVR());                   \
  }
#define WM_ADDORGX(x)    x += GUI.xOff
#define WM_ADDORGY(y)    y += GUI.yOff
#define WM_ADDORG(x0,y0) WM_ADDORGX(x0); WM_ADDORGY(y0)
#define WM_SUBORGX(x)    x -= GUI.xOff
#define WM_SUBORGY(y)    y -= GUI.yOff
#define WM_SUBORG(x0,y0) WM_SUBORGX(x0); WM_SUBORGY(y0)
