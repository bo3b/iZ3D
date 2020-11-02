/****************************************************************************\
Copyright (c) 2002-2007 NVIDIA Corporation.

NVIDIA Corporation("NVIDIA") supplies this software to you in
consideration of your agreement to the following terms, and your use,
installation, modification or redistribution of this NVIDIA software
constitutes acceptance of these terms.  If you do not agree with these
terms, please do not use, install, modify or redistribute this NVIDIA
software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, NVIDIA grants you a personal, non-exclusive
license, under NVIDIA's copyrights in this original NVIDIA software (the
"NVIDIA Software"), to use, reproduce, modify and redistribute the
NVIDIA Software, with or without modifications, in source and/or binary
forms; provided that if you redistribute the NVIDIA Software, you must
retain the copyright notice of NVIDIA, this notice and the following
text and disclaimers in all such redistributions of the NVIDIA Software.
Neither the name, trademarks, service marks nor logos of NVIDIA
Corporation may be used to endorse or promote products derived from the
NVIDIA Software without specific prior written permission from NVIDIA.
Except as expressly stated in this notice, no other rights or licenses
express or implied, are granted by NVIDIA herein, including but not
limited to any patent rights that may be infringed by your derivative
works or by other works in which the NVIDIA Software may be
incorporated. No hardware is licensed hereunder. 

THE NVIDIA SOFTWARE IS BEING PROVIDED ON AN "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED,
INCLUDING WITHOUT LIMITATION, WARRANTIES OR CONDITIONS OF TITLE,
NON-INFRINGEMENT, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
ITS USE AND OPERATION EITHER ALONE OR IN COMBINATION WITH OTHER
PRODUCTS.

IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT,
INCIDENTAL, EXEMPLARY, CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, LOST PROFITS; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) OR ARISING IN ANY WAY
OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE
NVIDIA SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT,
TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\****************************************************************************/

#ifndef _NVPMAPI_INT_H_
#define _NVPMAPI_INT_H_

#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
//                   NVIDIA Performance Monitoring (PM) API,
//          Interface to NVIDIA Graphics Performance Data Gathering
//          -------------------------------------------------------
//-----------------------------------------------------------------------------

typedef unsigned int UINT;
typedef unsigned long long UINT64;

#define NVPM_INFINITE_VAL  0xFFFFFFFFFFFFFFFF

// The negative result values are thrown on init or if init failed
#ifndef NVPMRESULT_DEFINED
typedef enum {
  NVPM_FAILURE_DISABLED = -5,        // Performance disabled in registry
  NVPM_FAILURE_32BIT_ON_64BIT = -4,  // Mixed mode (32bit client 64bit kernel) unsupported
  NVPM_NO_IMPLEMENTATION = -3,       // Returned when NVPMInit has not been called or failed
  NVPM_LIBRARY_NOT_FOUND = -2,       // nvpmapi.dll was not found
  NVPM_FAILURE = -1,                 // General, internal failure when initializing 
  NVPM_OK = 0,
  NVPM_ERROR_INVALID_PARAMETER,
  NVPM_ERROR_DRIVER_MISMATCH,
  NVPM_ERROR_NOT_INITIALIZED,
  NVPM_ERROR_ALREADY_INITIALIZED,
  NVPM_ERROR_BAD_ENUMERATOR,
  NVPM_ERROR_STRING_TOO_SMALL,
  NVPM_ERROR_INVALID_COUNTER,
  NVPM_ERROR_OUT_OF_MEMORY,
  NVPM_ERROR_NOT_IN_EXPERIMENT,
  NVPM_ERROR_EXPERIMENT_INCOMPLETE,
  NVPM_ERROR_INVALID_PASS,
  NVPM_ERROR_NOT_IN_PASS,
  NVPM_ERROR_IN_PASS,
  NVPM_ERROR_NOT_IN_OBJECT,
  NVPM_ERROR_IN_OBJECT,
  NVPM_ERROR_INVALID_OBJECT,
  NVPM_ERROR_COUNTER_NOT_ENABLED,
  NVPM_ERROR_COUNTER_NOT_FOUND,
  NVPM_ERROR_EXPERIMENT_NOT_RUN,
  NVPM_ERROR_EXPERIMENT_RUNNING,
  NVPM_ERROR_32BIT_ON_64BIT,
  NVPM_ERROR_INTERNAL,
} NVPMRESULT;
#define NVPMRESULT_DEFINED
#endif

#ifdef NVPMAPI_INTERFACE 
#undef NVPMAPI_INTERFACE 
#endif

#ifdef _WIN32
#define NVPMAPI_INTERFACE extern NVPMRESULT __cdecl 
#else
#define NVPMAPI_INTERFACE extern NVPMRESULT
#endif

// Initialize the NVPMAPI system
NVPMAPI_INTERFACE NVPMInit();

NVPMAPI_INTERFACE NVPMSetWarningLevel(UINT unLevel);

// Shutdown NVPMAPI and allow it to perform any necessary resource cleanup
NVPMAPI_INTERFACE NVPMShutdown();

// Enumerate available counters/experiments
typedef int NVPMEnumFunc(UINT unCounterIndex, char *pcCounterName);
NVPMAPI_INTERFACE NVPMEnumCounters(NVPMEnumFunc pEnumFunction);
    
// Retrieve the number of counters available 
NVPMAPI_INTERFACE NVPMGetNumCounters(UINT *punCount);
    
// Retrieve name or description of a counter.  Passing NULL for pcString 
// and a valid pointer for punLen will return the length of the 
// name/description in punLen.  Passing a pointer in pcString and a buffer 
// size in punLen will attempt to write the name (\0 term) to pcString.  
// If the buffer is too small, nothing is written and punLen is set to the 
// string length needed.
NVPMAPI_INTERFACE NVPMGetCounterName(UINT unCounterIndex, char *pcString, UINT *punLen);
NVPMAPI_INTERFACE NVPMGetCounterDescription(UINT unCounterIndex, char *pcString, UINT *punLen);
NVPMAPI_INTERFACE NVPMGetCounterIndex(char *pcString, UINT *punCounterIndex);
NVPMAPI_INTERFACE NVPMGetCounterClockRate(char *pcString, float *pfValue); // Return in MHz
      
// Query attribute information for a given counter.  These can be called 
// when enumerating with NVPMEnumCounters();
typedef enum {NVPM_CT_GPU, NVPM_CT_OGL, NVPM_CT_D3D, NVPM_CT_SIMEXP} NVPMCOUNTERTYPE;
typedef enum {NVPM_CV_PERCENT, NVPM_CV_RAW} NVPMCOUNTERVALUE;
typedef enum {NVPM_CDH_PERCENT, NVPM_CDH_RAW} NVPMCOUNTERDISPLAYHINT;
typedef enum {
  NVPMA_COUNTER_TYPE, // Return the type of the counter (NVPMCOUNTERTYPE)
  NVPMA_COUNTER_VALUE, // Return the counter value type (NVPMCOUNTERVALUE)
  NVPMA_COUNTER_DISPLAY_HINT, // Give a hint to how the counter should be displayed.  
  NVPMA_COUNTER_MAX,  // Return the maximum counter value
} NVPMATTRIBUTE;

NVPMAPI_INTERFACE NVPMGetCounterAttribute(UINT unCounterIndex, NVPMATTRIBUTE nvAttribute, UINT64 *punValue);

// Add counters interface for enabling either a single counter or a compound experiment.  
NVPMAPI_INTERFACE NVPMAddCounterByName(char *pcName);
NVPMAPI_INTERFACE NVPMAddCounter(UINT unCounterIndex);
NVPMAPI_INTERFACE NVPMAddCounters(UINT unCount, UINT *punCounterIndices);

// Remove counters interface for disabling either a single counter or a compound experiment.  
NVPMAPI_INTERFACE NVPMRemoveCounterByName(char *pcName);
NVPMAPI_INTERFACE NVPMRemoveCounter(UINT unCounterIndex);
NVPMAPI_INTERFACE NVPMRemoveCounters(UINT unCount, UINT *punCounterIndices);
NVPMAPI_INTERFACE NVPMRemoveAllCounters();
  
// Allocate slots for counter data to be put into.  If this isn't done, all data is put in "slot 0".  
NVPMAPI_INTERFACE NVPMAllocObjects(int);
NVPMAPI_INTERFACE NVPMDeleteObjects();

///////////////////////////////////////////////////////////////////////////////
// This is the experiment based interface for NVPMAPI

// Signals to NVPMAPI that the user is ready to begin sampling.  It returns in 
// pnNumPasses the number of passes it will take to provide data for all of 
// the enabled counters.
NVPMAPI_INTERFACE NVPMBeginExperiment(int *pnNumPasses);
  
// Lets NVPMAPI know that you are done with an experiment.  Bookend for 
// NVPMBeginExperiment and handles dangling passes/objects.
NVPMAPI_INTERFACE NVPMEndExperiment();

// Signifies the beginning of a pass.  
NVPMAPI_INTERFACE NVPMBeginPass(int nPass);

// Lets NVPMAPI know that you are done with a pass.  Bookend for 
// NVPMBeginPass and handles dangling objects.
NVPMAPI_INTERFACE NVPMEndPass(int nPass);

// Begin/end an object in a pass.
NVPMAPI_INTERFACE NVPMBeginObject(int nObjectID);
NVPMAPI_INTERFACE NVPMEndObject(int nObjectID);

///////////////////////////////////////////////////////////////////////////////
// This is the typical "sample now" based interface.  If you pass in an array 
// of SampleValue's, it will return the currently active counters (NULL returns
// no counters).  Fill *punCount with the available entries in pucValues/pucCycles
typedef struct _NVPMSampleValue {
  UINT unCounterIndex;
  UINT64 ulValue;
  UINT64 ulCycles;
} NVPMSampleValue;
NVPMAPI_INTERFACE NVPMSample(NVPMSampleValue *pSamples, UINT *punCount);

typedef struct _NVPMSampleValueEx {
  UINT unCounterIndex;
  UINT64 ulValue;
  UINT64 ulCycles;
  UINT unCounterValueUpdated;
} NVPMSampleValueEx;

#define MAKEVERSION(MAJ, MIN, STRUCT) ((MAJ << 24) | (MIN << 16) | sizeof(STRUCT))

#define NVPMSAMPLEVALUEEX_MAJVER 1
#define NVPMSAMPLEVALUEEX_MINVER 1
#define NVPMSAMPLEVALUEEX_VERSION() MAKEVERSION(NVPMSAMPLEVALUEEXMAJVER, NVPMSAMPLEVALUEEXMINVER, NVPMSampleValueEx)
NVPMAPI_INTERFACE NVPMSampleEx(NVPMSampleValueEx *pSamples, UINT *punCount, UINT unNVPMSampleValueExVersion);

///////////////////////////////////////////////////////////////////////////////
// Retrieve the resulting counter values
NVPMAPI_INTERFACE NVPMGetCounterValueByName(char *pcName, int nObjectID, UINT64 *pulValue, UINT64 *pulCycles);
NVPMAPI_INTERFACE NVPMGetCounterValue(UINT unCounterIndex, int nObjectID, UINT64 *pulValue, UINT64 *pulCycles);
NVPMAPI_INTERFACE NVPMGetGPUBottleneckName(UINT64 ulValue, char *pcName);
        
// If NVPM_ERROR_INTERNAL is returned, this will give extended information to help debug what happened
// Send this result (along with any other pertinent contextual information) to NVPerfKit@nvidia.com
NVPMAPI_INTERFACE NVPMGetExtendedError(UINT *pnError);

#ifdef __cplusplus
};
#endif

#endif // _NVPMAPI_INT_H_
