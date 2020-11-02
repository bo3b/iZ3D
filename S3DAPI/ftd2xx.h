#ifndef _FTD2XX_H_
#define _FTD2XX_H_

typedef HMODULE							FT_HANDLE;

//
// FT status
//
enum FT_STATUS
{
	FT_OK,
	FT_INVALID_HANDLE,
	FT_DEVICE_NOT_FOUND,
	FT_DEVICE_NOT_OPENED,
	FT_IO_ERROR,
	FT_INSUFFICIENT_RESOURCES,
	FT_INVALID_PARAMETER
};
#define FT_SUCCESS(status) ((status) == FT_OK)

//
// FT_OpenEx Flags
//
#define FT_OPEN_BY_SERIAL_NUMBER		1
#define FT_OPEN_BY_DESCRIPTION			2

//
// Baud Rates
//
#define FT_BAUD_300						300
#define FT_BAUD_600						600
#define FT_BAUD_1200					1200
#define FT_BAUD_2400					2400
#define FT_BAUD_4800					4800
#define FT_BAUD_9600					9600
#define FT_BAUD_14400					14400
#define FT_BAUD_19200					19200
#define FT_BAUD_38400					38400
#define FT_BAUD_57600					57600
#define FT_BAUD_115200					115200
#define FT_BAUD_230400					230400
#define FT_BAUD_460800					460800
#define FT_BAUD_921600					921600

//
// Word Lengths
//
#define FT_BITS_8						(UCHAR) 8
#define FT_BITS_7						(UCHAR) 7
#define FT_BITS_6						(UCHAR) 6
#define FT_BITS_5						(UCHAR) 5

//
// Stop Bits
//
#define FT_STOP_BITS_1					(UCHAR) 0
#define FT_STOP_BITS_1_5				(UCHAR) 1
#define FT_STOP_BITS_2					(UCHAR) 2

//
// Parity
//
#define FT_PARITY_NONE					(UCHAR) 0
#define FT_PARITY_ODD					(UCHAR) 1
#define FT_PARITY_EVEN					(UCHAR) 2
#define FT_PARITY_MARK					(UCHAR) 3
#define FT_PARITY_SPACE					(UCHAR) 4

//
// Flow Control
//
#define FT_FLOW_NONE					0x0000
#define FT_FLOW_RTS_CTS					0x0100
#define FT_FLOW_DTR_DSR					0x0200
#define FT_FLOW_XON_XOFF				0x0400


typedef struct _ft_device_list_info_node
{
	DWORD		Flags;
	DWORD		Type;
	DWORD		ID;
	DWORD		LocId;
	char		SerialNumber[16];
	char		Description[64];
	FT_HANDLE	ftHandle;
}
FT_DEVICE_LIST_INFO_NODE;

//////////////////////////////////////////////////////////////////////////

BOOL									InitFTD2XX();
VOID									ReleaseFTD2XX();

typedef FT_STATUS						( WINAPI *TPFN_FT_OpenEx )( PVOID, DWORD, FT_HANDLE* );
typedef FT_STATUS						( WINAPI *TPFN_FT_Close )( FT_HANDLE );
typedef FT_STATUS						( WINAPI *TPFN_FT_CreateDeviceInfoList )( LPDWORD );
typedef FT_STATUS						( WINAPI *TPFN_FT_GetDeviceInfoList )( FT_DEVICE_LIST_INFO_NODE*, LPDWORD );
typedef FT_STATUS						( WINAPI *TPFN_FT_SetBaudRate )( FT_HANDLE , DWORD );
typedef FT_STATUS						( WINAPI *TPFN_FT_SetDataCharacteristics )( FT_HANDLE, UCHAR, UCHAR, UCHAR );
typedef FT_STATUS						( WINAPI *TPFN_FT_SetFlowControl )( FT_HANDLE, USHORT, UCHAR, UCHAR );
typedef FT_STATUS						( WINAPI *TPFN_FT_Write )( FT_HANDLE, LPVOID, DWORD, LPDWORD );
typedef FT_STATUS						( WINAPI *TPFN_FT_Read )( FT_HANDLE, LPVOID, DWORD, LPDWORD );

extern TPFN_FT_OpenEx					FT_OpenEx;
extern TPFN_FT_Close					FT_Close;
extern TPFN_FT_CreateDeviceInfoList		FT_CreateDeviceInfoList;
extern TPFN_FT_GetDeviceInfoList		FT_GetDeviceInfoList;
extern TPFN_FT_SetBaudRate				FT_SetBaudRate;
extern TPFN_FT_SetDataCharacteristics	FT_SetDataCharacteristics;
extern TPFN_FT_SetFlowControl			FT_SetFlowControl;
extern TPFN_FT_Write					FT_Write;
extern TPFN_FT_Read						FT_Read;

//////////////////////////////////////////////////////////////////////////

#endif//_FTD2XX_H_

