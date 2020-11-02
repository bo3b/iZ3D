/*++

Copyright (c) 2004-2006  Microsoft Corporation

Module Name:
    DmmDiag.hxx

Abstract:
    This modules contains the definition of DMM diagnosibility serialization format.

Author:

--*/

#ifndef _DMM_DIAG_H_
#define _DMM_DIAG_H_

#include <d3dkmdt.h>

#if defined(ASSERT)

    #define DMM_OPTIONAL_ASSERT(exp) ASSERT(exp)

#else

    #define DMM_OPTIONAL_ASSERT(exp)

#endif


// NOTE: Increment minor version each time format changes between builds.
//       Increment major version each time format is released publically.
//
// The first 4 bits is the major version, the remaining 12 bits are the minor version.
enum
{
    DMM_DIAG_INFO_VISTA_BETA2_VERSION = 0x1001,
    DMM_DIAG_INFO_VISTA_RC1_VERSION   = 0x1002,
    DMM_DIAG_INFO_VISTA_RTM_VERSION   = 0x1003,

    DMM_DIAG_INFO_VERSION             = DMM_DIAG_INFO_VISTA_RTM_VERSION
};


typedef struct _DMM_DIAG_INFO_INDEX
{
    // Version of diagnosibility info format (= DMM_DIAG_INFO_VERSION)
    USHORT  Version;

    // Size of entire DMM diagnosibility serialization
    ULONG  Size;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // video present target set of type DMM_VIDEOPRESENTSOURCESET_SERIALIZATION.
    ULONG  VideoPresentSourceSetSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // video present target set of type DMM_VIDEOPRESENTTARGETSET_SERIALIZATION.
    ULONG  VideoPresentTargetSetSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // active VidPN of type DMM_VIDPN_SERIALIZATION.
    ULONG  ActiveVidPnSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // recently committed VidPNs of type DMM_VIDPNSET_SERIALIZATION.
    ULONG  RecentlyCommittedVidPnsSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // connected monitors of type DMM_MONITORSET_SERIALIZATION.
    ULONG  ConnectedMonitorsSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // recently requested mode changes of type DMM_DISPMODECHANGEREQUEST_SERIALIZATION.
    ULONG  RecentlyRequestedDispModeChangesSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // recently recommended functional VidPNs of type DMM_VIDPNSET_SERIALIZATION.
    ULONG  RecentlyRecommendedFunctionalVidPnsSerializationOffset;

    // Offset (from base of this DMM_DIAG_INFO_INDEX instance) to serialization of
    // recent monitor presence events of type DMM_MONITORPRESENCEEVENTSET_SERIALIZATION.
    ULONG  RecentMonitorPresenceEventsSerializationOffset;
}
DMM_DIAG_INFO_INDEX;


typedef struct _DMM_VIDEOPRESENTSOURCE_SERIALIZATION
{
    // Video present source ID.
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  Id;

    // For keeping track of whether this source is attached to the desktop or not.
    BOOLEAN  IsAttachedToDesktop;

    // Predicate specifying whether corresponding video present source's primary is being presented.
    BOOLEAN  IsPrimaryVisible;
}
DMM_VIDEOPRESENTSOURCE_SERIALIZATION;


typedef struct _DMM_VIDEOPRESENTSOURCESET_SERIALIZATION
{
    // Number of video present targets.
    UCHAR  NumSources;

    // NOTE: Number of elements is specified by NumSources.
    //
    // Predicate array specifying whether corresponding video present source is presented.
    __field_ecount_full(NumSources) DMM_VIDEOPRESENTSOURCE_SERIALIZATION SourceSerialization[1];
}
DMM_VIDEOPRESENTSOURCESET_SERIALIZATION;


typedef struct _DMM_VIDPNPATHANDTARGETMODE_SERIALIZATION
{
    // Path info.
    D3DKMDT_VIDPN_PRESENT_PATH  PathInfo;

    // Target mode info.
    D3DKMDT_VIDPN_TARGET_MODE  TargetMode;
}
DMM_VIDPNPATHANDTARGETMODE_SERIALIZATION;


typedef struct _DMM_VIDPNPATHSFROMSOURCE_SERIALIZATION
{
    // VidPN source mode serialization.
    D3DKMDT_VIDPN_SOURCE_MODE  SourceMode;

    // Number of paths originating from this source.
    UCHAR  NumPathsFromSource;

    // NOTE: Number of elements is specified by NumPathsFromSource.
    //
    // Serializations of each path originating from source in question and corresponding target mode.
    __field_ecount_full(NumPathsFromSource) DMM_VIDPNPATHANDTARGETMODE_SERIALIZATION  PathAndTargetModeSerialization[1];
}
DMM_VIDPNPATHSFROMSOURCE_SERIALIZATION;


// NOTE: Use GetPathsFromSourceSerialization() to access paths-from-source serializations.
typedef struct _DMM_VIDPN_SERIALIZATION
{
    // Size in bytes of this VidPN serialization.
    ULONG  Size;

    // Number of active sources in the serialized VidPN.
    UCHAR  NumActiveSources;

    // NOTE: Number of elements is specified by NumActiveSources.
    //
    // Offsets (from base of this DMM_VIDPN_SERIALIZATION instance) to serialization of
    // paths-from-source info of type DMM_VIDPNPATHSFROMSOURCE_SERIALIZATION.
    __field_ecount_full(NumActiveSources) ULONG  PathsFromSourceSerializationOffsets[1];
}
DMM_VIDPN_SERIALIZATION;

typedef struct _DMM_VIDPNSET_SERIALIZATION
{
    // Number of VidPNs.
    UCHAR  NumVidPns;

    // Offset (from base of this DMM_VIDPNSET_SERIALIZATION instance) to serialization
    // of VidPN instance of type DMM_VIDPN_SERIALIZATION.
    __field_ecount_full(NumVidPns) ULONG  VidPnOffset[1];
}
DMM_VIDPNSET_SERIALIZATION;


__inline
DMM_VIDPNPATHSFROMSOURCE_SERIALIZATION*
GetPathsFromSourceSerialization
(
  __in_bcount(sizeof(DMM_VIDPN_SERIALIZATION)) const DMM_VIDPN_SERIALIZATION* const  i_pVidPnSerialization,
  __in_range(0, i_pVidPnSerialization->NumActiveSources - 1) const UCHAR             i_ucActiveSourceIndex
)
{
    ULONG  ulPathsFromSourceSerializationOffset;

    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pVidPnSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_ucActiveSourceIndex < i_pVidPnSerialization->NumActiveSources );
    }

    ulPathsFromSourceSerializationOffset = i_pVidPnSerialization->PathsFromSourceSerializationOffsets[i_ucActiveSourceIndex];

    // State validation (checked-only).
    DMM_OPTIONAL_ASSERT( ulPathsFromSourceSerializationOffset != 0 );

    return (DMM_VIDPNPATHSFROMSOURCE_SERIALIZATION*)((UCHAR*)i_pVidPnSerialization + ulPathsFromSourceSerializationOffset);
}

__inline
DMM_VIDPN_SERIALIZATION*
GetVidPnSerialization
(
  __in_bcount(sizeof(DMM_VIDPNSET_SERIALIZATION)) const DMM_VIDPNSET_SERIALIZATION *const  i_pVidPnSetSerialization,
  __in_range(0, i_pVidPnSetSerialization->NumVidPns - 1) const UCHAR                       i_ucVidPnIndex
)
{
    ULONG  ulVidPnOffset;

    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pVidPnSetSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_ucVidPnIndex < i_pVidPnSetSerialization->NumVidPns );
    }

    ulVidPnOffset = i_pVidPnSetSerialization->VidPnOffset[i_ucVidPnIndex];

    // State validation (checked-only).
    DMM_OPTIONAL_ASSERT( ulVidPnOffset != 0 );

    return (DMM_VIDPN_SERIALIZATION*)((UCHAR*)i_pVidPnSetSerialization + ulVidPnOffset);
}


typedef enum _DMM_VIDEO_PRESENT_TARGET_OWNERSHIP
{
    DMM_VPTO_UNINITIALIZED = 0,
    DMM_VPTO_SHARED        = 1,  // target is available for use by any DMM client
    DMM_VPTO_EXCLUSIVE     = 2   // target is owned by DxgPort
}
DMM_VIDEO_PRESENT_TARGET_OWNERSHIP;


typedef struct _DMM_VIDEOPRESENTTARGET_SERIALIZATION
{
    // Target info.
    D3DKMDT_VIDEO_PRESENT_TARGET  Info;

    // Type of ownership this target is under.
    DMM_VIDEO_PRESENT_TARGET_OWNERSHIP  Ownership;

} DMM_VIDEOPRESENTTARGET_SERIALIZATION;


typedef struct _DMM_VIDEOPRESENTTARGETSET_SERIALIZATION
{
    // Number of video present targets.
    UCHAR  NumTargets;

    // NOTE: Number of elements is specified by NumTargets.
    //
    // Serialization of each video present target
    __field_ecount_full(NumTargets) DMM_VIDEOPRESENTTARGET_SERIALIZATION  TargetSerialization[1];

} DMM_VIDEOPRESENTTARGETSET_SERIALIZATION;


typedef struct _DMM_MONITOR_SOURCE_MODE_SERIALIZATION
{
    // Mode info.
    D3DKMDT_MONITOR_SOURCE_MODE  Info;

    // Type of timings (e.g. established, standard, detailed, etc.).
    D3DKMDT_MONITOR_TIMING_TYPE  TimingType;
}
DMM_MONITOR_SOURCE_MODE_SERIALIZATION;


typedef struct _DMM_MONITORSOURCEMODESET_SERIALIZATION
{
    // Number of monitor source modes.
    UCHAR  NumModes;

    // NOTE: Number of elements is specified by NumModes.
    //
    // Serializations of each mode.
    __field_ecount_full(NumModes) DMM_MONITOR_SOURCE_MODE_SERIALIZATION  ModeSerialization[1];
}
DMM_MONITORSOURCEMODESET_SERIALIZATION;


typedef struct _DMM_MONITORFREQUENCYRANGESET_SERIALIZATION
{
    // Number of monitor frequency ranges.
    UCHAR  NumFrequencyRanges;

    // NOTE: Number of elements is specified by NumFrequencyRanges.
    //
    // Serializations of each frequency range.
    __field_ecount_full(NumFrequencyRanges) D3DKMDT_MONITOR_FREQUENCY_RANGE  FrequencyRangeSerialization[1];
}
DMM_MONITORFREQUENCYRANGESET_SERIALIZATION;


// LIMITATION: Only 128-byte descriptors are supported.
typedef struct _DMM_MONITORDESCRIPTOR_SERIALIZATION
{
    // Descriptor's ID.
    D3DKMDT_MONITOR_DESCRIPTOR_ID  Id;

    // Descriptor's type.
    D3DKMDT_MONITOR_DESCRIPTOR_TYPE  Type;

    // Origins of monitor descriptor data.
    D3DKMDT_MONITOR_CAPABILITIES_ORIGIN  Origin;

    // Descriptor data.
    UCHAR Data[128];
}
DMM_MONITORDESCRIPTOR_SERIALIZATION;


typedef struct _DMM_MONITORDESCRIPTORSET_SERIALIZATION
{
    // Number of monitor descriptors.
    UCHAR  NumDescriptors;

    // NOTE: Number of elements is specified by NumDescriptors.
    //
    // Serializations of each descriptor.
    __field_ecount_full(NumDescriptors) DMM_MONITORDESCRIPTOR_SERIALIZATION  DescriptorSerialization[1];
}
DMM_MONITORDESCRIPTORSET_SERIALIZATION;


typedef enum _DMM_MODE_PRUNING_ALGORITHM
{
    DMM_MPA_UNINITIALIZED   = 0,
    DMM_MPA_GDI             = 1,
    DMM_MPA_VISTA           = 2,
    DMM_MPA_GDI_VISTA_UNION = 3,
    DMM_MPA_MAXVALID        = 3
}
DMM_MODE_PRUNING_ALGORITHM;


typedef struct _DMM_MONITOR_SERIALIZATION
{
    // Size in bytes of this monitor serialization.
    ULONG  Size;

    // ID of the video present target to which this monitor is connected.
    D3DDDI_VIDEO_PRESENT_TARGET_ID  VideoPresentTargetId;

    // Monitor's orientation.
    D3DKMDT_MONITOR_ORIENTATION  Orientation;

    // Predicate specifying whether monitor is simulated.
    BOOLEAN  IsSimulatedMonitor;

    // Predicate specifying whether default monitor profile caps are assumed for this monitor.
    BOOLEAN  IsUsingDefaultProfile;

    // Mode pruning algorithm used for this monitor.
    DMM_MODE_PRUNING_ALGORITHM  ModePruningAlgorithm;

    // Monitor's device power state.
    DEVICE_POWER_STATE  MonitorPowerState;

    // Offset (from base of this MONITOR_SERIALIZATION instance) to serialization of
    // monitor source mode set of type DMM_MONITORSOURCEMODESET_SERIALIZATION.
    ULONG  SourceModeSetOffset;

    // Offset (from base of this MONITOR_SERIALIZATION instance) to serialization of
    // monitor source mode set of type DMM_MONITORFREQUENCYRANGESET_SERIALIZATION.
    ULONG  FrequencyRangeSetOffset;

    // Offset (from base of this MONITOR_SERIALIZATION instance) to serialization of
    // monitor source mode set of type DMM_MONITORDESCRIPTORSET_SERIALIZATION.
    ULONG  DescriptorSetOffset;
}
DMM_MONITOR_SERIALIZATION;


typedef struct _DMM_MONITORSET_SERIALIZATION
{
    // Number of monitors.
    UCHAR  NumMonitors;

    // Offset (from base of this DMM_MONITORSET_SERIALIZATION instance) to serialization
    // of monitor instance of type DMM_MONITOR_SERIALIZATION.
    __field_ecount_full(NumMonitors) ULONG  MonitorOffset[1];
}
DMM_MONITORSET_SERIALIZATION;


typedef enum _DMM_MONITOR_PRESENCE_EVENT_TYPE
{
    DMM_MPET_UNINITIALIZED         = 0,
    DMM_MPET_ADDMONITOR            = 1,
    DMM_MPET_REMOVEMONITOR         = 2,
    DMM_MPET_DRIVERARRIVAL         = 3,
    DMM_MPET_DRIVERQUERYREMOVE     = 4,
    DMM_MPET_DRIVERREMOVECANCELLED = 5,
    DMM_MPET_DRIVERREMOVECOMPLETE  = 6,
    DMM_MPET_MAXVALID = DMM_MPET_DRIVERREMOVECOMPLETE
}
DMM_MONITOR_PRESENCE_EVENT_TYPE;


typedef struct _DMM_MONITORPRESENCEEVENT_SERIALIZATION
{
    DMM_MONITOR_PRESENCE_EVENT_TYPE  EventType;

    // ID of the video present target on which the arrival/departure happened.
    D3DDDI_VIDEO_PRESENT_TARGET_ID  VideoPresentTargetId;

    // Unique sequential identifier of the event.
    ULONG Id;

    // Event processing status
    NTSTATUS Status;

    // Time since boot in 100-nanoseconds.
    ULONG64 TimeSinceBoot;
}
DMM_MONITORPRESENCEEVENT_SERIALIZATION;


typedef struct _DMM_MONITORPRESENCEEVENTSET_SERIALIZATION
{
    // Number of monitor descriptors.
    UCHAR  NumEvents;

    // NOTE: Number of elements is specified by NumEvents.
    //
    // Serializations of each descriptor.
    __field_ecount_full(NumEvents) DMM_MONITORPRESENCEEVENT_SERIALIZATION  EventSerialization[1];
}
DMM_MONITORPRESENCEEVENTSET_SERIALIZATION;


__inline
DMM_MONITORSOURCEMODESET_SERIALIZATION*
GetMonitorSourceModeSetSerialization
(
  __in_bcount(sizeof(DMM_MONITOR_SERIALIZATION)) const DMM_MONITOR_SERIALIZATION *const  i_pMonitorSerialization
)
{
    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization->SourceModeSetOffset != 0 );
    }

    return (DMM_MONITORSOURCEMODESET_SERIALIZATION*)((UCHAR*)i_pMonitorSerialization
                                                     +
                                                     i_pMonitorSerialization->SourceModeSetOffset);
}

__inline
DMM_MONITORFREQUENCYRANGESET_SERIALIZATION*
GetMonitorFrequencyRangeSetSerialization
(
  __in_bcount(sizeof(DMM_MONITOR_SERIALIZATION)) const DMM_MONITOR_SERIALIZATION *const  i_pMonitorSerialization
)
{
    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization->FrequencyRangeSetOffset != 0 );
    }

    return (DMM_MONITORFREQUENCYRANGESET_SERIALIZATION*)((UCHAR*)i_pMonitorSerialization
                                                         +
                                                         i_pMonitorSerialization->FrequencyRangeSetOffset);
}

__inline
DMM_MONITORDESCRIPTORSET_SERIALIZATION*
GetMonitorDescriptorSetSerialization
(
  __in_bcount(sizeof(DMM_MONITOR_SERIALIZATION)) const DMM_MONITOR_SERIALIZATION *const  i_pMonitorSerialization
)
{
    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_pMonitorSerialization->DescriptorSetOffset != 0 );
    }

    return (DMM_MONITORDESCRIPTORSET_SERIALIZATION*)((UCHAR*)i_pMonitorSerialization
                                                     +
                                                     i_pMonitorSerialization->DescriptorSetOffset);
}


typedef enum _DMM_CLIENT_TYPE
{
    DMM_CT_UNINITIALIZED  = 0,
    DMM_CT_CDD_NOPATHDATA = 1,  // legacy GDI mode change that does not contain VidPN path data
    DMM_CT_USERMODE       = 2,  // mode change initiated via the SetDisplayMode user-mode thunk                      (LDDM specific)
    DMM_CT_CDD_PATHDATA   = 3,  // kernel-mode initiated mode change that specifies VidPN path data                  (LDDM-specific)
    DMM_CT_DXGPORT        = 4   // mode change initiated by Port via Dmm{Add|Remove}PresentPath{To|From}ActiveVidPn  (LDDM-specific)
}
DMM_CLIENT_TYPE;


typedef enum _DMM_VIDPNCHANGE_TYPE
{
    DMM_CVR_UNINITIALIZED  = 0,
    DMM_CVR_UPDATEMODALITY = 1, // no topology changes - only path modality is updated on the affected source.
    DMM_CVR_ADDPATH        = 2, // a single path is added to affected source.
    DMM_CVR_ADDPATHS       = 3, // multiple paths are added to affected source.
    DMM_CVR_REMOVEPATH     = 4, // a single (could be the only) path is removed from the affected source.
    DMM_CVR_REMOVEALLPATHS = 5  // all paths are removed from affected source.
}
DMM_VIDPNCHANGE_TYPE;


typedef struct _DMM_COMMITVIDPNREQUEST_DIAGINFO
{
    // Type of the client that initiated the commit VidPN in question.
    DMM_CLIENT_TYPE  ClientType : 4;

    // Type of the change performed on the active VidPN.
    DMM_VIDPNCHANGE_TYPE  VidPnChange : 4;

    // Predicate specifying whether commit VidPN in question is to reclaim a cloned target.
    unsigned char  ReclaimClonedTarget : 1;

    // Predicate specifying whether commit VidPN in question is cleaning up after a failed attempt to commit another VidPN.
    unsigned char  CleanupAfterFailedCommitVidPn : 1;
}
DMM_COMMITVIDPNREQUEST_DIAGINFO;


typedef struct _DMM_COMMITVIDPNREQUEST_SERIALIZATION
{
    // ID of the source affected by the VidPN requested to be committed.
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  AffectedVidPnSourceId;

    // Type of the scenario for which client requested to commit the VidPN in question.
    DMM_COMMITVIDPNREQUEST_DIAGINFO  RequestDiagInfo;

    // Copy of the VidPN that was requested to be committed.
    DMM_VIDPN_SERIALIZATION  VidPnSerialization;
}
DMM_COMMITVIDPNREQUEST_SERIALIZATION;


typedef struct _DMM_COMMITVIDPNREQUESTSET_SERIALIZATION
{
    // Number of commit VidPN requests
    UCHAR  NumCommitVidPnRequests;

    // Offset (from base of this DMM_COMMITVIDPNREQUESTSET_SERIALIZATION instance) to serialization
    // of VidPN instance of type DMM_COMMITVIDPNREQUEST_SERIALIZATION.
    __field_ecount_full(NumCommitVidPnRequests) ULONG  CommitVidPnRequestOffset[1];
}
DMM_COMMITVIDPNREQUESTSET_SERIALIZATION;

__inline
DMM_COMMITVIDPNREQUEST_SERIALIZATION*
GetCommitVidPnRequestSerialization
(
  __in_bcount(sizeof(DMM_COMMITVIDPNREQUESTSET_SERIALIZATION)) const DMM_COMMITVIDPNREQUESTSET_SERIALIZATION *const  i_pCommitVidPnRequestSetSerialization,
  __in_range(0, i_pCommitVidPnRequestSetSerialization->NumCommitVidPnRequests- 1) const UCHAR                        i_ucCommitVidPnRequestIndex
)
{
    ULONG  ulVidPnOffset;

    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pCommitVidPnRequestSetSerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_ucCommitVidPnRequestIndex < i_pCommitVidPnRequestSetSerialization->NumCommitVidPnRequests);
    }

    ulVidPnOffset = i_pCommitVidPnRequestSetSerialization->CommitVidPnRequestOffset[i_ucCommitVidPnRequestIndex];

    // State validation (checked-only).
    DMM_OPTIONAL_ASSERT( ulVidPnOffset != 0 );

    return (DMM_COMMITVIDPNREQUEST_SERIALIZATION*)((UCHAR*)i_pCommitVidPnRequestSetSerialization + ulVidPnOffset);
}

#define DMM_DISPMODECHANGEREQUEST_PROCESS_NAME_SIZE 16

typedef struct _DMM_DISPMODECHANGEREQUEST_SERIALIZATION
{
    // ID of the video present source on which the mode change has been requested.
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  VideoPresentSourceId;

    // Display mode info.
    D3DKMT_DISPLAYMODE  ModeInfo;

    // Type of DMM client that requested the mode change.
    DMM_CLIENT_TYPE  ClientType;

    // Status of mode change request execution.
    NTSTATUS  ModeChangeStatus;

    // Unique identifier of the mode change request.
    ULONG Id;

    // Time since boot in 100-nanoseconds.
    ULONG64 TimeSinceBoot;

    // Requestor process name
    UCHAR ProcessName[DMM_DISPMODECHANGEREQUEST_PROCESS_NAME_SIZE];
}
DMM_DISPMODECHANGEREQUEST_SERIALIZATION;


typedef struct _DMM_DISPMODECHANGEREQUESTSET_SERIALIZATION
{
    // Number of display mode change requests
    UCHAR  NumDispModeChangeRequests;

    // NOTE: Number of elements is specified by NumDispModeChangeRequests.
    //
    // Serializations of each display mode change request.
    __field_ecount_full(NumDispModeChangeRequests) DMM_DISPMODECHANGEREQUEST_SERIALIZATION  DispModeChangeRequestSerialization[1];
}
DMM_DISPMODECHANGEREQUESTSET_SERIALIZATION;


typedef struct _DMM_VIDPNSOURCEMODESET_SERIALIZATION
{
    // Number of VidPN source modes.
    UCHAR  NumModes;

    // NOTE: Number of elements is specified by NumModes.
    //
    // Serializations of each display mode change request.
    D3DKMDT_VIDPN_SOURCE_MODE  ModeSerialization[1];
}
DMM_VIDPNSOURCEMODESET_SERIALIZATION;


typedef struct _DMM_VIDPNTARGETMODESET_SERIALIZATION
{
    // Number of VidPN target modes.
    UCHAR  NumModes;

    // NOTE: Number of elements is specified by NumModes.
    //
    // Serializations of each display mode change request.
    D3DKMDT_VIDPN_TARGET_MODE  ModeSerialization[1];
}
DMM_VIDPNTARGETMODESET_SERIALIZATION;


typedef struct _DMM_VIDPNPATHANDTARGETMODESET_SERIALIZATION
{
    // Path info.
    D3DKMDT_VIDPN_PRESENT_PATH  PathInfo;

    // Target mode info.
    DMM_VIDPNTARGETMODESET_SERIALIZATION  TargetModeSet;
}
DMM_VIDPNPATHANDTARGETMODESET_SERIALIZATION;


typedef struct _DMM_COFUNCPATHSMODALITY_SERIALIZATION
{
    // Number of paths originating from this source.
    UCHAR  NumPathsFromSource;

    // NOTE: Number of elements is specified by NumPathsFromSource.
    //
    // Offsets (from base of this DMM_COFUNCPATHSMODALITY_SERIALIZATION instance) to serialization of
    // path-and-target-mode-set info of type DMM_VIDPNPATHANDTARGETMODESET_SERIALIZATION.
    __field_ecount_full(NumPathsFromSource) ULONG  PathAndTargetModeSetOffset[1];
}
DMM_COFUNCPATHSMODALITY_SERIALIZATION;


__inline
DMM_VIDPNPATHANDTARGETMODESET_SERIALIZATION*
GetCofuncModalityOfPathFromSourceSerialization
(
  __in_bcount(sizeof(DMM_COFUNCPATHSMODALITY_SERIALIZATION)) const DMM_COFUNCPATHSMODALITY_SERIALIZATION* const  i_pCofuncPathModalitySerialization,
  __in_range(0, i_pCofuncPathModalitySerialization->NumPathsFromSource - 1) const UCHAR                          i_ucPathFromSourceIndex
)
{
    ULONG  ulCofuncModalityOfPathsFromSourceSerializationOffset;

    // Parameter validation (checked-only).
    {
        DMM_OPTIONAL_ASSERT( i_pCofuncPathModalitySerialization != NULL );
        DMM_OPTIONAL_ASSERT( i_ucPathFromSourceIndex < i_pCofuncPathModalitySerialization->NumPathsFromSource );
    }

    ulCofuncModalityOfPathsFromSourceSerializationOffset = i_pCofuncPathModalitySerialization->PathAndTargetModeSetOffset[i_ucPathFromSourceIndex];

    // State validation (checked-only).
    DMM_OPTIONAL_ASSERT( ulCofuncModalityOfPathsFromSourceSerializationOffset != 0 );

    return (DMM_VIDPNPATHANDTARGETMODESET_SERIALIZATION*)((UCHAR*)i_pCofuncPathModalitySerialization + ulCofuncModalityOfPathsFromSourceSerializationOffset);
}

#endif // _DMM_DIAG_H_


