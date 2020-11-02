#pragma once
#include "S3DAPI.h"
#include "GlobalData.h"
#define  INTERLACE_STEP (4)

//--- must be one object per swapchain ---
class S3DAPI_API ShiftFinder
{
public:
	void	FindShift(UINT currentFrame);
	LONG	IsCalculationComplete(void)		{ return m_bThreadReady; }
	bool	IsDeviceMultithreaded()			{ return m_bDeviceIsMultithreaded; }
	bool	IsInitializedSuccessfully(void) { return m_bInitializationSuccessful; }
	int		GetCurrentShift(void)			{ return m_CurrentShift; }
	int		GetShiftToHold(void)			{ return m_ShiftToHold; }
	UINT32	GetCurrentCorrelation()			{ return m_MinimumCorrelation; }
	void	SetShiftToHoldInvalid(UINT currentFrame); 
	UINT	GetShiftInValidationFrame()		{ return m_ShiftInValidationFrame ;}
	bool	IsCurrentShiftValid()			{ return m_bCurrentShiftValid; }
	bool	IsShiftToHoldValid()			{ return m_bShiftToHoldValid; }
	bool	IsInterfaceDetected()			{ return m_bInterfaceDetected; }
	float	GetReverseImageSizeX()			{ return m_rImageSizeX; }

protected:
	ShiftFinder();
	~ShiftFinder() { Clear(); }

	HRESULT Initialize(RECT* pLeftRect, RECT* pRightRect);
	void	Clear();

	virtual bool GetRTData() = 0;
	void	findCorrelation();
	void	convert_R8G8B8A8_ToMonochrome(BYTE* src, int srcPith, BYTE* dest, int destSizeX);
	void	convert_X8R8G8B8_ToMonochrome(BYTE* src, int srcPith, BYTE* dest, int destSizeX);
	void	summarizeDeltasCPP();
	void	summarizeDeltasSSE();

	void	(ShiftFinder::*summarizeDeltas)();	

	//--- SSE2 optimized image buffer ---
	BYTE*		m_ImageLeft;
	BYTE*		m_ImageRight;
	RECT		m_srcRectLeft,  m_srcRectRight;
	POINT		m_dstPointLeft, m_dstPointRight;

	//--- finding parameters  ---
	bool		m_bInitializationSuccessful;
	int			m_HalfSearchIndexRange;
	float		m_rFramePixelCount;
	float		m_FrameYCoordinate;		//--- axis Y direction from bottom to up ---
	float		m_rImageSizeX;
	int			m_FrameSizeX;
	int			m_FrameSizeY;
	int			m_SearchAreaSizeX;

	//--- parameters changed by thread ---
	UINT32*		m_Curve;
	UINT32		m_MinimumCorrelation;
	int			m_ShiftToHold;
	int			m_CurrentShift;
	bool		m_bShiftToHoldValid;
	bool		m_bCurrentShiftValid;
	bool		m_bInterfaceDetected;
	UINT32		m_ZeroShiftCount;

	//--- timing information parameters ---
	UINT		m_ShiftInValidationFrame;
	UINT		m_ShiftFindingFrame;

	//--- separate thread for shift searching. one thread per SC.  ---
	LONG		m_bThreadReady;
	HANDLE		m_hThread;
	LONG		m_bStopThread;
	bool		m_bDeviceIsMultithreaded;
	friend		DWORD WINAPI FindShiftThread(ShiftFinder* lpvParam);

	//--- for auto-shift debug ---
private:
#if defined(OLEG_RESEARCH) && !defined(FINAL_RELEASE)
	AsyncButton m_WriteStatsButton;
	int			m_CurrentFrame;
	FILE*		m_StatsFile;
	void		CheckButton(UINT currentFrame);
	HRESULT		PrepareDataDir();
	void		WriteLeftAndRight();
	void		WriteStats(DWORD searchResultMask);
	TCHAR		m_StatsFileName[MAX_PATH];
	TCHAR		m_DataPath[MAX_PATH];
	bool		m_bWriteStats;
#endif
};

class S3DAPI_API AutoFocus
{
public:
	AutoFocus()
	{
		m_PreviousCallTime.QuadPart = 0;
		m_CallTimeDelta.QuadPart    = 0; 
	}
	float UpdateZPS(ShiftFinder* pShift, CameraPreset* camera);

protected:
	//--- time delta between subsequent UpdateZPS() calls ---
	LARGE_INTEGER m_PreviousCallTime;
	LARGE_INTEGER m_CallTimeDelta;
};
