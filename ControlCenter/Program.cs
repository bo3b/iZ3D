/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using ControlCenter.Data;

namespace ControlCenter
{
	static class Program
	{
		[DllImport("user32.dll")]
		[return: MarshalAs(UnmanagedType.Bool)]
        static extern bool SetForegroundWindow(IntPtr hWnd);

        private const int WM_USER = 0x0400;

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        static bool PostMessage(IntPtr hWnd, uint Msg, uint wParam, int lParam)
        {
            return PostMessage(hWnd, Msg, new IntPtr(wParam), new IntPtr(lParam));
        }

		private static IntPtr CCApplicationWindow()
		{ 
			Process me = Process.GetCurrentProcess();
            foreach (Process process in Process.GetProcessesByName(me.ProcessName))
			{
				if (process.Id != me.Id && process.MainWindowHandle != null)
				{
					return process.MainWindowHandle;
				}
            }
#if DEBUG
            if (Debugger.IsAttached)
            {
                const string vshost = ".vshost";
                int i = me.ProcessName.LastIndexOf(vshost);
                if (i == me.ProcessName.Length - vshost.Length)
                {
                    string p = me.ProcessName.Substring(0, i);
                    foreach (Process process in Process.GetProcessesByName(p))
                    {
                        if (process.Id != me.Id && process.MainWindowHandle != null)
                        {
                            return process.MainWindowHandle;
                        }
                    }
                }
            }
#endif
            return IntPtr.Zero;
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
            IntPtr form = CCApplicationWindow();
			bool silent = false;
			bool runApp = true;
			foreach (string str in args)
			{
				runApp = false;
				if (str.Length > 0 && (str[0] == '-' || str[0] == '/'))
				{
					string cmd = str.Substring(1);
                    if (String.Compare(cmd, "silent") == 0)
                        silent = true;
                    else
                    {
                        DataHolder.GetInstance().LoadStereoState();
                        if (String.Compare(cmd, "EnableDriver", true) == 0)
                        {
                            Utils.EnableDriver(true);
                            if (form != IntPtr.Zero)
                                PostMessage(form, WM_USER, 0, 1);
                        }
                        else if (String.Compare(cmd, "DisableDriver", true) == 0)
                        {
                            Utils.EnableDriver(false);
                            if (form != IntPtr.Zero)
                                PostMessage(form, WM_USER, 0, 0);
                        }
                        else if (String.Compare(cmd, "EnableStereo", true) == 0)
                        {
                            DataHolder.GetInstance().StereoSettings.StereoEnabled = true;
                            DataHolder.GetInstance().SaveStereoState();
                            if (form != IntPtr.Zero)
                                PostMessage(form, WM_USER, 1, 1);
                        }
                        else if (String.Compare(cmd, "DisableStereo", true) == 0)
                        {
                            DataHolder.GetInstance().StereoSettings.StereoEnabled = false;
                            DataHolder.GetInstance().SaveStereoState();
                            if (form != IntPtr.Zero)
                                PostMessage(form, WM_USER, 1, 0);
                        }
                    }
				}
			}
			if (!runApp && !silent)
				return;
			bool bNew = true;
			using (Mutex mutex = new Mutex(true, "Global\\IZ3DCC_2C59A0B6-0656-4E25-B475-B883E47388E3_" + Environment.UserName, out bNew))
			{
                if (bNew)
                {
                    // TODO: WaitForServiceIsReady
                    Application.EnableVisualStyles();
                    Application.SetCompatibleTextRenderingDefault(false);
                    Form mainForm = new MainForm();
                    if (silent)
                        mainForm.WindowState = FormWindowState.Minimized;
                    Application.Run(mainForm);
                }
                else
                {
                    Trace.WriteLine("Application is already running!");
                    if (form != IntPtr.Zero)
                        SetForegroundWindow(form);
                }
			}
		}
	}
}
