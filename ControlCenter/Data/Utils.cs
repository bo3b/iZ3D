/* IZ3D_FILE: $Id: MainForm.cs 4276 2010-12-06 18:48:27Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 4276 $
* $Date: 2010-12-06 21:48:27 +0300 (Ïí, 06 äåê 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/branch/CCRedesignCSharp/ControlCenter/MainForm.cs $
*/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using ControlCenter.Shell;
using Microsoft.Win32;

namespace ControlCenter.Data
{
	static class Utils
	{
		public static string GetAppDataFolder()
		{
			string applicationData =
			Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
			applicationData += "\\iZ3D Driver\\";
			return applicationData;
		}

		public static void OpenDocument(string docName)
		{
			Process myProcess = new Process();

			try
			{
				myProcess.StartInfo.FileName = GetAppDataFolder() + docName;
				myProcess.StartInfo.Verb = "open";
				myProcess.StartInfo.CreateNoWindow = true;
				myProcess.Start();
				myProcess.WaitForExit();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		static class NativeMethods
		{
			[DllImport("Win32\\S3DUtils.dll", CharSet = CharSet.Ansi)]
			public static extern void SetupS3DMonitor(IntPtr hwnd, IntPtr hinst, String lpszCmdLine, int nCmdShow);
			[DllImport("Win32\\S3DUtils.dll", CharSet = CharSet.Ansi)]
			public static extern void GetAndWriteToRegistryDriverModuleName(IntPtr hwnd, IntPtr hinst, String lpszCmdLine, int nCmdShow);
			[DllImport("Win32\\S3DUtils.dll", CharSet = CharSet.Ansi)]
			public static extern void RemoveFromRegistryDriverModuleName(IntPtr hwnd, IntPtr hinst, String lpszCmdLine, int nCmdShow);
			[DllImport("Win64\\S3DUtils.dll", CharSet = CharSet.Ansi, EntryPoint = "GetAndWriteToRegistryDriverModuleName")]
			public static extern void GetAndWriteToRegistryDriverModuleName64(IntPtr hwnd, IntPtr hinst, String lpszCmdLine, int nCmdShow);
			[DllImport("Win64\\S3DUtils.dll", CharSet = CharSet.Ansi, EntryPoint = "RemoveFromRegistryDriverModuleName")]
			public static extern void RemoveFromRegistryDriverModuleName64(IntPtr hwnd, IntPtr hinst, String lpszCmdLine, int nCmdShow);
			[DllImport("Win32\\DiagTool.dll")]
			[return: MarshalAs(UnmanagedType.U1)]
			public static extern bool CreateDXDiagLog();
		}

		public static void RunAndWait(string FileName, string Arguments = null)
		{
			Process process = new Process();
			try
			{
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.FileName = FileName;
				process.StartInfo.Arguments = Arguments;
				process.Start();
				process.WaitForExit();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		public static void RunDynTest(bool bFullscreen, Form form)
		{
			Process myProcess = new Process();
			myProcess.StartInfo.UseShellExecute = false;
			myProcess.StartInfo.FileName = Application.StartupPath + @"\win32\DTest.exe";
			if (bFullscreen)
				myProcess.StartInfo.Arguments = "-fullscreen";
			else
				myProcess.StartInfo.Arguments = "";

			if (Is64BitOS() && form != null)
			{
				string str1 = StringLocalization.GetString("//ControlCenter/Help/Run32BitDynamicTest");
				string str2 = StringLocalization.GetString("//ControlCenter/Help/Select");
				switch (MessageBox.Show(form, str1, str2, MessageBoxButtons.YesNoCancel,
					MessageBoxIcon.Question, MessageBoxDefaultButton.Button1))
				{
					case DialogResult.No:
						myProcess.StartInfo.FileName = Application.StartupPath + @"\win64\DTest.exe";
						break;
					case DialogResult.Cancel:
						return;
				}
			}
			try
			{
				myProcess.Start();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		static class SendInputData
		{
			/// <summary>
			/// The event type contained in the union field
			/// </summary>
			public enum SendInputEventType : int
			{
				/// <summary>
				/// Contains Mouse event data
				/// </summary>
				InputMouse,
				/// <summary>
				/// Contains Keyboard event data
				/// </summary>
				InputKeyboard,
				/// <summary>
				/// Contains Hardware event data
				/// </summary>
				InputHardware
			}

			public const uint KEYEVENTF_EXTENDEDKEY = 0x0001;
			public const uint KEYEVENTF_KEYUP = 0x0002;
			public const uint KEYEVENTF_UNICODE = 0x0004;
			public const uint KEYEVENTF_SCANCODE = 0x0008;

			/// <summary>
			/// Used in mouseData if XDOWN or XUP specified
			/// </summary>
			[Flags]
			public enum MouseDataFlags : uint
			{
				/// <summary>
				/// First button was pressed or released
				/// </summary>
				XBUTTON1 = 0x0001,
				/// <summary>
				/// Second button was pressed or released
				/// </summary>
				XBUTTON2 = 0x0002
			}

			/// <summary>
			/// The flags that a MouseInput.dwFlags can contain
			/// </summary>
			[Flags]
			public enum MouseEventFlags : uint
			{
				/// <summary>
				/// Movement occurred
				/// </summary>
				MOUSEEVENTF_MOVE = 0x0001,
				/// <summary>
				/// button down (pair with an up to create a full click)
				/// </summary>
				MOUSEEVENTF_LEFTDOWN = 0x0002,
				/// <summary>
				/// button up (pair with a down to create a full click)
				/// </summary>
				MOUSEEVENTF_LEFTUP = 0x0004,
				/// <summary>
				/// button down (pair with an up to create a full click)
				/// </summary>
				MOUSEEVENTF_RIGHTDOWN = 0x0008,
				/// <summary>
				/// button up (pair with a down to create a full click)
				/// </summary>
				MOUSEEVENTF_RIGHTUP = 0x0010,
				/// <summary>
				/// button down (pair with an up to create a full click)
				/// </summary>
				MOUSEEVENTF_MIDDLEDOWN = 0x0020,
				/// <summary>
				/// button up (pair with a down to create a full click)
				/// </summary>
				MOUSEEVENTF_MIDDLEUP = 0x0040,
				/// <summary>
				/// button down (pair with an up to create a full click)
				/// </summary>
				MOUSEEVENTF_XDOWN = 0x0080,
				/// <summary>
				/// button up (pair with a down to create a full click)
				/// </summary>
				MOUSEEVENTF_XUP = 0x0100,
				/// <summary>
				/// Wheel was moved, the value of mouseData is the number of movement values
				/// </summary>
				MOUSEEVENTF_WHEEL = 0x0800,
				/// <summary>
				/// Map X,Y to entire desktop, must be used with MOUSEEVENT_ABSOLUTE
				/// </summary>
				MOUSEEVENTF_VIRTUALDESK = 0x4000,
				/// <summary>
				/// The X and Y members contain normalized Absolute Co-Ords. If not set X and Y are relative
				/// data to the last position (i.e. change in position from last event)
				/// </summary>
				MOUSEEVENTF_ABSOLUTE = 0x8000
			}

			/// <summary>
			/// The mouse data structure
			/// </summary>
			[StructLayout(LayoutKind.Sequential)]
			public struct MouseInputData
			{
				/// <summary>
				/// The x value, if ABSOLUTE is passed in the flag then this is an actual X and Y value
				/// otherwise it is a delta from the last position
				/// </summary>
				public int dx;
				/// <summary>
				/// The y value, if ABSOLUTE is passed in the flag then this is an actual X and Y value
				/// otherwise it is a delta from the last position
				/// </summary>
				public int dy;
				/// <summary>
				/// Wheel event data, X buttons
				/// </summary>
				public uint mouseData;
				/// <summary>
				/// ORable field with the various flags about buttons and nature of event
				/// </summary>
				public MouseEventFlags dwFlags;
				/// <summary>
				/// The timestamp for the event, if zero then the system will provide
				/// </summary>
				public uint time;
				/// <summary>
				/// Additional data obtained by calling app via GetMessageExtraInfo
				/// </summary>
				public IntPtr dwExtraInfo;
			}

			[StructLayout(LayoutKind.Sequential)]
			public struct KEYBDINPUT
			{
				public ushort wVk;
				public ushort wScan;
				public uint dwFlags;
				public uint time;
				public IntPtr dwExtraInfo;
			}

			[StructLayout(LayoutKind.Sequential)]
			public struct HARDWAREINPUT
			{
				public int uMsg;
				public short wParamL;
				public short wParamH;
			}

			/// <summary>
			/// Captures the union of the three three structures.
			/// </summary>
			[StructLayout(LayoutKind.Explicit)]
			public struct MouseKeybdhardwareInputUnion
			{
				/// <summary>
				/// The Mouse Input Data
				/// </summary>
				[FieldOffset(0)]
				public MouseInputData mi;

				/// <summary>
				/// The Keyboard input data
				/// </summary>
				[FieldOffset(0)]
				public KEYBDINPUT ki;

				/// <summary>
				/// The hardware input data
				/// </summary>
				[FieldOffset(0)]
				public HARDWAREINPUT hi;
			}

			/// <summary>
			/// The Data passed to SendInput in an array.
			/// </summary>
			/// <remarks>Contains a union field type specifies what it contains </remarks>
			[StructLayout(LayoutKind.Sequential)]
			public struct INPUT
			{
				/// <summary>
				/// The actual data type contained in the union Field
				/// </summary>
				public SendInputEventType type;
				public MouseKeybdhardwareInputUnion mkhi;
			}

			[DllImport("user32.dll", SetLastError = true)]
			static extern public uint SendInput(uint nInputs, ref INPUT pInputs, int cbSize);
		}

		public static void RunDynTestWithActivation(bool bFullscreen)
		{
			SendInputData.INPUT inp = new SendInputData.INPUT();
			inp.type = SendInputData.SendInputEventType.InputKeyboard;
			inp.mkhi.ki.wVk = (ushort)Keys.LControlKey;
			inp.mkhi.ki.dwFlags = SendInputData.KEYEVENTF_EXTENDEDKEY;
			SendInputData.SendInput(1, ref inp, Marshal.SizeOf(new SendInputData.INPUT()));
			RunDynTest(bFullscreen, null);
			Thread.Sleep(2000);
			inp.mkhi.ki.dwFlags = SendInputData.KEYEVENTF_KEYUP;
			SendInputData.SendInput(1, ref inp, Marshal.SizeOf(new SendInputData.INPUT()));
		}

		public static void RunDll32(string method)
		{
			Process.Start("rundll32.exe", "\"" + Application.StartupPath + @"\win32\S3DUtils.dll"", " + method);
		}

		public static void RunDll64(string method)
		{
			Process.Start("rundll32.exe", "\"" + Application.StartupPath + @"\win64\S3DUtils.dll"", " + method);
		}

		public static void RunStaticTest()
		{
			RunDll32("Show3D");
		}

		public static void SetupS3DMonitor()
		{
			NativeMethods.SetupS3DMonitor(IntPtr.Zero, IntPtr.Zero, null, 0);
		}

		public static void CreateDXDiagLog()
		{
			try
			{
				File.Delete(GetAppDataFolder() + "Report.txt");
			}
			catch (IOException)
			{				
			}
			if (NativeMethods.CreateDXDiagLog())
				OpenDocument("Report.txt");
		}

		const float MaxStereobase = 40;
		const float SeparationMultiplayer = MaxStereobase / 50 / 100;

		public static float PercentsToSeparation(float value)
		{
			return value * SeparationMultiplayer;
		}
		public static float SeparationToPercents(float value)
		{
			return value / SeparationMultiplayer;
		}

		public static bool Is64BitProcess()
		{
			return (IntPtr.Size == 8);
		}

		public static bool is64BitOS = Is64BitProcess() || InternalCheckIsWow64();

		public static bool Is64BitOS()
		{
			return is64BitOS;
		}

		[DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
		[return: MarshalAs(UnmanagedType.Bool)]
		private static extern bool IsWow64Process(
			[In] IntPtr hProcess,
			[Out] out bool wow64Process
		);

		[System.Runtime.CompilerServices.MethodImpl(MethodImplOptions.NoInlining)]
		private static bool InternalCheckIsWow64()
		{
			if ((Environment.OSVersion.Version.Major == 5 && Environment.OSVersion.Version.Minor >= 1) ||
				Environment.OSVersion.Version.Major >= 6)
			{
				using (Process p = Process.GetCurrentProcess())
				{
					bool retVal;
					if (!IsWow64Process(p.Handle, out retVal))
					{
						return false;
					}
					return retVal;
				}
			}
			else
			{
				return false;
			}
		}

		public static string ResolveLink(string filename)
		{
			string ext = Path.GetExtension(filename);
			if (String.Compare(ext, ".lnk", true) == 0)
			{
				var shl = new ShellLink(filename);
				return shl.Target;
			}
			return filename;
		}

		public static bool IsAutoStartUpEnabled(string strAppPath)
		{
			RegistryKey rkCurrentUser = Registry.CurrentUser;
			RegistryKey regKey = rkCurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Run", false);

			//will return false if the key does not exist 
			if (regKey == null)
				return false;

			string strAppName = "iZ3D Control Center";
			string value = "\"" + strAppPath + "\" /silent";
			try
			{
				string curValue = regKey.GetValue(strAppName) as string;
				return (curValue == value);
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
				return false;
			}
		}

		public static bool AutoStartUp(string strAppPath, bool bAdd)
		{
			RegistryKey rkCurrentUser = Registry.CurrentUser;
			RegistryKey regKey = rkCurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Run", true);

			//will return false if the key does not exist 
			if (regKey == null)
				return false;

			regKey.DeleteValue("Control Center.exe", false);
			string strAppName = "iZ3D Control Center";
			if (bAdd)
			{
				string value = "\"" + strAppPath + "\" /silent";
				try
				{
					regKey.SetValue(strAppName, value);
				}
				catch (Exception ex)
				{
					Debug.WriteLine(ex.Message);
					return false;
				}
			}
			else
			{
				regKey.DeleteValue(strAppName, false);
			}
			return true;
		}

		[DllImport("win32\\S3DUtils.dll")]
		static extern void GenerateFont(IntPtr hWnd, IntPtr hinst, string lpszCmdLine, int nCmdShow);  

		public static void GenerateFont()
		{
			try
			{
				GenerateFont(IntPtr.Zero, IntPtr.Zero, null, 0);
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		public static void EnableMouseLock()
		{			
			Process myProcess32 = new Process();
			try
			{
				string finalPath32 = Application.StartupPath + "\\Win32\\S3DLocker.exe";
				myProcess32.StartInfo.UseShellExecute = false;
				myProcess32.StartInfo.FileName = finalPath32;
				myProcess32.StartInfo.CreateNoWindow = true;
				myProcess32.Start();
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}

			if (Is64BitOS())
			{
				Process myProcess64 = new Process();
				try
				{
					string finalPath64 = Application.StartupPath + "\\Win64\\S3DLocker.exe";
					myProcess64.StartInfo.UseShellExecute = false;
					myProcess64.StartInfo.FileName = finalPath64;
					myProcess64.StartInfo.CreateNoWindow = true;
					myProcess64.Start();
				}
				catch (Exception ex)
				{
					Debug.WriteLine(ex.Message);
				}
			}
		}

		[DllImport("user32.dll", SetLastError = true)]
		static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
		[return: MarshalAs(UnmanagedType.Bool)]
		[DllImport("user32.dll", SetLastError = true)]
		static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);        
		const uint WM_QUIT = 0x0012;

		public static void DisableMouseLock()
		{
			IntPtr hwnd = FindWindow("S3DLockerClass", "S3DLocker32Window");
			if (hwnd != IntPtr.Zero)
				PostMessage(hwnd, WM_QUIT, IntPtr.Zero, IntPtr.Zero);
			hwnd = FindWindow("S3DLockerClass", "S3DLocker64Window");
			if (hwnd != IntPtr.Zero)
				PostMessage(hwnd, WM_QUIT, IntPtr.Zero, IntPtr.Zero);
		}

		public static void LaunchMouseLock(bool launch)
		{
			if (launch)
				EnableMouseLock();
			else
				DisableMouseLock();
		}

		public static void EnableDriver(bool enableDriver)
		{
			// Stereo status
			IPC.IPC ipc = new IPC.IPC();
			ipc.D3DInjected = enableDriver;

			DataHolder.GetInstance().StereoSettings.DriverEnabled = enableDriver;
			DataHolder.GetInstance().SaveStereoState();

			if (enableDriver)
				RunDll32("GetAndWriteToRegistryDriverModuleName");
			else
				RunDll32("RemoveFromRegistryDriverModuleName");
			if (Is64BitOS())
			{
				if (enableDriver)
					RunDll64("GetAndWriteToRegistryDriverModuleName");
				else
					RunDll64("RemoveFromRegistryDriverModuleName");
			}
		}

		public struct UpdateData
		{
			public UpdateData(string _src, string _dst)
			{
				src = _src;
				dstFolder = _dst;
			}
			public string src;
			public string dstFolder;      
		}
				
		private class UpdateResults
		{
			public EventWaitHandle downloadComplete = new EventWaitHandle(false, EventResetMode.AutoReset);
			public List<UpdateData> updatesData = null;
			public bool updatesAvailable = false;
		}

		private static void DownloadUpdatesXml(Object sender, DownloadStringCompletedEventArgs e)
		{
			UpdateResults results = e.UserState as UpdateResults;
			if (e.Cancelled || e.Error != null)
			{
				results.downloadComplete.Set();
				return;
			}
			string xml = e.Result;
			try
			{
				XmlDocument updateXml = new XmlDocument();
				updateXml.LoadXml(xml);
				var nl = updateXml.GetElementsByTagName("Pack");
				if (nl.Count > 0)
				{
					XmlElement el = nl.Item(0) as XmlElement;
					if (el == null)
						return;// false;
					string version = el.Attributes["Version"].Value;

					//HACK: For fixing wrong version number
					if (version == "1.99.9999")
						version = DriverVersion.VERSION;

					if (String.Compare(version, DriverVersion.VERSION) > 0)
					{
						var files = el.GetElementsByTagName("Files");
						if (files.Count == 0)
						{
							results.downloadComplete.Set();
							return;
						}
						el = files.Item(0) as XmlElement;
						if (el == null)
						{
							results.downloadComplete.Set();
							return;
						}
						results.updatesData = new List<UpdateData>();
						var fs = el.GetElementsByTagName("File");
						foreach (var node in fs)
						{
							el = node as XmlElement;
							if (el == null)
								continue;
							string src = el.Attributes["Src"].Value;
							string dst = el.Attributes["Dest"].Value;
							var ud = new UpdateData(src, dst);
							results.updatesData.Add(ud);
						}
						if (results.updatesData.Count > 0)
							results.updatesAvailable = true;
						results.downloadComplete.Set();
						return;
					}
				}
				//BeginInvoke(new InvokeDelegate(ShowUpdateMessage));
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.Message);
			}
			results.downloadComplete.Set();
		}

		public static bool UpdatesAvailable(string fileUrl, out List<UpdateData> updatesData, DownloadProgressChangedEventHandler callback)
		{
			WebClient client = new WebClient();
			// Add a user agent header in case the 
			// requested URI contains a query.
			client.Headers.Add("user-agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
			client.DownloadStringCompleted += DownloadUpdatesXml;
			client.DownloadProgressChanged += callback;
			UpdateResults res = new UpdateResults();
			client.DownloadStringAsync(new Uri(fileUrl), res);
			res.downloadComplete.WaitOne();
			updatesData = res.updatesData;
			return res.updatesAvailable;
		}

		private class FileDownloadResults
		{
			public EventWaitHandle downloadComplete = new EventWaitHandle(false, EventResetMode.AutoReset);
			public List<UpdateData> updatesData = null;
			public bool fileOk = false;
		}

		private static void DownloadFileComplete(Object sender, AsyncCompletedEventArgs e)
		{
			FileDownloadResults results = e.UserState as FileDownloadResults;
			if (e.Cancelled || e.Error != null)
			{
				results.downloadComplete.Set();
				return;
			}
			results.fileOk = true;
			results.downloadComplete.Set();
		}

		public static bool DownloadFile(string fileUrl, string dstName, DownloadProgressChangedEventHandler callback)
		{
			WebClient client = new WebClient();
			// Add a user agent header in case the 
			// requested URI contains a query.
			client.Headers.Add("user-agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
			client.DownloadFileCompleted += DownloadFileComplete;
			client.DownloadProgressChanged += callback;
			FileDownloadResults res = new FileDownloadResults();
			client.DownloadFileAsync(new Uri(fileUrl), dstName, res);
			res.downloadComplete.WaitOne();
			return res.fileOk;
		}
	}
}
