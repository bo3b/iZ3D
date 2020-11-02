#include "stdafx.h"
#include "Utils.h"

void ScaleRects(TDisplayScalingMode mode, RECT* src, RECT* dst, RECT* scaledSrc, RECT* scaledDst)
{
	*scaledSrc = *src;
	*scaledDst = *dst;

	int srcSx = src->right  - src->left;
	int srcSy = src->bottom - src->top;
	int dstSx = dst->right  - dst->left;
	int dstSy = dst->bottom - dst->top;

	switch(mode)
	{
		case DISPLAY_SCALING_TOP_LEFT:			// 1 == Image started from top left corner. (black padding right bottom)
		{
			int minSx = min(srcSx, dstSx);
			int minSy = min(srcSy, dstSy);
			scaledSrc->right   = scaledSrc->left + minSx;
			scaledSrc->bottom  = scaledSrc->top  + minSy;
			scaledDst->right   = scaledDst->left + minSx;
			scaledDst->bottom  = scaledDst->top  + minSy;
			break;
		}
		case DISPLAY_SCALING_CENTERED:			// 3 == Centering (black padding around)
		{
			int srcCenterX = (src->right   + src->left ) / 2;
			int srcCenterY = (src->bottom  + src->top  ) / 2;
			int dstCenterX = (dst->right   + dst->left ) / 2;
			int dstCenterY = (dst->bottom  + dst->top  ) / 2;
			int minHalfSx = min(srcSx, dstSx) / 2;
			int minHalfSy = min(srcSy, dstSy) / 2;
			scaledSrc->left    = srcCenterX - minHalfSx;
			scaledSrc->right   = srcCenterX + minHalfSx;
			scaledSrc->top     = srcCenterY - minHalfSy;
			scaledSrc->bottom  = srcCenterY + minHalfSy;
			scaledDst->left    = dstCenterX - minHalfSx;
			scaledDst->right   = dstCenterX + minHalfSx;
			scaledDst->top     = dstCenterY - minHalfSy;
			scaledDst->bottom  = dstCenterY + minHalfSy;
			break;
		}	
		case DISPLAY_SCALING_SCALEDASPECT:			// 4 == Scaling (Fixed Aspect Ratio Touch-from-inside, black bars may appear)
		{
			int dstCenterX = (dst->right   + dst->left ) / 2;
			int dstCenterY = (dst->bottom  + dst->top  ) / 2;
			if(srcSx * dstSy > dstSx * srcSy) // srcSx / srcSy > dstSx / dstSy
			{
				float scale = 0.5f * dstSx / srcSx;
				scaledDst->top     = LONG(dstCenterY - srcSy * scale);
				scaledDst->bottom  = LONG(dstCenterY + srcSy * scale);
			}
			else
			{
				float scale = 0.5f * dstSy / srcSy;
				scaledDst->left    = LONG(dstCenterX - srcSx * scale);
				scaledDst->right   = LONG(dstCenterX + srcSx * scale);
			}
			break;
		}
		case DISPLAY_SCALING_SCALEDASPECTEX:		// 5 == Scaling (Fixed Aspect Ratio Touch-from-outside, some data loss is possible)
		{
			int srcCenterX = (src->right   + src->left ) / 2;
			int srcCenterY = (src->bottom  + src->top  ) / 2;
			if(srcSx * dstSy <= dstSx * srcSy) // srcSx / srcSy <= dstSx / dstSy
			{
				float scale = 0.5f * srcSx / dstSx;
				scaledSrc->top     = LONG(srcCenterY - dstSy * scale);
				scaledSrc->bottom  = LONG(srcCenterY + dstSy * scale);
			}
			else
			{
				float scale = 0.5f * srcSy / dstSy;
				scaledSrc->left    = LONG(srcCenterX - dstSx * scale);
				scaledSrc->right   = LONG(srcCenterX + dstSx * scale);
			}
			break;
		}
		default:
		case DISPLAY_SCALING_SCALED:				// 2 == Scaling (N->B-> Aspect Ratio may change)
			break;
	}
}
