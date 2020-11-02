// Copyright (c) Microsoft Corporation.  All rights reserved.

//Copyright (c) Microsoft Corporation.  All rights reserved.
using System;
using System.Collections.Generic;
using System.Text;
using System.Security;
using System.Runtime.InteropServices;
using Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders;
//using System.Windows.Media.Imaging;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    /// <summary>
    /// Internal class that contains interop declarations for 
    /// functions that are considered benign but that
    /// are performance critical. 
    /// </summary>
    /// <remarks>
    /// Functions that are benign but not performance critical 
    /// should be located in the NativeMethods class.
    /// </remarks>
    [SuppressUnmanagedCodeSecurity]
    internal static class SafeNativeMethods
    {

        #region General Declarations

        // Various helper constants
        internal static IntPtr NO_PARENT = IntPtr.Zero;

        // Various important window messages
        internal const int WM_USER = 0x0400;
        internal const int WM_ENTERIDLE = 0x0121;

        // FormatMessage constants and structs.
        internal const int FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;

        // App recovery and restart return codes
        internal const uint ResultFailed = 0x80004005;
        internal const uint ResultInvalidArgument = 0x80070057;
        internal const uint ResultFalse = 1;
        internal const uint ResultNotFound = 0x80070490;


        [DllImport(ExternDll.Kernel32, CharSet = CharSet.Auto)]
        internal static extern int FormatMessage(
                int flags,
                IntPtr source,
                int messageId,
                int languageId,
                [MarshalAs(UnmanagedType.LPWStr)] StringBuilder buffer,
                int size,
                IntPtr args);

        #endregion

        #region GDI and DWM Declarations

        [StructLayout(LayoutKind.Sequential)]
        public struct SIZE
        {
            public int cx;
            public int cy;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            internal int left;
            internal int top;
            internal int right;
            internal int bottom;
        };

        [StructLayout(LayoutKind.Sequential)]
        internal struct DWM_THUMBNAIL_PROPERTIES
        {
            internal DwmThumbnailFlags dwFlags;
            internal SafeNativeMethods.RECT rcDestination;
            internal SafeNativeMethods.RECT rcSource;
            internal byte opacity;
            internal bool fVisible;
            internal bool fSourceClientAreaOnly;
        };

        // Enable/disable non-client rendering based on window style.
        internal const int DWMNCRP_USEWINDOWSTYLE = 0;
        // Disabled non-client rendering; window style is ignored.
        internal const int DWMNCRP_DISABLED = 1;
        // Enabled non-client rendering; window style is ignored.
        internal const int DWMNCRP_ENABLED = 2;
        // Enable/disable non-client rendering Use DWMNCRP_* values.
        internal const int DWMWA_NCRENDERING_ENABLED = 1;
        // Non-client rendering policy.
        internal const int DWMWA_NCRENDERING_POLICY = 2;
        // Potentially enable/forcibly disable transitions 0 or 1.
        internal const int DWMWA_TRANSITIONS_FORCEDISABLED = 3; 

        [StructLayout(LayoutKind.Sequential)]
        internal struct UNSIGNED_RATIO
        {
            internal UInt32 uiNumerator;
            internal UInt32 uiDenominator;
        };

        [StructLayout(LayoutKind.Sequential)]
        internal struct DWM_PRESENT_PARAMETERS
        {
            internal int cbSize;
            internal bool fQueue;
            internal UInt64 cRefreshStart;
            internal uint cBuffer;
            internal bool fUseSourceRate;
            internal UNSIGNED_RATIO uiNumerator;
        };

        
        [StructLayout(LayoutKind.Explicit)]
        internal struct DWM_TIMING_INFO
        {
            [FieldOffset(0)]
            internal UInt32 cbSize;
            [FieldOffset(4)]
            internal UNSIGNED_RATIO rateRefresh;// Monitor refresh rate
            [FieldOffset(12)]
            internal UNSIGNED_RATIO rateCompose;// composition rate     
            [FieldOffset(20)]
            internal UInt64 qpcVBlank;          // QPC time at VBlank
            [FieldOffset(28)]
            internal UInt64 cRefresh;           // DWM refresh counter
            [FieldOffset(36)]
            internal UInt64 qpcCompose;         // QPC time at a compose time
            [FieldOffset(44)]
            internal UInt64 cFrame;             // Frame number that was composed at qpcCompose
            [FieldOffset(52)]
            internal UInt64 cRefreshFrame;      // Refresh count of the frame that was composed at qpcCompose
            [FieldOffset(60)]
            internal UInt64 cRefreshConfirmed;  // The target refresh count of the last
            // frame confirmed completed by the GPU
            [FieldOffset(68)]
            internal UInt32 cFlipsOutstanding;  // the number of outstanding flips

            //
            // Feedback on previous performance only valid on 2nd and subsequent calls
            //
            [FieldOffset(72)]
            internal UInt64 cFrameCurrent;      // Last frame displayed
            [FieldOffset(80)]
            internal UInt64 cFramesAvailable;   // number of frames available but not displayed, used or dropped
            [FieldOffset(88)]
            internal UInt64 cFrameCleared;      // Source frame number when the following statistics where last cleared
            [FieldOffset(96)]
            internal UInt64 cFramesReceived;    // number of new frames received
            [FieldOffset(104)]
            internal UInt64 cFramesDisplayed;   // number of unique frames displayed
            [FieldOffset(112)]
            internal UInt64 cFramesDropped;     // number of rendered frames that wer  never
            [FieldOffset(120)]                // displayed because composition occured too late
            internal UInt64 cFramesMissed;      // number of times an old frame was composed 
            // when a new frame should have been used
            // but was not available
        };
        

        internal const int DWM_BB_ENABLE = 0x00000001;  // fEnable has been specified
        internal const int DWM_BB_BLURREGION = 0x00000002;  // hRgnBlur has been specified
        internal const int DWM_BB_TRANSITIONONMAXIMIZED = 0x00000004;  // fTransitionOnMaximized has been specified

        internal enum DwmBlurBehindDwFlags : uint
        {
            DWM_BB_ENABLE = 0x00000001,
            DWM_BB_BLURREGION = 0x00000002,
            DWM_BB_TRANSITIONONMAXIMIZED = 0x00000004
        }

        internal enum DwmThumbnailFlags : uint
        {
            DWM_TNP_RECTDESTINATION = 0x00000001, //Indicates a value for rcDestination has been specified.
            DWM_TNP_RECTSOURCE = 0x00000002, //Indicates a value for rcSource has been specified.
            DWM_TNP_OPACITY = 0x00000004, //Indicates a value for opacity has been specified.
            DWM_TNP_VISIBLE = 0x00000008, // Indicates a value for fVisible has been specified.
            DWM_TNP_SOURCECLIENTAREAONLY = 0x00000010 //Indicates a value for fSourceClientAreaOnly has been specified.
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct DWM_BLURBEHIND
        {
            public DwmBlurBehindDwFlags dwFlags;
            public bool fEnable;
            public IntPtr hRgnBlur;
            public bool fTransitionOnMaximized;
        };

        [StructLayout(LayoutKind.Sequential)]
        internal struct MARGINS
        {
            public int cxLeftWidth;      // width of left border that retains its size
            public int cxRightWidth;     // width of right border that retains its size
            public int cyTopHeight;      // height of top border that retains its size
            public int cyBottomHeight;   // height of bottom border that retains its size
        };


        #endregion

        #region Task Dialog Declarations

        // Main task dialog configuration struct.
        // NOTE: Packing must be set to 4 to make this work on 64-bit platforms.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto, Pack = 4)]
        internal class TASKDIALOGCONFIG
        {
            internal uint cbSize;
            internal IntPtr hwndParent;
            internal IntPtr hInstance;
            internal TASKDIALOG_FLAGS dwFlags;
            internal TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszWindowTitle;
            internal TASKDIALOGCONFIG_ICON_UNION MainIcon; // NOTE: 32-bit union field, holds pszMainIcon as well
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszMainInstruction;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszContent;
            internal uint cButtons;
            internal IntPtr pButtons;           // Ptr to TASKDIALOG_BUTTON structs
            internal int nDefaultButton;
            internal uint cRadioButtons;
            internal IntPtr pRadioButtons;      // Ptr to TASKDIALOG_BUTTON structs
            internal int nDefaultRadioButton;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszVerificationText;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszExpandedInformation;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszExpandedControlText;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszCollapsedControlText;
            internal TASKDIALOGCONFIG_ICON_UNION FooterIcon;  // NOTE: 32-bit union field, holds pszFooterIcon as well
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszFooter;
            internal PFTASKDIALOGCALLBACK pfCallback;
            internal IntPtr lpCallbackData;
            internal uint cxWidth;
        }

        internal const int TASKDIALOG_IDEALWIDTH = 0;  // Value for TASKDIALOGCONFIG.cxWidth
        internal const int TASKDIALOG_BUTTON_SHIELD_ICON = 1;

        // NOTE: We include a "spacer" so that the struct size varies on 
        // 64-bit architectures.
        [StructLayout(LayoutKind.Explicit, CharSet = CharSet.Auto)]
        internal struct TASKDIALOGCONFIG_ICON_UNION
        {
            internal TASKDIALOGCONFIG_ICON_UNION(int i)
            {
                spacer = IntPtr.Zero;
                pszIcon = 0;
                hMainIcon = i;
            }

            [FieldOffset(0)]
            internal int hMainIcon;
            [FieldOffset(0)]
            internal int pszIcon;
            [FieldOffset(0)]
            internal IntPtr spacer;
        }

        // NOTE: Packing must be set to 4 to make this work on 64-bit platforms.
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto, Pack = 4)]
        internal struct TASKDIALOG_BUTTON
        {
            public TASKDIALOG_BUTTON(int n, string txt)
            {
                nButtonID = n;
                pszButtonText = txt;
            }

            internal int nButtonID;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszButtonText;
        }

        // Task Dialog - identifies common buttons.
        [Flags]
        internal enum TASKDIALOG_COMMON_BUTTON_FLAGS
        {
            TDCBF_OK_BUTTON = 0x0001, // selected control return value IDOK
            TDCBF_YES_BUTTON = 0x0002, // selected control return value IDYES
            TDCBF_NO_BUTTON = 0x0004, // selected control return value IDNO
            TDCBF_CANCEL_BUTTON = 0x0008, // selected control return value IDCANCEL
            TDCBF_RETRY_BUTTON = 0x0010, // selected control return value IDRETRY
            TDCBF_CLOSE_BUTTON = 0x0020  // selected control return value IDCLOSE
        }

        // Identify button *return values* - note that, unfortunately, these are different
        // from the inbound button values.
        internal enum TASKDIALOG_COMMON_BUTTON_RETURN_ID
        {
            IDOK = 1,
            IDCANCEL = 2,
            IDABORT = 3,
            IDRETRY = 4,
            IDIGNORE = 5,
            IDYES = 6,
            IDNO = 7,
            IDCLOSE = 8
        }

        internal enum TASKDIALOG_ELEMENTS
        {
            TDE_CONTENT,
            TDE_EXPANDED_INFORMATION,
            TDE_FOOTER,
            TDE_MAIN_INSTRUCTION
        }

        internal enum TASKDIALOG_ICON_ELEMENT
        {
            TDIE_ICON_MAIN,
            TDIE_ICON_FOOTER
        }

        // Task Dialog - flags
        [Flags]
        internal enum TASKDIALOG_FLAGS
        {
            NONE = 0,
            TDF_ENABLE_HYPERLINKS = 0x0001,
            TDF_USE_HICON_MAIN = 0x0002,
            TDF_USE_HICON_FOOTER = 0x0004,
            TDF_ALLOW_DIALOG_CANCELLATION = 0x0008,
            TDF_USE_COMMAND_LINKS = 0x0010,
            TDF_USE_COMMAND_LINKS_NO_ICON = 0x0020,
            TDF_EXPAND_FOOTER_AREA = 0x0040,
            TDF_EXPANDED_BY_DEFAULT = 0x0080,
            TDF_VERIFICATION_FLAG_CHECKED = 0x0100,
            TDF_SHOW_PROGRESS_BAR = 0x0200,
            TDF_SHOW_MARQUEE_PROGRESS_BAR = 0x0400,
            TDF_CALLBACK_TIMER = 0x0800,
            TDF_POSITION_RELATIVE_TO_WINDOW = 0x1000,
            TDF_RTL_LAYOUT = 0x2000,
            TDF_NO_DEFAULT_RADIO_BUTTON = 0x4000
        }

        internal enum TASKDIALOG_MESSAGES
        {
            TDM_NAVIGATE_PAGE = WM_USER + 101,
            TDM_CLICK_BUTTON = WM_USER + 102, // wParam = Button ID
            TDM_SET_MARQUEE_PROGRESS_BAR = WM_USER + 103, // wParam = 0 (nonMarque) wParam != 0 (Marquee)
            TDM_SET_PROGRESS_BAR_STATE = WM_USER + 104, // wParam = new progress state
            TDM_SET_PROGRESS_BAR_RANGE = WM_USER + 105, // lParam = MAKELPARAM(nMinRange, nMaxRange)
            TDM_SET_PROGRESS_BAR_POS = WM_USER + 106, // wParam = new position
            TDM_SET_PROGRESS_BAR_MARQUEE = WM_USER + 107, // wParam = 0 (stop marquee), wParam != 0 (start marquee), lparam = speed (milliseconds between repaints)
            TDM_SET_ELEMENT_TEXT = WM_USER + 108, // wParam = element (TASKDIALOG_ELEMENTS), lParam = new element text (LPCWSTR)
            TDM_CLICK_RADIO_BUTTON = WM_USER + 110, // wParam = Radio Button ID
            TDM_ENABLE_BUTTON = WM_USER + 111, // lParam = 0 (disable), lParam != 0 (enable), wParam = Button ID
            TDM_ENABLE_RADIO_BUTTON = WM_USER + 112, // lParam = 0 (disable), lParam != 0 (enable), wParam = Radio Button ID
            TDM_CLICK_VERIFICATION = WM_USER + 113, // wParam = 0 (unchecked), 1 (checked), lParam = 1 (set key focus)
            TDM_UPDATE_ELEMENT_TEXT = WM_USER + 114, // wParam = element (TASKDIALOG_ELEMENTS), lParam = new element text (LPCWSTR)
            TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE = WM_USER + 115, // wParam = Button ID, lParam = 0 (elevation not required), lParam != 0 (elevation required)
            TDM_UPDATE_ICON = WM_USER + 116  // wParam = icon element (TASKDIALOG_ICON_ELEMENTS), lParam = new icon (hIcon if TDF_USE_HICON_* was set, PCWSTR otherwise)
        }

        internal enum TASKDIALOG_NOTIFICATIONS
        {
            TDN_CREATED = 0,
            TDN_NAVIGATED = 1,
            TDN_BUTTON_CLICKED = 2,            // wParam = Button ID
            TDN_HYPERLINK_CLICKED = 3,         // lParam = (LPCWSTR)pszHREF
            TDN_TIMER = 4,                     // wParam = Milliseconds since dialog created or timer reset
            TDN_DESTROYED = 5,
            TDN_RADIO_BUTTON_CLICKED = 6,      // wParam = Radio Button ID
            TDN_DIALOG_CONSTRUCTED = 7,
            TDN_VERIFICATION_CLICKED = 8,      // wParam = 1 if checkbox checked, 0 if not, lParam is unused and always 0
            TDN_HELP = 9,
            TDN_EXPANDO_BUTTON_CLICKED = 10    // wParam = 0 (dialog is now collapsed), wParam != 0 (dialog is now expanded)
        }

        // Used in the various SET_DEFAULT* TaskDialog messages
        internal const int NO_DEFAULT_BUTTON_SPECIFIED = 0;

        // Task Dialog config and related structs (for TaskDialogIndirect())
        internal delegate int PFTASKDIALOGCALLBACK(
            IntPtr hwnd,
            uint msg,
            IntPtr wParam,
            IntPtr lParam,
            IntPtr lpRefData);

        internal enum PBST
        {
            PBST_NORMAL = 0x0001,
            PBST_ERROR = 0x0002,
            PBST_PAUSED = 0x0003
        }

        internal enum TD_ICON
        {
            TD_WARNING_ICON = 65535,
            TD_ERROR_ICON = 65534,
            TD_INFORMATION_ICON = 65533,
            TD_SHIELD_ICON = 65532
        }

        #endregion

        #region StockIcon declarations

        [StructLayoutAttribute(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        internal struct StockIconInfo
        {
            internal UInt32 StuctureSize;
            internal IntPtr Handle;
            internal Int32 ImageIndex;
            internal Int32 Identifier;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            internal string Path;
        }

        #endregion

        #region KnownFolders

        [StructLayout(LayoutKind.Sequential)]
        internal struct NativeFolderDefinition
        {
            internal FolderCategory category;
            internal IntPtr name;
            internal IntPtr description;
            internal Guid parentId;
            internal IntPtr relativePath;
            internal IntPtr parsingName;
            internal IntPtr tooltip;
            internal IntPtr localizedName;
            internal IntPtr icon;
            internal IntPtr security;
            internal UInt32 attributes;
            internal DefinitionOptions definitionOptions;
            internal Guid folderTypeId;
        }

        #endregion

        #region Elevation COM Object

        [Flags]
        internal enum CLSCTX
        {
            CLSCTX_INPROC_SERVER = 0x1,
            CLSCTX_INPROC_HANDLER = 0x2,
            CLSCTX_LOCAL_SERVER = 0x4,
            CLSCTX_REMOTE_SERVER = 0x10,
            CLSCTX_NO_CODE_DOWNLOAD = 0x400,
            CLSCTX_NO_CUSTOM_MARSHAL = 0x1000,
            CLSCTX_ENABLE_CODE_DOWNLOAD = 0x2000,
            CLSCTX_NO_FAILURE_LOG = 0x4000,
            CLSCTX_DISABLE_AAA = 0x8000,
            CLSCTX_ENABLE_AAA = 0x10000,
            CLSCTX_FROM_DEFAULT_CONTEXT = 0x20000,
            CLSCTX_INPROC = CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            CLSCTX_SERVER = CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
            CLSCTX_ALL = CLSCTX_SERVER | CLSCTX_INPROC_HANDLER
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct BIND_OPTS3
        {
            internal uint cbStruct;
            internal uint grfFlags;
            internal uint grfMode;
            internal uint dwTickCountDeadline;
            internal uint dwTrackFlags;
            internal uint dwClassContext;
            internal uint locale;
            // This will be passed as null, so the type doesn't matter.
            object pServerInfo;
            internal IntPtr hwnd;
        }

        #endregion
        #region Command Link Definitions

        internal const int BS_COMMANDLINK = 0x0000000E;
        internal const uint BCM_SETNOTE = 0x00001609;
        internal const uint BCM_GETNOTE = 0x0000160A;
        internal const uint BCM_GETNOTELENGTH = 0x0000160B;
        internal const uint BCM_SETSHIELD = 0x0000160C;

        #endregion

        // Property System structs and consts.
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal struct PROPERTYKEY
        {
            internal Guid fmtid;
            internal uint pid;
        }

        #region File Operations Definitions

        [Flags]
        internal enum FOS : uint
        {
            FOS_OVERWRITEPROMPT = 0x00000002,
            FOS_STRICTFILETYPES = 0x00000004,
            FOS_NOCHANGEDIR = 0x00000008,
            FOS_PICKFOLDERS = 0x00000020,
            // Ensure that items returned are filesystem items.
            FOS_FORCEFILESYSTEM = 0x00000040,
            // Allow choosing items that have no storage.
            FOS_ALLNONSTORAGEITEMS = 0x00000080,
            FOS_NOVALIDATE = 0x00000100,
            FOS_ALLOWMULTISELECT = 0x00000200,
            FOS_PATHMUSTEXIST = 0x00000800,
            FOS_FILEMUSTEXIST = 0x00001000,
            FOS_CREATEPROMPT = 0x00002000,
            FOS_SHAREAWARE = 0x00004000,
            FOS_NOREADONLYRETURN = 0x00008000,
            FOS_NOTESTFILECREATE = 0x00010000,
            FOS_HIDEMRUPLACES = 0x00020000,
            FOS_HIDEPINNEDPLACES = 0x00040000,
            FOS_NODEREFERENCELINKS = 0x00100000,
            FOS_DONTADDTORECENT = 0x02000000,
            FOS_FORCESHOWHIDDEN = 0x10000000,
            FOS_DEFAULTNOMINIMODE = 0x20000000
        }
        internal enum CDCONTROLSTATE
        {
            CDCS_INACTIVE = 0x00000000,
            CDCS_ENABLED = 0x00000001,
            CDCS_VISIBLE = 0x00000002
        }
        internal enum SIGDN : uint
        {
            SIGDN_NORMALDISPLAY = 0x00000000,           // SHGDN_NORMAL
            SIGDN_PARENTRELATIVEPARSING = 0x80018001,   // SHGDN_INFOLDER | SHGDN_FORPARSING
            SIGDN_DESKTOPABSOLUTEPARSING = 0x80028000,  // SHGDN_FORPARSING
            SIGDN_PARENTRELATIVEEDITING = 0x80031001,   // SHGDN_INFOLDER | SHGDN_FOREDITING
            SIGDN_DESKTOPABSOLUTEEDITING = 0x8004c000,  // SHGDN_FORPARSING | SHGDN_FORADDRESSBAR
            SIGDN_FILESYSPATH = 0x80058000,             // SHGDN_FORPARSING
            SIGDN_URL = 0x80068000,                     // SHGDN_FORPARSING
            SIGDN_PARENTRELATIVEFORADDRESSBAR = 0x8007c001,     // SHGDN_INFOLDER | SHGDN_FORPARSING | SHGDN_FORADDRESSBAR
            SIGDN_PARENTRELATIVE = 0x80080001           // SHGDN_INFOLDER
        }
        internal enum SIATTRIBFLAGS
        {
            // if multiple items and the attirbutes together.
            SIATTRIBFLAGS_AND = 0x00000001,
            // if multiple items or the attributes together.
            SIATTRIBFLAGS_OR = 0x00000002,
            // Call GetAttributes directly on the 
            // ShellFolder for multiple attributes.
            SIATTRIBFLAGS_APPCOMPAT = 0x00000003,
        }
        internal enum FDE_SHAREVIOLATION_RESPONSE
        {
            FDESVR_DEFAULT = 0x00000000,
            FDESVR_ACCEPT = 0x00000001,
            FDESVR_REFUSE = 0x00000002
        }
        internal enum FDE_OVERWRITE_RESPONSE
        {
            FDEOR_DEFAULT = 0x00000000,
            FDEOR_ACCEPT = 0x00000001,
            FDEOR_REFUSE = 0x00000002
        }
        internal enum FDAP
        {
            FDAP_BOTTOM = 0x00000000,
            FDAP_TOP = 0x00000001,
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        internal struct COMDLG_FILTERSPEC
        {
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszName;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string pszSpec;

            internal COMDLG_FILTERSPEC(string name, string spec)
            {
                pszName = name;
                pszSpec = spec;
            }
        }

        #endregion

        #region Power Management
        internal const uint WM_POWERBROADCAST = 536;
        internal const uint PBT_POWERSETTINGCHANGE = 32787;
        internal const uint SPI_SETSCREENSAVEACTIVE = 0x0011;
        internal const uint SPIF_UPDATEINIFILE = 0x0001;
        internal const uint SPIF_SENDCHANGE = 0x0002;

        // Code for CreateWindowEx, for a windowless message pump.
        internal const int HWND_MESSAGE = -3;

        internal const uint STATUS_ACCESS_DENIED = 0xC0000022;
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        internal struct WNDCLASSEX
        {
            internal uint cbSize;
            internal uint style;
            [MarshalAs(UnmanagedType.FunctionPtr)]
            internal WNDPROC lpfnWndProc;
            internal int cbClsExtra;
            internal int cbWndExtra;
            internal IntPtr hInstance;
            internal IntPtr hIcon;
            internal IntPtr hCursor;
            internal IntPtr hbrBackground;
            [MarshalAs(UnmanagedType.LPTStr)]
            internal string lpszMenuName;
            [MarshalAs(UnmanagedType.LPTStr)]
            internal string lpszClassName;
            internal IntPtr hIconSm;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct POINT
        {
            internal int X;
            internal int Y;

            internal POINT(int x, int y)
            {
                this.X = x;
                this.Y = y;
            }
        }


        [StructLayout(LayoutKind.Sequential)]
        internal struct MSG
        {
            internal IntPtr hwnd;
            internal uint message;
            internal IntPtr wParam;
            internal IntPtr lParam;
            internal uint time;
            internal POINT pt;
        }

        internal delegate int WNDPROC(IntPtr hWnd,
            uint uMessage,
            IntPtr wParam,
            IntPtr lParam);


        // This structure is sent when the PBT_POWERSETTINGSCHANGE message is sent.
        // It describes the power setting that has changed and 
        // contains data about the change.
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        internal struct PowerBroadcastSetting
        {
            internal Guid PowerSetting;
            internal Int32 DataLength;
        }

        // This structure is used when calling CallNtPowerInformation 
        // to retrieve SystemPowerCapabilities
        [StructLayout(LayoutKind.Sequential)]
        internal struct SystemPowerCapabilities
        {
            [MarshalAs(UnmanagedType.I1)]
            internal bool PowerButtonPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SleepButtonPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool LidPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemS1;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemS2;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemS3;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemS4;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemS5;
            [MarshalAs(UnmanagedType.I1)]
            internal bool HiberFilePresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool FullWake;
            [MarshalAs(UnmanagedType.I1)]
            internal bool VideoDimPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool ApmPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool UpsPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool ThermalControl;
            [MarshalAs(UnmanagedType.I1)]
            internal bool ProcessorThrottle;
            internal byte ProcessorMinThrottle;
            internal byte ProcessorMaxThrottle;
            [MarshalAs(UnmanagedType.I1)]
            internal bool FastSystemS4;
            internal byte spare2_1;
            internal byte spare2_2;
            internal byte spare2_3;
            [MarshalAs(UnmanagedType.I1)]
            internal bool DiskSpinDown;
            internal byte spare3_1;
            internal byte spare3_2;
            internal byte spare3_3;
            internal byte spare3_4;
            internal byte spare3_5;
            internal byte spare3_6;
            internal byte spare3_7;
            internal byte spare3_8;
            [MarshalAs(UnmanagedType.I1)]
            internal bool SystemBatteriesPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool BatteriesAreShortTerm;
            internal int granularity;
            internal int capacity;
        }

        [StructLayout(LayoutKind.Sequential)]
        internal struct SystemBatteryState
        {
            [MarshalAs(UnmanagedType.I1)]
            internal bool AcOnLine;
            [MarshalAs(UnmanagedType.I1)]
            internal bool BatteryPresent;
            [MarshalAs(UnmanagedType.I1)]
            internal bool Charging;
            [MarshalAs(UnmanagedType.I1)]
            internal bool Discharging;
            internal byte spare1;
            internal byte spare2;
            internal byte spare3;
            internal byte spare4;
            internal uint MaxCapacity;
            internal uint RemainingCapacity;
            internal uint Rate;
            internal uint EstimatedTime;
            internal uint DefaultAlert1;
            internal uint DefaultAlert2;
        }
        #endregion

    }
}