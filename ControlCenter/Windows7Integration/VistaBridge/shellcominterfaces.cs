// Copyright (c) Microsoft Corporation.  All rights reserved.

//Copyright (c) Microsoft Corporation.  All rights reserved.
using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Microsoft.SDK.Samples.VistaBridge.Library.KnownFolders;

#pragma warning disable 0108

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{

    [ComImport,
    Guid(IIDGuid.IShellItem),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IShellItem
    {
        // Not supported: IBindCtx.
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void BindToHandler(
            [In, MarshalAs(UnmanagedType.Interface)] IntPtr pbc, 
            [In] ref Guid bhid, 
            [In] ref Guid riid, 
            out IntPtr ppv);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetParent([MarshalAs(UnmanagedType.Interface)] out IShellItem ppsi);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetDisplayName(
            [In] SafeNativeMethods.SIGDN sigdnName, 
            [MarshalAs(UnmanagedType.LPWStr)] out string ppszName);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetAttributes([In] uint sfgaoMask, out uint psfgaoAttribs);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void Compare(
            [In, MarshalAs(UnmanagedType.Interface)] IShellItem psi, 
            [In] uint hint, 
            out int piOrder);
    }

    [ComImport,
    Guid(IIDGuid.IShellItemArray),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IShellItemArray
    {
        // Not supported: IBindCtx.
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void BindToHandler(
            [In, MarshalAs(UnmanagedType.Interface)] IntPtr pbc, 
            [In] ref Guid rbhid, 
            [In] ref Guid riid, 
            out IntPtr ppvOut);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetPropertyStore(
            [In] int Flags, 
            [In] ref Guid riid, 
            out IntPtr ppv);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetPropertyDescriptionList(
            [In] ref SafeNativeMethods.PROPERTYKEY keyType, 
            [In] ref Guid riid, 
            out IntPtr ppv);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetAttributes(
            [In] SafeNativeMethods.SIATTRIBFLAGS dwAttribFlags, 
            [In] uint sfgaoMask, 
            out uint psfgaoAttribs);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetCount(out uint pdwNumItems);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetItemAt(
            [In] uint dwIndex, 
            [MarshalAs(UnmanagedType.Interface)] out IShellItem ppsi);

        // Not supported: IEnumShellItems (will use GetCount and GetItemAt instead).
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void EnumItems([MarshalAs(UnmanagedType.Interface)] out IntPtr ppenumShellItems);
    }

    [ComImport,
    Guid(IIDGuid.IKnownFolder),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IKnownFolder
    {
        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        Guid GetId();

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        FolderCategory GetCategory();

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        void GetShellItem([In] int i,
             [MarshalAs(UnmanagedType.LPStruct)] Guid interfaceGuid,
             [Out, MarshalAs(UnmanagedType.IUnknown)] out object shellItem);

        [return: MarshalAs(UnmanagedType.LPWStr)]
        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        string GetPath([In] int option);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        void SetPath([In] int i, [In] string path);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        void GetIDList([In] int i,
            [Out] out IntPtr itemIdentifierListPointer);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        Guid GetFolderType();

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        RedirectionCapabilities GetRedirectionCapabilities();

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        void GetFolderDefinition(
            [Out, MarshalAs(UnmanagedType.Struct)] out SafeNativeMethods.NativeFolderDefinition definition);
     
    }

    [ComImport,
    Guid(IIDGuid.IKnownFolderManager),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IKnownFolderManager
    {
        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void FolderIdFromCsidl(int csidl, 
           [Out] out Guid knownFolderID);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void FolderIdToCsidl([In, MarshalAs(UnmanagedType.LPStruct)] Guid id, 
          [Out] out int csidl);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void GetFolderIds([Out] out IntPtr folders, 
          [Out] out UInt32 count);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void GetFolder([In, MarshalAs(UnmanagedType.LPStruct)] Guid id, 
          [Out, MarshalAs(UnmanagedType.Interface)] out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void GetFolderByName(string canonicalName, 
          [Out, MarshalAs(UnmanagedType.Interface)] out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void RegisterFolder(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid knownFolderGuid, 
            [In] ref SafeNativeMethods.NativeFolderDefinition knownFolderDefinition);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void UnregisterFolder(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid knownFolderGuid);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void FindFolderFromPath(
            [In, MarshalAs(UnmanagedType.LPWStr)] string path, 
            [In] int mode, 
            [Out, MarshalAs(UnmanagedType.Interface)] out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void FindFolderFromIDList(); 

        [MethodImpl(MethodImplOptions.InternalCall, 
            MethodCodeType = MethodCodeType.Runtime)]
        void Redirect(); 
    }

    [ComImport]
    [Guid("4df0c730-df9d-4ae3-9153-aa6b82e9795a")]
    internal class KnownFolderManagerClass : IKnownFolderManager
    {
        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void FolderIdFromCsidl(int csidl,
            [Out] out Guid knownFolderID);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void FolderIdToCsidl(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid id,
            [Out] out int csidl);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void GetFolderIds(
            [Out] out IntPtr folders,
            [Out] out UInt32 count);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void GetFolder(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid id,
            [Out, MarshalAs(UnmanagedType.Interface)] 
              out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void GetFolderByName(
            string canonicalName,
            [Out, MarshalAs(UnmanagedType.Interface)] out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void RegisterFolder(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid knownFolderGuid,
            [In] ref SafeNativeMethods.NativeFolderDefinition knownFolderDefinition);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void UnregisterFolder(
            [In, MarshalAs(UnmanagedType.LPStruct)] Guid knownFolderGuid);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void FindFolderFromPath(
            [In, MarshalAs(UnmanagedType.LPWStr)] string path,
            [In] int mode,
            [Out, MarshalAs(UnmanagedType.Interface)] out IKnownFolder knownFolder);

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void FindFolderFromIDList();

        [MethodImpl(MethodImplOptions.InternalCall,
            MethodCodeType = MethodCodeType.Runtime)]
        public virtual extern void Redirect();
    }

    [ComImport, 
    Guid(IIDGuid.IPropertyStore),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IPropertyStore
    {
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetCount([Out] out uint cProps);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetAt([In] uint iProp, out SafeNativeMethods.PROPERTYKEY pkey);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void GetValue([In] ref SafeNativeMethods.PROPERTYKEY key, out object pv);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void SetValue([In] ref SafeNativeMethods.PROPERTYKEY key, [In] ref object pv);

        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        void Commit();
    }
}