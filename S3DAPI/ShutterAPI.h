#pragma once 
#include "iZ3DShutterServiceAPI.h"

class ShutterAPI
{
public:
	ShutterAPI()
	{
		m_biZ3DShutterStatus	= NULL;					//--- is available to use
		m_biZ3DShutterEnabled	= NULL;					//--- is active now
	}
	~ShutterAPI()
	{
		ReleaseShutter();
	}

	BOOL InitShutter()
	{
		if(!m_biZ3DShutterStatus)
		{
			m_biZ3DShutterEnabled	= FALSE;
			m_biZ3DShutterStatus	= iZ3DInitShutter();
		}
		return m_biZ3DShutterStatus;
	}

	void ReleaseShutter()
	{
		if(m_biZ3DShutterStatus)		
		{
			iZ3DStopShutter();
			iZ3DReleaseShutter();
			m_biZ3DShutterEnabled	= FALSE;
			m_biZ3DShutterStatus	= NULL;
		}
	}

	void StartShutter()
	{
		if(m_biZ3DShutterStatus && !m_biZ3DShutterEnabled)
		{
			iZ3DStartShutter();
			m_biZ3DShutterEnabled = TRUE;
		}
	}

	void StopShutter()
	{
		if(m_biZ3DShutterStatus && m_biZ3DShutterEnabled)
		{
			iZ3DStopShutter();
			m_biZ3DShutterEnabled = FALSE;
		}
	}

	void Wait()
	{
		iZ3DWaitForPresent(100);
	}

protected:
	// kernel mode shutter stuff
	BOOL		m_biZ3DShutterStatus;				//--- is available to use
	BOOL		m_biZ3DShutterEnabled;				//--- is active now
};

extern S3DAPI_API ShutterAPI g_KMShutter;
