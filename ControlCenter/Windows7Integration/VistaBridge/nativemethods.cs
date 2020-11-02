// Copyright (c) Microsoft Corporation.  All rights reserved.

//Copyright (c) Microsoft Corporation.  All rights reserved.
using System;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Library;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    /// <summary>
    /// Internal class containing most native interop declarations used
    /// throughout the library.
    /// Functions that are not performance intensive belong in this class.
    /// </summary>
 
    internal static class NativeMethods
    {
        #region General Definitions

        // Various helpers for forcing binding to proper 
        // version of Comctl32 (v6).
        [DllImport(ExternDll.Kernel32, SetLastError = true,
        ThrowOnUnmappableChar = true, BestFitMapping = false)]
        internal static extern IntPtr LoadLibrary(
             [MarshalAs(UnmanagedType.LPStr)] string lpFileName);

        [DllImport(ExternDll.Kernel32, SetLastError = true,
            ThrowOnUnmappableChar = true, BestFitMapping = false)]
        internal static extern IntPtr GetProcAddress(
            IntPtr hModule,
            [MarshalAs(UnmanagedType.LPStr)] string lpProcName);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool DeleteObject(IntPtr graphicsObjectHandle);

        #endregion

        #region TaskDialog Definitions

        [DllImport(ExternDll.ComCtl32, CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern HRESULT TaskDialog(
            IntPtr hwndParent,
            IntPtr hInstance,
            [MarshalAs(UnmanagedType.LPWStr)] string pszWindowtitle,
            [MarshalAs(UnmanagedType.LPWStr)] string pszMainInstruction,
            [MarshalAs(UnmanagedType.LPWStr)] string pszContent,
            SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
            [MarshalAs(UnmanagedType.LPWStr)]string pszIcon,
            [In, Out] ref int pnButton);

        [DllImport(ExternDll.ComCtl32, CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern HRESULT TaskDialogIndirect(
            [In] SafeNativeMethods.TASKDIALOGCONFIG pTaskConfig,
            [Out] out int pnButton,
            [Out] out int pnRadioButton,
            [MarshalAs(UnmanagedType.Bool)][Out] out bool pVerificationFlagChecked);

        internal delegate HRESULT TDIDelegate(
            [In] SafeNativeMethods.TASKDIALOGCONFIG pTaskConfig,
            [Out] out int pnButton,
            [Out] out int pnRadioButton,
            [Out] out bool pVerificationFlagChecked);

        
        #endregion
        
        #region Shell Definitions

        [DllImport(ExternDll.Shell32, CharSet = CharSet.Auto, 
            SetLastError = true)]
        internal static extern uint SHCreateItemFromParsingName(
            [MarshalAs(UnmanagedType.LPWStr)] string path,
            // The following parameter is not used - binding context.
            IntPtr pbc, 
            ref Guid riid,
            [MarshalAs(UnmanagedType.Interface)] out IShellItem shellItem);

        //[DllImport("Shell32", CharSet= CharSet.Unicode)]
        //internal static extern SafeIconHandle ExtractIcon(IntPtr hInst,
        //    string fileName,
        //    int iconIndex);

        [DllImport("User32", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool DestroyIcon(IntPtr hIcon);

        [DllImport("User32", SetLastError = true,CharSet= CharSet.Unicode)]
        internal static extern int LoadString(IntPtr hInstance,
            int uID,
            StringBuilder buffer,
            int nBufferMax);
        #endregion

        #region Application Restart and Recovery Definitions


        [DllImport("kernel32.dll")]
        internal static extern void ApplicationRecoveryFinished(
           [MarshalAs(UnmanagedType.Bool)] bool success);

        [DllImport("kernel32.dll")]
        internal static extern uint ApplicationRecoveryInProgress(
            [Out, MarshalAs(UnmanagedType.Bool)] out bool canceled);

        //[DllImport("kernel32.dll")]
        //internal static extern uint GetApplicationRecoveryCallback(
        //    IntPtr processHandle,
        //    out RecoveryCallback recoveryCallback,
        //    out RecoveryData parameter,
        //    out uint pingInterval,
        //    out uint flags);

        //[DllImport("kernel32.dll", CharSet=CharSet.Unicode)]
        //internal static extern uint RegisterApplicationRecoveryCallback(
        //    RecoveryCallback recoveryCallback,
        //    RecoveryData parameter,
        //    uint pingInterval,
        //    uint flags); // Unused.


        //[DllImport("kernel32.dll")]
        //internal static extern uint RegisterApplicationRestart(
        //    [MarshalAs(UnmanagedType.BStr)] string commandLineArgs,
        //    RestartRestrictions flags);

        //[DllImport("KERNEL32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        //internal static extern uint GetApplicationRestartSettings(
        //    IntPtr process,
        //    IntPtr commandLine,
        //    ref uint size,
        //    out RestartRestrictions flags);

        [DllImport("kernel32.dll")]
        internal static extern uint UnregisterApplicationRecoveryCallback();

        [DllImport("kernel32.dll")]
        internal static extern uint UnregisterApplicationRestart();
        #endregion

        #region Power Management
        [DllImport("powrprof.dll", SetLastError = true)]
        internal static extern UInt32 CallNtPowerInformation(
             Int32 InformationLevel,
             IntPtr lpInputBuffer,
             UInt32 nInputBufferSize,
             IntPtr lpOutputBuffer,
             UInt32 nOutputBufferSize
        );

        //[DllImport("User32", SetLastError = true,
        //    EntryPoint = "RegisterPowerSettingNotification",
        //    CallingConvention = CallingConvention.StdCall)]
        //internal static extern int RegisterPowerSettingNotification(
        //        SafeWindowHandle hRecipient,
        //        ref Guid PowerSettingGuid,
        //        Int32 Flags);

        //[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        //internal static extern ExecutionState SetThreadExecutionState(ExecutionState esFlags);

        [DllImport("user32.dll",
            EntryPoint = "SystemParametersInfo", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SystemParametersInfoSet(
           uint action, uint param, IntPtr vparam, uint init);

        //[DllImport("User32", SetLastError = true, EntryPoint = "CreateWindowExW")]
        //internal static extern SafeWindowHandle CreateWindowEx(int dwExStyle,
        //    int lpClass,
        //    [MarshalAs(UnmanagedType.LPWStr)]string lpWindowName,
        //    int dwStyle, int X, int Y, int nWidth, int nHeight,
        //    int hWndParent, int hMenu, IntPtr hInstance, IntPtr lpParam);

        //[DllImport("User32", SetLastError = true, EntryPoint = "DestroyWindow",
        // CallingConvention = CallingConvention.StdCall)]
        //internal static extern int DestroyWindow(IntPtr handle);

        //[DllImport("user32.dll")]
        //internal static extern int GetMessage(
        //    out SafeNativeMethods.MSG lpMsg,
        //    SafeWindowHandle hWnd,
        //    uint wMsgFilterMin,
        //    uint wMsgFilterMax);

        [DllImport("User32.dll", SetLastError = true, EntryPoint = "RegisterClassExW")]
        public static extern int RegisterClassEx(
            ref SafeNativeMethods.WNDCLASSEX wndcls);

        [DllImport("user32.dll")]
        internal static extern int DispatchMessage([In] ref SafeNativeMethods.MSG lpmsg);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool TranslateMessage([In] ref SafeNativeMethods.MSG lpMsg);
        #endregion


    }

}