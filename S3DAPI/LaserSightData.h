#pragma once

//--- compatible with D3DTVERTEX_1T structure ---
struct LS_LOCATION { float x,y,z,rhw; float tu,tv; };

class S3DAPI_API LaserSightData
{
public:
	LaserSightData(bool bShiftTU_TV);
	void Initialize(SIZE textureSize, RECT* pLeftRect, RECT* pRightRect);
	void SetShift(int shift, DWORD separationMode);
	
	void		 SelectFrame(float frameTimeDelta);
	LS_LOCATION* GetLocationLeft()  { return m_SightLocationL; }
	LS_LOCATION* GetLocationRight() { return m_SightLocationR; }
protected:

	RECT		m_LeftRect;
	RECT		m_RightRect;
	LS_LOCATION	m_SightLocationL[4];
	LS_LOCATION	m_SightLocationR[4];
	float		m_ResolutionScale;
	float		m_AnimationTUDelta;
	float		m_AnimationTimeDelta;	
	int			m_CurrentFrame;
	int			m_FrameNumber;	
	
	//--- initial texture coordinates ---
	float m_initTU, m_initTV1, m_initTV2, m_initTV3, m_initTV4;
	//--- true when DynRenderer in DX10 used ---
	bool  m_bShiftTU_TV;
	//--- texel center correction ---
	float m_CenterU, m_CenterV;
};

