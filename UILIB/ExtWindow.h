#pragma once
#include <string>
#include <map>
namespace uilib {
class WizardSCData;
}

class CXExtWindow :
	public CXWindow
{
	typedef std::map< WPARAM, CXWindow* > TReacts;
	TReacts m_Reacts;
	bool	m_prevSt;
	//uilib::WizardSCData* m_pWizard;
	static std::wstring m_checkedText;
	static std::wstring m_uncheckedText;
public:
	static void SetCheckedText(const TCHAR* cT);
	static void SetUncheckedText(const TCHAR* uT);
	virtual void OnSysKey(WPARAM Key, LPARAM Extended);
	virtual void AddReact(WPARAM Key, CXWindow* pWindow);
	virtual bool OnRender();
	CXExtWindow( iz3d::uilib::CommonDevice* pDev );
	virtual ~CXExtWindow(void);
};
