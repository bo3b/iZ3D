#include "stdafx.h"
#include "LaserSightData.h"
#define SIGHT_NATIVE_RESOLUTION 1050.f
#define SIGHT_SIZEX	64 
#define SIGHT_SIZEY	64
#define SIGHT_FRAME_DELTA (1.f / 15)

LaserSightData::LaserSightData(bool bShiftTU_TV)
{
	m_ResolutionScale = 1.f;
	m_AnimationTUDelta = 0;
	m_AnimationTimeDelta = 0;
	m_bShiftTU_TV = bShiftTU_TV;
	if(bShiftTU_TV)
	{
		m_CenterU = 0.5f / SIGHT_SIZEX;
		m_CenterV = 0.5f / SIGHT_SIZEY;
	}
	else
	{
		m_CenterU = 0;
		m_CenterV = 0;
	}

	SetRect(&m_LeftRect,  0,0,SIGHT_SIZEX,SIGHT_SIZEY);
	SetRect(&m_RightRect, 0,0,SIGHT_SIZEX,SIGHT_SIZEY);

	m_SightLocationL[0].z   = m_SightLocationR[0].z   = 0;
	m_SightLocationL[0].rhw = m_SightLocationR[0].rhw = 1;
	m_SightLocationL[0].tu  = m_SightLocationR[0].tu  = 0 + m_CenterU;    
	m_SightLocationL[0].tv  = m_SightLocationR[0].tv  = 0 + m_CenterV;    

	m_SightLocationL[1].z   = m_SightLocationR[1].z   = 0;
	m_SightLocationL[1].rhw = m_SightLocationR[1].rhw = 1;
	m_SightLocationL[1].tu  = m_SightLocationR[1].tu  = 1 - m_CenterU;    
	m_SightLocationL[1].tv  = m_SightLocationR[1].tv  = 0 + m_CenterV;    

	m_SightLocationL[2].z   = m_SightLocationR[2].z   = 0;
	m_SightLocationL[2].rhw = m_SightLocationR[2].rhw = 1;
	m_SightLocationL[2].tu  = m_SightLocationR[2].tu  = 1 - m_CenterU;    
	m_SightLocationL[2].tv  = m_SightLocationR[2].tv  = 1 - m_CenterV;    

	m_SightLocationL[3].z   = m_SightLocationR[3].z   = 0;
	m_SightLocationL[3].rhw = m_SightLocationR[3].rhw = 1;
	m_SightLocationL[3].tu  = m_SightLocationR[3].tu  = 0 + m_CenterU;    
	m_SightLocationL[3].tv  = m_SightLocationR[3].tv  = 1 - m_CenterV;  
}

void LaserSightData::Initialize(SIZE textureSize, RECT* pLeftRect, RECT* pRightRect)
{
	m_LeftRect  = *pLeftRect;
	m_RightRect = *pRightRect;

	if(m_bShiftTU_TV)
	{
		m_CenterU = 0.5f / textureSize.cx;
		m_CenterV = 0.5f / textureSize.cy;
	}
	else
	{
		m_CenterU = 0;
		m_CenterV = 0;
	}

	//--- calculate resolution scale ---
	m_ResolutionScale = float(pLeftRect->bottom - pLeftRect->top) / SIGHT_NATIVE_RESOLUTION;

	//--- initialize render target coordinates ---
	SetShift(0, 0);

	m_CurrentFrame = 0;
	//--- initialize texture coordinates and animation step ---
	if(SIGHT_SIZEX >= textureSize.cx)
	{ 
		//--- disable sight animation ---
		m_AnimationTUDelta = 0;
		m_initTU = 1;
		m_FrameNumber = 1;
	}
	else
	{
		m_AnimationTUDelta = float(SIGHT_SIZEX) / float(textureSize.cx);
		m_FrameNumber = int(textureSize.cx) / int(SIGHT_SIZEX); 
		m_initTU = m_AnimationTUDelta;
	}

	if((1 + 2 * gInfo.LaserSightIconIndex) * SIGHT_SIZEY >= (UINT)textureSize.cy)
	{
		//--- disable stereo sight ---
		m_initTV1 = 0;
		m_initTV2 = 1;
		m_initTV3 = 0;
		m_initTV4 = 1;
	}
	else
	{
		m_initTV1 = 2 * gInfo.LaserSightIconIndex * float(SIGHT_SIZEY) / float(textureSize.cy);
		m_initTV2 = m_initTV1 + float(SIGHT_SIZEY) / float(textureSize.cy);
		m_initTV3 = m_initTV2;
		m_initTV4 = m_initTV3 + float(SIGHT_SIZEY) / float(textureSize.cy);
	}

	m_AnimationTimeDelta = 0;

	m_SightLocationL[0].tu  = 0         + m_CenterU; 
	m_SightLocationR[0].tu  = 0         + m_CenterU;    
	m_SightLocationL[0].tv  = m_initTV1 + m_CenterV;
	m_SightLocationR[0].tv  = m_initTV3 + m_CenterV;    

	m_SightLocationL[1].tu  = m_initTU  + m_CenterU;    
	m_SightLocationR[1].tu  = m_initTU  + m_CenterU;    
	m_SightLocationL[1].tv  = m_initTV1 + m_CenterV;
	m_SightLocationR[1].tv  = m_initTV3 + m_CenterV;    

	m_SightLocationL[2].tu  = m_initTU  + m_CenterU;    
	m_SightLocationR[2].tu  = m_initTU  + m_CenterU;    
	m_SightLocationL[2].tv  = m_initTV2 + m_CenterV;
	m_SightLocationR[2].tv  = m_initTV4 + m_CenterV;    

	m_SightLocationL[3].tu  = 0         + m_CenterU;    
	m_SightLocationR[3].tu  = 0         + m_CenterU;    
	m_SightLocationL[3].tv  = m_initTV2 + m_CenterV;
	m_SightLocationR[3].tv  = m_initTV4 + m_CenterV;  
}

void LaserSightData::SetShift(int shift, DWORD separationMode)
{
	float centerLX = -0.5f + m_LeftRect.left  +    gInfo.LaserSightXCoordinate  * (m_LeftRect.right   - m_LeftRect.left );
	float centerRX = -0.5f + m_RightRect.left +    gInfo.LaserSightXCoordinate  * (m_RightRect.right  - m_RightRect.left);
	float centerLY = -0.5f + m_LeftRect.top   + (1-gInfo.LaserSightYCoordinate) * (m_LeftRect.bottom  - m_LeftRect.top  );
	float centerRY = -0.5f + m_RightRect.top  + (1-gInfo.LaserSightYCoordinate) * (m_RightRect.bottom - m_RightRect.top );

	DEBUG_TRACE3(_T("LaserSight: X = %f,  Y = %f\n"),  centerLX, centerLY);

	float scale;
	int	  s = shift;
	if (s >  64) s =  64;
	if (s < -64) s = -64;
	scale = (1 - s / 128.f) * m_ResolutionScale;
	float scaledHalfSizeX = scale * SIGHT_SIZEX / 2;
	float scaledHalfSizeY = scale * SIGHT_SIZEY / 2;

	switch(separationMode)
	{
	case 0:
		centerLX -= 0.5f * shift;
		centerRX += 0.5f * shift;
		break;
	case 1:
		centerRX += shift;
		break;
	default:
		centerLX -= shift;
		break;
	}

	m_SightLocationL[0].x = m_SightLocationL[3].x  = centerLX - scaledHalfSizeX;
	m_SightLocationL[1].x = m_SightLocationL[2].x  = centerLX + scaledHalfSizeX;
	m_SightLocationR[0].x = m_SightLocationR[3].x  = centerRX - scaledHalfSizeX;
	m_SightLocationR[1].x = m_SightLocationR[2].x  = centerRX + scaledHalfSizeX;

	m_SightLocationL[0].y = m_SightLocationL[1].y  = centerLY - scaledHalfSizeY;
	m_SightLocationL[2].y = m_SightLocationL[3].y  = centerLY + scaledHalfSizeY;
	m_SightLocationR[0].y = m_SightLocationR[1].y  = centerRY - scaledHalfSizeY;
	m_SightLocationR[2].y = m_SightLocationR[3].y  = centerRY + scaledHalfSizeY;
}

void LaserSightData::SelectFrame(float frameTimeDelta)
{
	m_AnimationTimeDelta += frameTimeDelta;
	if(m_AnimationTimeDelta >= SIGHT_FRAME_DELTA)
	{
		int FramePassed = int(m_AnimationTimeDelta / SIGHT_FRAME_DELTA);
		m_AnimationTimeDelta -= FramePassed * SIGHT_FRAME_DELTA;
		m_CurrentFrame += FramePassed;
		m_CurrentFrame %= m_FrameNumber;

		m_SightLocationL[0].tu  = m_SightLocationR[0].tu  = m_CurrentFrame       * m_initTU + m_CenterU;    
		m_SightLocationL[1].tu  = m_SightLocationR[1].tu  = (m_CurrentFrame + 1) * m_initTU + m_CenterU;    
		m_SightLocationL[2].tu  = m_SightLocationR[2].tu  = (m_CurrentFrame + 1) * m_initTU + m_CenterU;    
		m_SightLocationL[3].tu  = m_SightLocationR[3].tu  = m_CurrentFrame       * m_initTU + m_CenterU;    
	}
}