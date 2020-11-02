#pragma once
#include "GlobalData.h"

//--- calculate new rects from src and dst rect for selected scaling mode ---
void S3DAPI_API ScaleRects(TDisplayScalingMode mode, RECT* src, RECT* dst, RECT* scaledSrc, RECT* scaledDst);
