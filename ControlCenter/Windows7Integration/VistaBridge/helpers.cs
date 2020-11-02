// Copyright (c) Microsoft Corporation.  All rights reserved.

//Copyright (c) Microsoft Corporation.  All rights reserved.
using System;
using System.Windows;
using System.Text;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.IO;
using System.Drawing;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    internal static class Helpers
    {
        /*internal static Window GetDefaultOwnerWindow()
        {
            Window defaultWindow = null;

            if (Application.Current != null && Application.Current.MainWindow != null)
            {
                defaultWindow = Application.Current.MainWindow;
            }
            return defaultWindow;
        }*/

        internal static bool RunningOnVista
        {
            get
            {
                return Environment.OSVersion.Version.Major >= 6;
            }
        }
        internal static bool RunningOnXP
        {
            get
            {
                return Environment.OSVersion.Version.Major >= 5;
            }
        }

        internal static IShellItem GetShellItemFromPath(string path)
        {
            if (String.IsNullOrEmpty(path))
                throw new ArgumentNullException(
                "path", "Shell item cannot be generated from null or empty path.");

            IShellItem resultItem;
            Guid shellItemGuid = new Guid(IIDGuid.IShellItem);
            uint result = NativeMethods.SHCreateItemFromParsingName(
                path,
                IntPtr.Zero,
                ref shellItemGuid,
                out resultItem);
            // Throw if an error occurred.
            System.Runtime.InteropServices.Marshal.ThrowExceptionForHR((int)result);
            return resultItem;
        }
        internal static string GetStringResource(string resourceId)
        {
            string[] parts;
            string library;
            int index;

            if (String.IsNullOrEmpty(resourceId))
            {
                return String.Empty;
            }
            // Known folder "Recent" has a malformed resource id
            // for its tooltip. This causes the resource id to
            // parse into 3 parts instead of 2 parts if we don't fix.
            resourceId = resourceId.Replace("shell32,dll", "shell32.dll");
            parts = resourceId.Split(new char[] { ',' });

            library = parts[0];
            library = library.Replace(@"@", String.Empty);

            parts[1] = parts[1].Replace("-", String.Empty);
            index = Int32.Parse(parts[1]);

            library = Environment.ExpandEnvironmentVariables(library);
            IntPtr handle = NativeMethods.LoadLibrary(library);
            StringBuilder stringValue = new StringBuilder(255);
            int retval = NativeMethods.LoadString(
                handle, index, stringValue, 255);

            if (retval == 0)
            {
                int error = Marshal.GetLastWin32Error();
                throw new System.ComponentModel.Win32Exception(error);
            }
            return stringValue.ToString();
        }

        /*internal static BitmapSource GetIcon(string iconResourceId)
        {
            string[] parts;
            string library;
            int index;
            BitmapSource bms;

            if (String.IsNullOrEmpty(iconResourceId))
                return null;

            parts = iconResourceId.Split(new char[] { ',' });
            Debug.Assert(parts.Length == 2);
            library = parts[0];
            library = library.Replace("@", "");
            index = Int32.Parse(parts[1]);

            SafeIconHandle hIcon = NativeMethods.ExtractIcon(
                Process.GetCurrentProcess().Handle,
                library,
                index);

            if (hIcon == null)
                throw new FileNotFoundException("Can't find icon.");

            Bitmap bmp = Bitmap.FromHicon(hIcon.DangerousGetHandle());

            using (MemoryStream stream = new MemoryStream())
            {
                bmp.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
                stream.Seek(0, SeekOrigin.Begin);
                // Need to create the frame with OnLoad to 
                // allow the stream to be closed.
                BitmapFrame bf = BitmapFrame.Create(
                    stream,
                    BitmapCreateOptions.None,
                    BitmapCacheOption.OnLoad);

                bms = bf;
            }
            hIcon.SetHandleAsInvalid();
            return bms;
        }*/
    }
}