#ifndef _ATIDX9STEREO_H_
#define _ATIDX9STEREO_H_

#define ATI_STEREO_VERSION_MAJOR 0
#define ATI_STEREO_VERSION_MINOR 3

// @todo Need to handle packing to guarantee correct layouts

// This include file contains all the DX9 Stereo Display extension definitions
// (structures, enums, constants) shared between the driver and the application

#pragma pack(push, 4)

#define FOURCC_AQBS        MAKEFOURCC('A','Q','B','S')

// SetSrcEye/SetDstEye Parameters
#define ATI_STEREO_LEFTEYE   0
#define ATI_STEREO_RIGHTEYE  1

// This is the enum for all the commands that can be sent to the driver in the
// surface communication packet

typedef enum _ATIDX9STEREOCOMMAND
{
    ATI_STEREO_GETVERSIONDATA            = 0,    // Return version data structure
    ATI_STEREO_ENABLESTEREO              = 1,    // Enable stereo
    ATI_STEREO_ENABLELEFTONLY            = 2,    // Enable stereo but only display left eye
    ATI_STEREO_ENABLERIGHTONLY           = 3,    // Enable stereo but only display right eye
    ATI_STEREO_ENABLESTEREOSWAPPED       = 4,    // Enable stereo but swap left and right eyes
    ATI_STEREO_GETLINEOFFSET             = 5,    // Return the line offset from end of left eye to beginning of right eye.
    ATI_STEREO_GETDISPLAYMODES           = 6,    // Return an array of all the supported stereo display modes in a TBD format
    ATI_STEREO_SETSRCEYE                 = 7,    // Sets the source eye for blts and surface copies (left/right eye specified in dwParams)
    ATI_STEREO_SETDSTEYE                 = 8,    // Sets the destination eye for blts and surface copies (left/right eye specified in dwParams)
    ATI_STEREO_ENABLEPERSURFAA           = 9,    // Create independent AA buffers for all multi-sample render targets (excluding the flip chain)
    ATI_STEREO_ENABLEPRIMARYAA           = 10,   // Enable AA for primaries when multi-sample fields in present params are set and stereo is enabled.
    ATI_STEREO_COMMANDMAX                = 11,   // Largest command enum.
    ATI_STEREO_FORCEDWORD                = 0xffffffff
} ATIDX9STEREOCOMMAND;

// Note: The following API calls are affected by SETSRCEYE and SETDSTEYE: Clear, StretchRect, GetBackBuffer, GetFrontBufferData, UpdateSurface


// Structure used to send commands and get data from the driver through the
// FOURCC_AQBS surface.  When a FOURCC_AQBS surface is created and locked,
// a pointer to this structure is returned.  If properly filled in, it will
// process the appropriate command when the surface is unlocked

typedef struct _ATIDX9STEREOCOMMPACKET
{
    DWORD               dwSignature;        // Signature to indicate to the driver that the app is sending a command
    DWORD               dwSize;             // Size of this structure.  Passed to the app on lock
    ATIDX9STEREOCOMMAND stereoCommand;      // Command given to the driver
    HRESULT             *pResult;           // Pointer to buffer where error code will be written to. D3D_OK if successful
    DWORD               dwOutBufferSize;    // Size of optional buffer to place outgoing data into (in bytes).  Must be set if data is returned
    BYTE                *pOutBuffer;        // Pointer to buffer for outgoing data. (lineoffset, displaymodes, etc)
    DWORD               dwInBufferSize;     // Size of optional buffer to place incoming parameters
    BYTE                *pInBuffer;         // Pointer to buffer for incoming data (SetSrcEye, SetDstEye, etc)
} ATIDX9STEREOCOMMPACKET;


typedef struct _ATIDX9STEREOVERSION
{
    DWORD   dwSize;             // Size of this structure
    DWORD   dwVersionMajor;     // Major Version
    DWORD   dwVersionMinor;     // Minor Version
    DWORD   dwMaxCommand;       // Max command enum
    DWORD   dwCaps;             // Stereo Caps (not implemented yet)
    DWORD   dwReserved[11];
} ATIDX9STEREOVERSION;

#pragma pack(pop)

#endif // _ATIDX9STEREO_H_