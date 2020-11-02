#ifndef DRIVER_NAME_H
#define DRIVER_NAME_H

#define MONITOR_NAME			"iZ3D Monitor"
#define MONITOR_INF_FILE		"iz3dmonitor.inf"
#define FULL_PRODUCT_NAME		"iZ3D Driver"
#define PRODUCT_NAME			"iZ3D Driver"
#define COMPANY_NAME			"iZ3D Inc."
#define SHORT_COMPANY_NAME		"iZ3D"
#define CONTROL_CENTER			"iZ3D Control Center"
#define COMPANY_SITE			"http://www.iz3d.com/"
#define SETUP_FILE_NAME			"iZ3DDriverSetup"

//------------------------ section for C++ code only --------------
#ifdef	_MSC_VER
#define  S3DWRAPPERD3D7_DLL_NAME		TEXT("S3DWrapperD3D7.dll")
#define  S3DWRAPPERD3D8_DLL_NAME		TEXT("S3DWrapperD3D8.dll")
#define  S3DWRAPPERD3D9_DLL_NAME		TEXT("S3DWrapperD3D9.dll")
#define  S3DWRAPPERD3D10_DLL_NAME		TEXT("S3DWrapperD3D10.dll")
#define  S3DWRAPPEROGL_DLL_NAME			TEXT("S3DWrapperOGL.dll")
#define  S3DINJECTOR_DLL_NAME			TEXT("S3DInjector.dll")
#define  S3DOGLINJECTOR_DLL_NAME		TEXT("S3DOGLInjector.dll")
#endif 

#endif // DRIVER_NAME_H
