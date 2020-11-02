#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "GlobalInfo.h"

void iz3d::profile::GlobalInfo::VendorSetting::Init( xercesc::DOMNode* pNode ) {
	xercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();
	Vendor = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"Vendor" )->getNodeValue() );
	EnableAA = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"EnableAA" )->getNodeValue() );
	WideRenderTarget = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"WideRenderTarget" )->getNodeValue() );
}

void iz3d::profile::GlobalInfo::Presets::Preset::Init( xercesc::DOMNode* pNode ) {
	xercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();
	One_div_ZPS = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"One_div_ZPS" )->getNodeValue() );
	StereoBase = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"StereoBase" )->getNodeValue() );
	AutoFocusEnable = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"AutoFocusEnable" )->getNodeValue() );
	Index = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"Index" )->getNodeValue() );
}

void iz3d::profile::GlobalInfo::Presets::Init( xercesc::DOMNode* pNode ) {
	xercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();
	xercesc::DOMNodeList* pBufNodes = 0;
	pBufNodes = pNode->getOwnerDocument()->getElementsByTagName( L"Preset" );
	vecPreset.resize( pBufNodes->getLength() );
	for ( size_t i = 0; i < pBufNodes->getLength(); ++i )
	{
		vecPreset[i].Init( pBufNodes->item( i ) );
	}
}

void iz3d::profile::GlobalInfo::Keys::Init( xercesc::DOMNode* pNode ) {
	xercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();
	ToggleStereo = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"ToggleStereo" )->getNodeValue() );
	ZPSIncrease = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"ZPSIncrease" )->getNodeValue() );
	ZPSDecrease = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"ZPSDecrease" )->getNodeValue() );
	StereoBaseIncrease = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"StereoBaseIncrease" )->getNodeValue() );
	StereoBaseDecrease = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"StereoBaseDecrease" )->getNodeValue() );
	SetPreset1 = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SetPreset1" )->getNodeValue() );
	SetPreset2 = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SetPreset2" )->getNodeValue() );
	SetPreset3 = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SetPreset3" )->getNodeValue() );
	SwapEyes = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SwapEyes" )->getNodeValue() );
	ToggleAutoFocus = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"ToggleAutoFocus" )->getNodeValue() );
	ToggleLaserSight = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"ToggleLaserSight" )->getNodeValue() );
}

void iz3d::profile::GlobalInfo::Init( xercesc::DOMNode* pNode ) {
	xercesc::DOMNamedNodeMap* pNodeMap = pNode->getAttributes();
	RouterType = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"RouterType" )->getNodeValue() );
	UseWrapper = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"UseWrapper" )->getNodeValue() );
	ShowFPS = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ShowFPS" )->getNodeValue() );
	ShowOSD = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ShowOSD" )->getNodeValue() );
	EnableStereo = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"EnableStereo" )->getNodeValue() );
	SeparationMode = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SeparationMode" )->getNodeValue() );
	FrustumAdjustMode = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"FrustumAdjustMode" )->getNodeValue() );
	BlackAreaWidth = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"BlackAreaWidth" )->getNodeValue() );
	DeviceMode = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"DeviceMode" )->getNodeValue() );
	DisableFullscreenModeEmulation = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"DisableFullscreenModeEmulation" )->getNodeValue() );
	ForceVSyncOff = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ForceVSyncOff" )->getNodeValue() );
	MonoRenderTargetTextures = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"MonoRenderTargetTextures" )->getNodeValue() );
	MonoRenderTargetSurfaces = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"MonoRenderTargetSurfaces" )->getNodeValue() );
	MonoDepthStencilTextures = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"MonoDepthStencilTextures" )->getNodeValue() );
	MonoDepthStencilSurfaces = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"MonoDepthStencilSurfaces" )->getNodeValue() );
	AlwaysModifyShaders = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"AlwaysModifyShaders" )->getNodeValue() );
	VertexShaderModificationMethod = boost::lexical_cast< std::wstring >( pNodeMap->getNamedItem( L"VertexShaderModificationMethod" )->getNodeValue() );
	ModifyIncorrectProjectionShaders = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ModifyIncorrectProjectionShaders" )->getNodeValue() );
	AlwaysUseDefaultProjectionMatrixForVS = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"AlwaysUseDefaultProjectionMatrixForVS" )->getNodeValue() );
	CreateOneComponentRTInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CreateOneComponentRTInMono" )->getNodeValue() );
	DontCreateSquareRTInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"DontCreateSquareRTInMono" )->getNodeValue() );
	DontCreateRTThatLessThanBBInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"DontCreateRTThatLessThanBBInMono" )->getNodeValue() );
	DontCreateSquareDSInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"DontCreateSquareDSInMono" )->getNodeValue() );
	DontCreateDSThatLessThanBBInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"DontCreateDSThatLessThanBBInMono" )->getNodeValue() );
	CreateBigSquareRTInStereo = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CreateBigSquareRTInStereo" )->getNodeValue() );
	CreateBigSquareDSInStereo = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CreateBigSquareDSInStereo" )->getNodeValue() );
	RenderToRightInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"RenderToRightInMono" )->getNodeValue() );
	GameSpecific = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"GameSpecific" )->getNodeValue() );
	MonoFixedPipeline = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"MonoFixedPipeline" )->getNodeValue() );
	MonoFixedPipelineCheckingMethodIndex = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"MonoFixedPipelineCheckingMethodIndex" )->getNodeValue() );
	MonoRHWPipeline = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"MonoRHWPipeline" )->getNodeValue() );
	MonoRHWCheckingMethodIndex = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"MonoRHWCheckingMethodIndex" )->getNodeValue() );
	RenderNonMatrixShadersInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"RenderNonMatrixShadersInMono" )->getNodeValue() );
	RenderIncorrectProjectionShadersInMono = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"RenderIncorrectProjectionShadersInMono" )->getNodeValue() );
	UseSimpleStereoProjectionMethod = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"UseSimpleStereoProjectionMethod" )->getNodeValue() );
	SkipCheckOrthoMatrix = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"SkipCheckOrthoMatrix" )->getNodeValue() );
	CheckExistenceInverseMatrix = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CheckExistenceInverseMatrix" )->getNodeValue() );
	CheckShadowMatrix = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CheckShadowMatrix" )->getNodeValue() );
	CheckInverseLR = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"CheckInverseLR" )->getNodeValue() );
	FixNone16x9TextPosition = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"FixNone16x9TextPosition" )->getNodeValue() );
	SeparationScale = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"SeparationScale" )->getNodeValue() );
	SearchFrameSizeX = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SearchFrameSizeX" )->getNodeValue() );
	SearchFrameSizeY = boost::lexical_cast< int >( pNodeMap->getNamedItem( L"SearchFrameSizeY" )->getNodeValue() );
	ShiftSpeed = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ShiftSpeed" )->getNodeValue() );
	Max_ABS_rZPS = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"Max_ABS_rZPS" )->getNodeValue() );
	FrameYCoordinate = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"FrameYCoordinate" )->getNodeValue() );
	LaserSightXCoordinate = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"LaserSightXCoordinate" )->getNodeValue() );
	LaserSightYCoordinate = boost::lexical_cast< float >( pNodeMap->getNamedItem( L"LaserSightYCoordinate" )->getNodeValue() );
	ShowWizardAtStartup = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"ShowWizardAtStartup" )->getNodeValue() );
	SwapEyes = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"SwapEyes" )->getNodeValue() );
	LaserSightEnable = boost::lexical_cast< bool >( pNodeMap->getNamedItem( L"LaserSightEnable" )->getNodeValue() );
	xercesc::DOMNodeList* pBufNodes = 0;
	pBufNodes = pNode->getOwnerDocument()->getElementsByTagName( L"VendorSetting" );
	vecVendorSetting.resize( pBufNodes->getLength() );
	for ( size_t i = 0; i < pBufNodes->getLength(); ++i )
	{
		vecVendorSetting[i].Init( pBufNodes->item( i ) );
	}
	pBufNodes = pNode->getOwnerDocument()->getElementsByTagName( L"Presets" );
	m_Presets.Init( pBufNodes->item( 0 ) );
	pBufNodes = pNode->getOwnerDocument()->getElementsByTagName( L"Keys" );
	m_Keys.Init( pBufNodes->item( 0 ) );
}

iz3d::profile::GlobalInfo::GlobalInfo()
{
	try
	{
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch( xercesc::XMLException& ){}
	xercesc::XercesDOMParser* pParser = new xercesc::XercesDOMParser();
	pParser->parse( L"BaseProfile.xml" );
	xercesc::DOMDocument* pXmlDoc = pParser->getDocument();
	xercesc::DOMNode* startNode = pXmlDoc->getElementsByTagName( L"DefaultProfile" )->item( 0 );
	Init( startNode );
	try
	{
		xercesc::XMLPlatformUtils::Terminate();
	}
	catch ( xercesc::XMLException& ){}
}