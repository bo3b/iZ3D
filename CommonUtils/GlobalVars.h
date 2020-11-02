#pragma once

#include "Singleton.h"
#include "GlobalInfo.h"
namespace iz3d
{
	struct DATA_INPUT {
		unsigned ActivePresetIndex_;
		float ZNear_;
		float Multiplier_;
		float ConvergenceShift_;
		DWORD DebugInfoPage_;
		bool StereoActive_;
		bool SwapEyes_;
		bool LaserSightEnable_;
		void SwitchCurrentPresetAutofocus();
		iz3d::profile::Preset* getCurrentPreset();
	};

	inline iz3d::profile::Preset* DATA_INPUT::getCurrentPreset()
	{
		return &iz3d::profile::GlobalInfo::Get().m_Presets.vecPreset[ActivePresetIndex_];
	}

	inline void DATA_INPUT::SwitchCurrentPresetAutofocus()
	{
		iz3d::profile::Preset* p = getCurrentPreset();
		if(p->AutoFocusEnable)
		{
			p->AutoFocusEnable = FALSE;
			p->One_div_ZPS = p->One_div_ZPS;
		}
		else
		{
			p->One_div_ZPS_Saved = p->One_div_ZPS;
			p->AutoFocusEnable = TRUE;
		}
	}

	inline bool operator==(iz3d::DATA_INPUT inputOne, iz3d::DATA_INPUT inputOther)
	{
		return (inputOne.getCurrentPreset()->StereoBase		== inputOther.getCurrentPreset()->StereoBase  &&
			inputOne.getCurrentPreset()->One_div_ZPS		== inputOther.getCurrentPreset()->One_div_ZPS &&
			inputOne.SwapEyes_							== inputOther.SwapEyes_);
	}

	inline bool operator!=(iz3d::DATA_INPUT inputOne, iz3d::DATA_INPUT inputOther)
	{
		return !(inputOne == inputOther);
	}

	

	class GlobalVars : public iz3d::Singleton< GlobalVars >
	{
		friend class iz3d::Singleton< GlobalVars >;
	public:
		iz3d::DATA_INPUT input_;
	};
	
}