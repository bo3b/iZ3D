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
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using ComponentFactory.Krypton.Toolkit;
using ControlCenter.Data;
using Windows7.DesktopIntegration;

namespace ControlCenter
{
	public partial class MainForm : Form
	{
		#region Win32 GDI functions
		// class that exposes needed win32 gdi functions.
		class NativeMethods
		{
			[StructLayout(LayoutKind.Sequential, Pack = 1)]
			struct ARGB
			{
				public byte Blue;
				public byte Green;
				public byte Red;
				public byte Alpha;
			}


			[StructLayout(LayoutKind.Sequential, Pack = 1)]
			public struct BLENDFUNCTION
			{
				public byte BlendOp;
				public byte BlendFlags;
				public byte SourceConstantAlpha;
				public byte AlphaFormat;
			}


			public const Int32 ULW_COLORKEY = 0x00000001;
			public const Int32 ULW_ALPHA = 0x00000002;
			public const Int32 ULW_OPAQUE = 0x00000004;

			public const byte AC_SRC_OVER = 0x00;
			public const byte AC_SRC_ALPHA = 0x01;


			[DllImport("user32.dll", SetLastError = true)]
			[return: MarshalAs(UnmanagedType.Bool)]
			public static extern bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref Point pptDst, ref Size psize, IntPtr hdcSrc, ref Point pprSrc, Int32 crKey, ref BLENDFUNCTION pblend, Int32 dwFlags);

			[DllImport("user32.dll", SetLastError = true)]
			public static extern IntPtr GetDC(IntPtr hWnd);

			[DllImport("user32.dll")]
			[return: MarshalAs(UnmanagedType.Bool)]
			public static extern bool ReleaseDC(IntPtr hWnd, IntPtr hDC);

			[DllImport("gdi32.dll", SetLastError = true)]
			public static extern IntPtr CreateCompatibleDC(IntPtr hDC);

			[DllImport("gdi32.dll", SetLastError = true)]
			[return: MarshalAs(UnmanagedType.Bool)]
			public static extern bool DeleteDC(IntPtr hdc);

			[DllImport("gdi32.dll")]
			public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

			[DllImport("gdi32.dll", SetLastError = true)]
			[return: MarshalAs(UnmanagedType.Bool)]
			public static extern bool DeleteObject(IntPtr hObject);
		}
		#endregion

		#region Alphablended form
		/// <para>Changes the current bitmap with a custom opacity level.  Here is where all happens!</para>
		public void SetBitmap(Bitmap bitmap, byte opacity = 255)
		{
			if (bitmap.PixelFormat != PixelFormat.Format32bppArgb)
				throw new ApplicationException("The bitmap must be 32ppp with alpha-channel.");

			// The idea of this is very simple,
			// 1. Create a compatible DC with screen;
			// 2. Select the bitmap with 32bpp with alpha-channel in the compatible DC;
			// 3. Call the UpdateLayeredWindow.

			IntPtr screenDc = NativeMethods.GetDC(IntPtr.Zero);
			IntPtr memDc = NativeMethods.CreateCompatibleDC(screenDc);
			IntPtr hBitmap = IntPtr.Zero;
			IntPtr oldBitmap = IntPtr.Zero;

			try
			{
				hBitmap = bitmap.GetHbitmap(Color.FromArgb(0));  // grab a GDI handle from this GDI+ bitmap
				oldBitmap = NativeMethods.SelectObject(memDc, hBitmap);

				Size size = bitmap.Size;
				Point pointSource = new Point(0, 0);
				Point topPos = new Point(Left, Top);
				NativeMethods.BLENDFUNCTION blend = new NativeMethods.BLENDFUNCTION();
				blend.BlendOp = NativeMethods.AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.SourceConstantAlpha = opacity;
				blend.AlphaFormat = NativeMethods.AC_SRC_ALPHA;

				bool bResult = NativeMethods.UpdateLayeredWindow(Handle, screenDc, ref topPos, ref size, memDc, ref pointSource, 0, ref blend, NativeMethods.ULW_ALPHA);
			}
			finally
			{
				NativeMethods.ReleaseDC(IntPtr.Zero, screenDc);
				if (hBitmap != IntPtr.Zero)
				{
					NativeMethods.SelectObject(memDc, oldBitmap);
					NativeMethods.DeleteObject(hBitmap);
				}
				NativeMethods.DeleteDC(memDc);
			}
		}


		protected override CreateParams CreateParams
		{
			get
			{
				CreateParams cp = base.CreateParams;
				cp.ExStyle |= 0x00080000; // This form has to have the WS_EX_LAYERED extended style
				return cp;
			}
		}

		private const int WM_NCHITTEST = 0x0084;
		private const int WM_NCLBUTTONDBLCLK = 0xA3;
		private const int WM_USER = 0x0400;

		// Let Windows drag this form for us
		protected override void WndProc(ref Message m)
		{
			switch (m.Msg)
			{
				case WM_NCHITTEST:
					m.Result = (IntPtr)2;	// HTCLIENT
					return;
				case WM_NCLBUTTONDBLCLK:
					return;
				case WM_USER:
					if (m.WParam.ToInt32() == 0) // enable driver
					{
						bool bEnable = m.LParam.ToInt32() != 0;
						rbEnableDriver.Checked = bEnable;
						rbDisableDriver.Checked = !bEnable;
						miEnableDriver.Checked = bEnable;
						miDisableDriver.Checked = !bEnable;
						DataHolder.GetInstance().StereoSettings.DriverEnabled = bEnable;
					}
					else if (m.WParam.ToInt32() == 1) // enable stereo
					{
						bool bEnable = m.LParam.ToInt32() != 0;
						cbEnableStereoAtStartup.Checked = bEnable;
						DataHolder.GetInstance().StereoSettings.StereoEnabled = bEnable;
					}
					return;
			}

			base.WndProc(ref m);
		}
		#endregion

		public MainForm()
		{
			InitializeComponent();
			knPages.SelectedPage = kpStatus;
			knInGame.SelectedPage = kpGame;

			//TransparencyKey = Color.Empty;
			//BackgroundImage = null;
		}

		JumpListManager jumpListManager = null;

		bool IsWin7OrAbove()
		{
			OperatingSystem os = Environment.OSVersion;
			Version vs = os.Version;
			if (os.Platform == PlatformID.Win32NT)
			{
				if (vs.Major == 6 && vs.Minor >= 1 || vs.Major > 6)
					return true;
			}
			return false;
		}


		#region Form
		private void MainForm_Load(object sender, EventArgs e)
		{
			//SetBitmap(Properties.Resources.BackG_tran);
			if (WindowState == FormWindowState.Minimized)
				Hide();

			if (IsWin7OrAbove())
			{
				jumpListManager = new JumpListManager(Handle);
				jumpListManager.UserRemovedItems += (o, ev) =>
				{
					ev.CancelCurrentOperation = true;
				};
				jumpListManager.ClearAllDestinations();
				jumpListManager.EnabledAutoDestinationType = ApplicationDestinationType.NONE;
			}

			Data.DataHolder.GetInstance().LoadAllData();

			DriverSettings Set = Data.DataHolder.GetInstance().Settings;

			StringLocalization.Language = Set.Language;
			if (StringLocalization.FailedToLoadFile && !StringLocalization.FailedToLoadDefFile)
			{
				Set.Language = "English";
				StringLocalization.Language = Set.Language;
				Data.DataHolder.GetInstance().SaveSettings();
			}

			Internationalize();

			if (IsWin7OrAbove())
			{
				StringLocalization.SelectPage("//ControlCenter/DirectX");
				jumpListManager.AddUserTask(new ShellLink
				{
					Path = Application.ExecutablePath,
					Title = StringLocalization.GetString("//rbEnableDriver"),
					Arguments = "/EnableDriver"
				});

				jumpListManager.AddUserTask(new ShellLink
				{
					Path = Application.ExecutablePath,
					Title = StringLocalization.GetString("//rbDisableDriver"),
					Arguments = "/DisableDriver"
				});
				StringLocalization.PopPage();
				jumpListManager.Refresh();
			}
			
			InitTabs();
			InitStereoStatus();
			InitProfiles();
			InitException();
			InitUpdater();
			InitSettings();
			InitHelp();
			InitAbout();

			// switch to simple view
			csModes.CheckedIndex = 0;
		}

		private OutputDll GetOutputObject(System.Collections.IEnumerable items, string name)
		{
			foreach (OutputDll output in items)
			{
				if (output.DLLName == name)
					return output;
			}
			return null;
		}

		private OutputParams GetOutputParams(System.Collections.IEnumerable items, string name)
		{
			OutputDll output = GetOutputObject(items, name);
			if (output == null)
				return 0;
			return output.Parameters;
		}

		private void btnClose_Click(object sender, EventArgs e)
		{
			if (IsWin7OrAbove())
				WindowState = FormWindowState.Minimized;
			else
				Hide();
		}

		private void btnMinimize_Click(object sender, EventArgs e)
		{
			WindowState = FormWindowState.Minimized;
		}

		private void MainForm_Closing(object sender, FormClosingEventArgs e)
		{
#if CMO_BUILD
			IPC.IPC ipc = new IPC.IPC();
			ipc.D3DInjected = false;
#endif
			//App.Helper.HelperThread.Exit();
			//_thread.Abort();
		}

		private void Apply_Click(object sender, EventArgs e)
		{
			this.Cursor = Cursors.WaitCursor;

			if (knPages.SelectedPage == kpStatus)
				kpStatus_ApplyChanges(); 
			else if (knPages.SelectedPage == kpInGame)
				kpInGame_ApplyChanges();
			else if (knPages.SelectedPage == kpExceptions)
				kpException_ApplyChanges();
			else if (knPages.SelectedPage == kpSettings)
				kpSettings_ApplyChanges();

			this.Cursor = Cursors.Default;
		}
		#endregion

		#region Localization
		public static void SetLocalizationText(Control control, string id)
		{
			string text = StringLocalization.GetString(id);
			if (!String.IsNullOrEmpty(text))
			{
				//if (StringLocalization.Language == "English" && text != control.Text)
				//	Debug.WriteLine("Using id " + id + " English value: " + control.Text);
				control.Text = text;
			}
			else
				Debug.WriteLine("Using id " + id + " value: " + control.Text);
		}

		public static void SetLocalizationText(ToolStripItem control, string id)
		{
			string text = StringLocalization.GetString(id);
			if (!String.IsNullOrEmpty(text))
			{
				//if (StringLocalization.Language == "English" && text != control.Text)
				//	Debug.WriteLine("Using id " + id + " English value: " + control.Text);
				control.Text = text;
			}
			else
				Debug.WriteLine("Using id " + id + " value: " + control.Text);
		}

		public static void SetLocalizationText(KryptonComboBox cb, string id)
		{
			var items = StringLocalization.GetStrings(id);

			StringLocalization.SelectPage("//lblWindowModes");
			cb.BeginUpdate();
			cb.Items.Clear();
			foreach (var s in items)
			{
				cb.Items.Add(s);
			}
			if (cb.Items.Count > 0)
				cb.SelectedIndex = 0;
			cb.EndUpdate();
			StringLocalization.PopPage();
		}

		private void Internationalize()
		{
			StringLocalization.SelectRootPage();
			//string str = StringLocalization.GetString("//lblCaption");
			StringLocalization.SelectPage("//ControlCenter");
			SetLocalizationText(chkbtnStatus, "//DirectX/lblCaption");
			SetLocalizationText(chkbtnInGame, "//Profiles/lblCaption");
			SetLocalizationText(chkbtnExceptions, "//Exceptions/lblCaption");
			SetLocalizationText(chkbtnUpdate, "//Update/lblCaption");
			SetLocalizationText(chkbtnSettings, "//Settings/lblCaption");
			SetLocalizationText(chkbtnHelp, "//Help/lblCaption");
			SetLocalizationText(chkbtnAbout, "//About/lblCaption");

			StringLocalization.SelectPage("//DirectX");
			SetLocalizationText(gbStereoStatus, "//sbStereoStatus");
			SetLocalizationText(rbEnableDriver, "//rbEnableDriver");
			SetLocalizationText(cbEnableStereoAtStartup, "//cbEnableStereoAtStartup");
			SetLocalizationText(rbDisableDriver, "//rbDisableDriver");
			SetLocalizationText(gbOutput, "//sbOutput");
			SetLocalizationText(miEnableDriver, "//rbEnableDriver");
			SetLocalizationText(miDisableDriver, "//rbDisableDriver");
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//Profiles");
			{
				SetLocalizationText(btnBrowseExe, "//btnBrowse");
				SetLocalizationText(kpGame, "//Game/lblCaption");
				SetLocalizationText(kpAutofocus, "//Autofocus/lblCaption");
				SetLocalizationText(kpHotkeys, "//Input/lblCaption");

				StringLocalization.SelectPage("//Game");
				SetLocalizationText(chkbShowFPS, "//lblToggleFPS");
				SetLocalizationText(chkbShowOSDMessages, "//lblShowOSD");
				SetLocalizationText(chkbDisableMouseLock, "//lblDisableMouseLock");
				SetLocalizationText(chkbShowWizard, "//lblShowWizard");
				SetLocalizationText(chkbEnableClipping, "//lblEnableClipping");
				SetLocalizationText(chkbForceVSyncOff, "//lblForceVSyncOff");
				SetLocalizationText(chkbUseSimpleStereoProjectionMethod, "//lblUseSimpleStereoProjection");
				SetLocalizationText(chkbShadowsMono, "//lblShadowsMono");

				SetLocalizationText(lblStereoSeparation, "//lblStereoSeparation");
				SetLocalizationText(lblSeparationScale, "//lblSeparationScale");
				SetLocalizationText(lblWindowedMode, "//lblWindowMode");
				SetLocalizationText(lblSeparationMod, "//lblSeparationMode");
				SetLocalizationText(lblClippingWidth, "//lblClippingWidth");

				SetLocalizationText(cbWindowMode, "//lblWindowModes");
				SetLocalizationText(cbSeparationMode, "//lblSeparationModes");
				StringLocalization.PopPage();

				StringLocalization.SelectPage("//Autofocus");
				SetLocalizationText(lblAutofocusWidth, "//lblWidth");
				SetLocalizationText(lblAutofocusHeight, "//lblHeight");
				SetLocalizationText(lblAutofocusSpeed, "//lblSpeed");
				SetLocalizationText(lblAutofocusMaximumShift, "//lblMaximumShift");
				SetLocalizationText(lblAutofocusTop, "//lblTop");
				SetLocalizationText(lblAutofocusBottom, "//lblBottom");
				StringLocalization.PopPage();

				StringLocalization.SelectPage("//Input");
				SetLocalizationText(lblIncConv, "//lblIncConv");
				SetLocalizationText(lblDecConv, "//lblDecConv");
				SetLocalizationText(lblIncSep, "//lblIncSep");
				SetLocalizationText(lblDecSep, "//lblDecSep");
				SetLocalizationText(lblPreset3, "//lblPreset3");
				SetLocalizationText(lblPreset2, "//lblPreset2");
				SetLocalizationText(lblPreset1, "//lblPreset1");
				SetLocalizationText(lblToggleStereo, "//lblToggleStereo");
				SetLocalizationText(lblSwapLR, "//lblSwapLR");
				SetLocalizationText(lblToggleAutofocus, "//lblToggleAutofocus");
				SetLocalizationText(lblToggleLasersight, "//lblToggleLasersight");
				SetLocalizationText(lblToggleFPS, "//lblToggleFPS");
				SetLocalizationText(lblToggleWizard, "//lblToggleWizard");
				SetLocalizationText(lblTakeScreenshot, "//lblTakeScreenshot");
				SetLocalizationText(lblToggleHotkeysOSD, "//lblToggleHotkeysOSD");
				SetLocalizationText(btnRestoreDefaults, "//btnRestoreDef");
				StringLocalization.PopPage();
			}
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//Exceptions");
			SetLocalizationText(btnBrowse, "//btnBrowse");
			SetLocalizationText(btnAdd, "//btnAdd");
			SetLocalizationText(btnRemove, "//btnRemove");
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//Update");
			SetLocalizationText(lblUpdatesStatus, "//msgNoUpdates");
			SetLocalizationText(btnUpdate, "//btnUpdate");
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//Settings");
			SetLocalizationText(gbSettings, "//sbSettings");
			SetLocalizationText(chkbLaunchWithWindows, "//checkMouseAutostart");
			SetLocalizationText(chkbUpdateOnHold, "//checkUpdateOnHold");
			SetLocalizationText(chkbLockMouse, "//checkMouseLock");
			SetLocalizationText(chkbEnableD3D1xDriver, "//enableD3D10Driver");
			SetLocalizationText(gbLanguage, "//sbLanguage");
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//Help");
			SetLocalizationText(btnUserGuide, "//btnManual");
			SetLocalizationText(btnDiagnostic, "//btnDiagnostic");
			SetLocalizationText(btnTrobleshooting, "//btnTrouble");
			SetLocalizationText(btnSetupScreens, "//btnSetupScreens");
			SetLocalizationText(btnActivate, "//btnActivate");
			StringLocalization.PopPage();

			StringLocalization.SelectPage("//About");
			SetLocalizationText(lblVersionC, "//Version");
			SetLocalizationText(lblReleaseDateC, "//ReleaseDate");
			SetLocalizationText(lblCopyright, "//lblCopyright");
			StringLocalization.PopPage();

			SetLocalizationText(btnApply, "//btnApply");

			SetLocalizationText(chkbtnSimple, "//btnSimple");
			SetLocalizationText(chkbtnAdvanced, "//btnAdvanced");
			StringLocalization.PopPage();

			if (cbOutputs.Items.Count != 0)
			{
				cbOutputs.BeginUpdate();
				for (int i = 0; i < cbOutputs.Items.Count; i++)
				{
					var output = cbOutputs.Items[i] as OutputDll;
					output.UpdateLocalizedText();
				}
				cbOutputs.EndUpdate();

				int index = cbOutputModes.SelectedIndex;
				OutputChanged(cbOutputs, null);
				cbOutputModes.SelectedIndex = index;
			}
		}
		#endregion

		#region Tabs
		class BtnSavedData
		{
			public BtnSavedData(KryptonCheckButton chkbtn)
			{
				location = chkbtn.Location;
				size = chkbtn.Size;
				imageNormal = chkbtn.StateCommon.Back.Image;
				imagePressed = chkbtn.StateCheckedPressed.Back.Image;
			}

			private void SetBtnImages(KryptonCheckButton chkbtn, Image imageNormal, Image imageChecked)
			{
				chkbtn.StateCommon.Back.Image = imageNormal;
				chkbtn.StatePressed.Back.Image = imageNormal;
				chkbtn.StateTracking.Back.Image = imageNormal;
				chkbtn.StateCheckedNormal.Back.Image = imageChecked;
				chkbtn.StateCheckedPressed.Back.Image = imageChecked;
				chkbtn.StateCheckedTracking.Back.Image = imageChecked;
			}

			static public void ApplySettings(KryptonCheckButton chkbtnFrom, KryptonCheckButton chkbtnTo)
			{
				BtnSavedData data = chkbtnFrom.Tag as BtnSavedData;
				data.Apply(chkbtnTo);
			}

			static public void ApplySettings(KryptonCheckButton chkbtn)
			{
				BtnSavedData data = chkbtn.Tag as BtnSavedData;
				data.Apply(chkbtn);
			}

			void Apply(KryptonCheckButton chkbtn)
			{
				chkbtn.Location = location;
				chkbtn.Size = size;
				SetBtnImages(chkbtn, imageNormal, imagePressed);
			}

			Point location;
			Size size;
			Image imageNormal;
			Image imagePressed;
		}

		private void InitTabs()
		{
			chkbtnStatus.Tag = new BtnSavedData(chkbtnStatus);
			chkbtnInGame.Tag = new BtnSavedData(chkbtnInGame);
			chkbtnExceptions.Tag = new BtnSavedData(chkbtnExceptions);
			chkbtnUpdate.Tag = new BtnSavedData(chkbtnUpdate);
#if YGT_BUILD || TAERIM_BUILD
			chkbtnUpdate.Enabled = false;
#endif
			chkbtnSettings.Tag = new BtnSavedData(chkbtnSettings);
			chkbtnHelp.Tag = new BtnSavedData(chkbtnHelp);
			chkbtnAbout.Tag = new BtnSavedData(chkbtnAbout);
		}

		private void chkbtnStatus_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpStatus;
		}

		private void chktbtnInGame_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpInGame;
		}

		private void chktbtnExceptions_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpExceptions;
		}

		private void chktbtnUpdate_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpUpdate;
		}

		private void chktbtnSettings_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpSettings;
			bool bDualMonitors = false;
			OutputParams param = GetOutputParams(cbOutputs.Items, Data.DataHolder.GetInstance().StereoSettings.Output);
			if ((param & OutputParams.DualMonitors) != 0)
				bDualMonitors = true;
			chkbLockMouse.Enabled = bDualMonitors;
			if (!bDualMonitors)
				chkbLockMouse.Checked = false;
		}

		private void chktbtnHelp_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpHelp;
			bool biZ3DMonitor = false;
			OutputParams param = GetOutputParams(cbOutputs.Items, Data.DataHolder.GetInstance().StereoSettings.Output);
			if ((param & OutputParams.iZ3DMonitor) != 0)
				biZ3DMonitor = true;
			btnSetupScreens.Enabled = biZ3DMonitor;
		}

		private void chktbtnAbout_Click(object sender, EventArgs e)
		{
			knPages.SelectedPage = kpAbout;
		}

		private void csModes_CheckedButtonChanged(object sender, EventArgs e)
		{
			if (!Visible)
				return;
			var cs = sender as KryptonCheckSet;
			int buttonHeight = chkbtnStatus.Height; // 34
			if (cs.CheckedIndex != 0)
			{
				chkbtnExceptions.Visible = true;
				chkbtnInGame.Visible = true;
				
				BtnSavedData.ApplySettings(chkbtnUpdate);
				BtnSavedData.ApplySettings(chkbtnSettings);
				BtnSavedData.ApplySettings(chkbtnHelp);
				BtnSavedData.ApplySettings(chkbtnAbout);
			}
			else
			{
				chkbtnExceptions.Visible = false;
				chkbtnInGame.Visible = false;

				BtnSavedData.ApplySettings(chkbtnInGame, chkbtnUpdate);
				BtnSavedData.ApplySettings(chkbtnExceptions, chkbtnSettings);
				BtnSavedData.ApplySettings(chkbtnUpdate, chkbtnHelp);
				BtnSavedData.ApplySettings(chkbtnSettings, chkbtnAbout);
			}
		}
		#endregion

		#region Tray
		private void notifyIcon1_DoubleClick(object sender, EventArgs e)
		{
			Show();
			Activate();
		}

		private void miExit_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void miEnableDriver_Click(object sender, EventArgs e)
		{
			if (!miEnableDriver.Checked)
			{
				miEnableDriver.Checked = true;
				return;
			}
			rbEnableDriver.Checked = true;
			miDisableDriver.Checked = false;
			Utils.EnableDriver(true);
		}

		private void miDisableDriver_Click(object sender, EventArgs e)
		{
			if (!miDisableDriver.Checked)
			{
				miDisableDriver.Checked = true;
				return;
			}
			rbDisableDriver.Checked = true;
			miEnableDriver.Checked = false;
			Utils.EnableDriver(false);
		}
		#endregion

		#region Page - Status
		private void InitStereoStatus()
		{
			IPC.IPC ipc = new IPC.IPC();

			if (ipc.Failed)
				MessageBox.Show(this, ipc.ErrorMessage, Text);

			bool bInjected = ipc.D3DInjected;
			bool driverEnabled = DataHolder.GetInstance().StereoSettings.DriverEnabled;
			bool enableStereo = DataHolder.GetInstance().StereoSettings.StereoEnabled;
			rbEnableDriver.Checked = driverEnabled;
			rbDisableDriver.Checked = !driverEnabled;
			miEnableDriver.Checked = driverEnabled;
			miDisableDriver.Checked = !driverEnabled;
			cbEnableStereoAtStartup.Checked = enableStereo;
			ipc.D3DInjected = driverEnabled;

			InitOutputs();
		}

		private void InitOutputs()
		{
			if (cbOutputs.Items.Count == 0)
			{
				cbOutputs.BeginUpdate();
				AddOutputAndModes("S3DOutput", new string[] { "Auto", "CFL", "CF", "TableCFL", "Table" }, 
					OutputParams.DualMonitors | OutputParams.iZ3DMonitor);
				AddOutputAndModes("S3DMarkedOutput", new string[] { "Default" }, OutputParams.Protected);
				AddOutputAndModes("AnaglyphOutput", new string[] { "RedCyan", "OptimizedRedCyan", "BlackWhite", "YellowBlue", "Custom" });
				AddOutputAndModes("LenovoOutput", new string[] { "RedCyan", "OptimizedRedCyan", "BlackWhite", "YellowBlue", "Custom" });
				AddOutputAndModes("AvitridOutput", new string[] { "HorizontalOptimized", "Horizontal", "VerticalOptimized", "Vertical" },
					OutputParams.Protected | OutputParams.ScalingRequired);
				AddOutputAndModes("TaerimOutput", new string[] { "HorizontalOptimized", "Horizontal", "VerticalOptimized", "Vertical" },
					OutputParams.Protected | OutputParams.ScalingRequired);
				AddOutputAndModes("ColorCodeOutput", new string[] { "Normal", "Special" });
                AddOutputAndModes("iZShutterOutput", new string[] { "Default" }, 
					OutputParams.Shutter | OutputParams.ScalingRequired);
				//AddOutputAndModes("CMOShutterOutput", new string[] { "Force120Hz", "Force100Hz", "Force60Hz", "Force55Hz", "Force50Hz" }, 
				//    OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("ASUS", "CMOShutterOutput",
								  new OutputDll.Mode[] { new OutputDll.Mode("Voltage120", 0), new OutputDll.Mode("Voltage100", 1) },
								  OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("Viewsonic_G1", "CMOShutterOutput",
								  new OutputDll.Mode[] { new OutputDll.Mode("Voltage120", 0), 
									  new OutputDll.Mode("Voltage100", 1) }, OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("MSI", "CMOShutterOutput",
								  new OutputDll.Mode[] { new OutputDll.Mode("Voltage120", 0), new OutputDll.Mode("Voltage100", 1) },
								  OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("Dell", "CMOShutterOutput",
								  new OutputDll.Mode[] { new OutputDll.Mode("Voltage120", 0), new OutputDll.Mode("Voltage100", 1) },
								  OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("AMDHD3DOutput", new string[] { "Default" }, OutputParams.Shutter | OutputParams.ScalingRequired);
				AddOutputAndModes("ATIDLP3DOutput", new string[] { "Checkerboard" }, OutputParams.ScalingRequired);
				AddOutputAndModes("DLP3DOutput", new string[] { "Checkerboard" }, OutputParams.Protected | OutputParams.ScalingRequired);
				AddOutputAndModes("ShutterOutput", new string[] { "Simple", "BlueLineCoded", "WhiteLineCoded" }, 
					OutputParams.Protected | OutputParams.Shutter);
				AddOutputAndModes("S3DShutterOutput", new string[] { "Marked" }, OutputParams.Protected | OutputParams.Shutter);
				AddOutputAndModes("S3D120HzProjectorsOutput", new string[] { "Simple" }, OutputParams.Protected | OutputParams.Shutter);
				AddOutputAndModes("InterlacedOutput", new string[] { "HorizontalOptimized", "Horizontal", "VerticalOptimized", "Vertical" },
					OutputParams.Protected | OutputParams.ScalingRequired);
				AddOutputAndModes("ZalmanOutput", new string[] { "InterlacedOptimized", "InterlacedNotOptimized" }, OutputParams.ScalingRequired);
				AddOutputAndModes("DualProjectionOutput", new string[] { "Default" }, OutputParams.Protected | OutputParams.DualMonitors);
				AddOutputAndModes("SideBySideOutput", new string[] { "LeftRight", "OverUnder", "Crosseyed" }, OutputParams.Protected);
				AddOutputAndModes("StereoMirrorOutput", new string[] { "HorizontalFlip", "VerticalFlip", "HorizontalAndVerticalFlip", 
					"VerticalAndHorizontalFlip", "DoubleHorizontalFlip", "DoubleVerticalFlip" }, 
					OutputParams.Protected | OutputParams.DualMonitors);
				AddOutputAndModes("VR920Output", new string[] { "Default", "WOFlush" }, OutputParams.Protected);
				AddOutputAndModes("Z800Output", new string[] { "Default" }, OutputParams.Protected | OutputParams.Shutter);
				cbOutputs.EndUpdate();
			}

			OutputDll output = GetOutputObject(cbOutputs.Items, Data.DataHolder.GetInstance().StereoSettings.Output);
			if (output != null)
			{
				cbOutputs.SelectedItem = output;
				int index = Data.DataHolder.GetInstance().StereoSettings.Mode;
				if (cbOutputModes.Items.Count > index)
					cbOutputModes.SelectedIndex = index;
				else if (cbOutputModes.Items.Count > 0)
					cbOutputModes.SelectedIndex = 0;
			}
		}

		private void kpStatus_ApplyChanges()
		{
			DataHolder.GetInstance().StereoSettings.StereoEnabled = cbEnableStereoAtStartup.Checked;
			Utils.EnableDriver(rbEnableDriver.Checked);
			miEnableDriver.Checked = rbEnableDriver.Checked;
			miDisableDriver.Checked = rbDisableDriver.Checked;

			// Outputs
			var output = cbOutputs.SelectedItem as OutputDll;
			DataHolder.GetInstance().StereoSettings.Output = output.DLLName;
			DataHolder.GetInstance().StereoSettings.Mode = output.Modes[cbOutputModes.SelectedIndex].id;
			if ((output.Parameters & OutputParams.ScalingRequired) != 0)
			{
				if (DataHolder.GetInstance().StereoSettings.ScalingMode == 0)
					DataHolder.GetInstance().StereoSettings.ScalingMode = 4;
			}
			else
				DataHolder.GetInstance().StereoSettings.ScalingMode = 0;

			DataHolder.GetInstance().SaveAllData();
		}

		private void OutputChanged(object sender, EventArgs e)
		{
			KryptonComboBox OutputBox = sender as KryptonComboBox;

			if (OutputBox == null)
				return;

			var output = OutputBox.SelectedItem as OutputDll;
			cbOutputModes.Items.Clear();

			foreach (OutputDll.Mode mode in output.Modes)
			{
				cbOutputModes.Items.Add(mode.localizedName);
			}
			cbOutputModes.SelectedIndex = 0;
		}

		private void AddOutputAndModes(string OutputName, string[] Modes, OutputParams Parameters = 0)
		{
			string path = Application.StartupPath + @"\Win32\OutputMethods\" + OutputName + ".dll";
			if (File.Exists(path))
			{
				var output = new OutputDll(OutputName, OutputName, Modes, Parameters);
				cbOutputs.Items.Add(output);
			}
		}

		private void AddOutputAndModes(string Name, string OutputName, OutputDll.Mode[] Modes, OutputParams Parameters = 0)
		{
			string path = Application.StartupPath + @"\Win32\OutputMethods\" + OutputName + ".dll";
			if (File.Exists(path))
			{
				var output = new OutputDll(Name, OutputName, Modes, Parameters);
				cbOutputs.Items.Add(output);
			}
		}
		#endregion

		#region Page - In-Game Settings
		private void InitProfiles()
		{
			UpdateProfilesList();			
			cbProfiles.SelectedIndex = 0;
#if CMO_BUILD || YGT_BUILD || TAERIM_BUILD
			chkbDisableMouseLock.Visible = false;
#endif
		}

		private void UpdateProfilesList()
		{
			cbProfiles.BeginUpdate();
			cbProfiles.Items.Clear();
			cbProfiles.Items.Add(DataHolder.GetInstance().DefaultProfile);

			var sortedList = DataHolder.GetInstance().UserProfiles;
			sortedList.Sort();
			foreach (Profile prof in sortedList)
			{
				cbProfiles.Items.Add(prof);
			}
			cbProfiles.EndUpdate();
		}

		private void kpInGame_ApplyChanges()
		{
			Profile profile = cbProfiles.SelectedItem as Profile;
			profile.Changed = true;

			// Header
			string oldName = profile.Name;
			profile.Name = tbProfileName.Text;
			profile.Filename = tbExeFileName.Text;

			// Game
			profile.ShowFPS = chkbShowFPS.Checked;
			profile.DisableMouseLock = chkbDisableMouseLock.Checked; // may be wrong check for button
			profile.ShowWizard = chkbShowWizard.Checked;
			profile.EnableClipping = chkbEnableClipping.Checked;
			profile.ShowOnscreenMessages = chkbShowOSDMessages.Checked;
			profile.ForceVSyncOff = chkbForceVSyncOff.Checked;
			profile.UseSimpleStereoProjectionMethod = chkbUseSimpleStereoProjectionMethod.Checked;
			profile.ShadowsMono = chkbShadowsMono.Checked;
			profile.StereoSeparation = (float)udStereoSeparation.Value;
			profile.SeparationScale = (float)udSeparationScale.Value;

			profile.WindowMode = cbWindowMode.SelectedIndex;
			profile.SeparationMode = cbSeparationMode.SelectedIndex;

			profile.ClippingWidth = (float)Convert.ToDouble(tbClippingWidth.Value / 100.0f);

			// Autofocus
			profile.AutofocusWidth = Convert.ToInt32(udAutofocusWidth.Value);
			profile.AutofocusHeight = Convert.ToInt32(udAutofocusHeight.Value);
			profile.AutofocusSpeed = Convert.ToInt32(udAutofocusSpeed.Value);

			profile.AutofocusMaximumShift = (float)udAutofocusMaxShift.Value;
			profile.AutofocusVerticalPosition = (float)Convert.ToDouble((float)tbTopBottom.Value / tbTopBottom.Maximum);

			// HotKeys
			profile.Keys.IncreaseConvergence = (HotKey)tbIncConvergence.Tag;
			profile.Keys.DecreaseConvergence = (HotKey)tbDecConvergence.Tag;
			profile.Keys.IncreaseSeparation = (HotKey)tbIncSeparation.Tag;
			profile.Keys.DecreaseSeparation = (HotKey)tbDecSeparation.Tag;
			profile.Keys.Preset1 = (HotKey)tbPreset1.Tag;
			profile.Keys.Preset2 = (HotKey)tbPreset2.Tag;
			profile.Keys.Preset3 = (HotKey)tbPreset3.Tag;
			profile.Keys.SwapEyes = (HotKey)tbSwapLR.Tag;
			profile.Keys.ToggleStereo = (HotKey)tbToggleStereo.Tag;
			profile.Keys.ToggleLasersight = (HotKey)tbToggleLaserSight.Tag;
			profile.Keys.ToggleAutofocus = (HotKey)tbToggleAutofocus.Tag;
			profile.Keys.StereoScreenshot = (HotKey)tbStereoScreenshot.Tag;
			profile.Keys.ToggleWizard = (HotKey)tbWizard.Tag;
			profile.Keys.ToggleFPS = (HotKey)tbShowFPS.Tag;
			profile.Keys.ToggleHotkeysOSD = (HotKey)tbToggleHotkeysOSD.Tag;

			DataHolder.GetInstance().SaveAllData();

			if (DataHolder.GetInstance().Settings.LockMouse &&
				!((bool)DataHolder.GetInstance().DefaultProfile.DisableMouseLock))
			{
				Utils.DisableMouseLock();
				Utils.EnableMouseLock();
			}

			if (oldName != profile.Name)
			{
				UpdateProfilesList();
				cbProfiles.SelectedItem = profile;
			}
		}

		List<HotKey> allHotKeys = new List<HotKey>();

		void SetHotkeyControlValue(KryptonTextBox tb, HotKey hk)
		{
			HotKey newHK = (HotKey)hk.Clone();
			allHotKeys.Add(newHK);
			tb.Tag = newHK;
			tb.Text = newHK.ToString();
		}

		void cbProfiles_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				bool bGameProfile = (cbProfiles.SelectedIndex != 0); // Default profile
				tbProfileName.Enabled = bGameProfile;
				tbExeFileName.Enabled = bGameProfile;
				btnBrowseExe.Enabled = bGameProfile;
				kpAutofocus.Enabled = bGameProfile;
				udStereoSeparation.Enabled = !bGameProfile;

				Profile profile = cbProfiles.SelectedItem as Profile;
				if (bGameProfile)
				{
					if (profile.BaseProfile != null)
						profile.ApplyValues(profile.BaseProfile);
					profile.ApplyValues(DataHolder.GetInstance().DefaultProfile);
				}

				// Game
				chkbShowFPS.Checked = (bool)profile.ShowFPS;
				chkbDisableMouseLock.Checked = (bool)profile.DisableMouseLock;
				chkbShowWizard.Checked = (bool)profile.ShowWizard;
				chkbEnableClipping.Checked = (bool)profile.EnableClipping;
				chkbShowOSDMessages.Checked = (bool)profile.ShowOnscreenMessages;
				chkbForceVSyncOff.Checked = (bool)profile.ForceVSyncOff;
				chkbUseSimpleStereoProjectionMethod.Checked = (bool)profile.UseSimpleStereoProjectionMethod;
				chkbShadowsMono.Checked = (bool)profile.ShadowsMono;
				udStereoSeparation.Value = (decimal)profile.StereoSeparation;
				udSeparationScale.Value = (decimal)profile.SeparationScale;

				cbWindowMode.SelectedIndex = (int)profile.WindowMode;
				cbSeparationMode.SelectedIndex = (int)profile.SeparationMode;

				if (profile.ClippingWidth < 0.0f || profile.ClippingWidth > tbClippingWidth.Maximum / 100.0f)
					profile.ClippingWidth = 0.02f;
				tbClippingWidth.Value = (int)(profile.ClippingWidth * 100 + 0.5f);
				lblClipping.Text = tbClippingWidth.Value.ToString() + "%";

				// Autofocus
				if ((decimal)profile.AutofocusWidth < udAutofocusWidth.Minimum || (decimal)profile.AutofocusWidth > udAutofocusWidth.Maximum)
					profile.AutofocusWidth = 512;
				udAutofocusWidth.Value = (int)profile.AutofocusWidth;
				if ((decimal)profile.AutofocusHeight < udAutofocusHeight.Minimum || (decimal)profile.AutofocusHeight > udAutofocusHeight.Maximum)
					profile.AutofocusHeight = 64;
				udAutofocusHeight.Value = (int)profile.AutofocusHeight;
				if ((decimal)profile.AutofocusSpeed < udAutofocusSpeed.Minimum || (decimal)profile.AutofocusSpeed > udAutofocusSpeed.Maximum)
					profile.AutofocusHeight = 1;
				udAutofocusSpeed.Value = (int)profile.AutofocusSpeed;

				if ((decimal)profile.AutofocusMaximumShift < udAutofocusMaxShift.Minimum || (decimal)profile.AutofocusMaximumShift > udAutofocusMaxShift.Maximum)
					profile.AutofocusMaximumShift = 0.2f;
				udAutofocusMaxShift.Value = (decimal)profile.AutofocusMaximumShift;
				if (profile.AutofocusVerticalPosition < 0.0f || profile.AutofocusVerticalPosition > 1.0f)
					profile.AutofocusVerticalPosition = 0.495f;
				tbTopBottom.Value = (int)(profile.AutofocusVerticalPosition * tbTopBottom.Maximum + 0.5f);

				// Hotkeys
				allHotKeys.Clear();
				SetHotkeyControlValue(tbIncConvergence, profile.Keys.IncreaseConvergence);
				SetHotkeyControlValue(tbDecConvergence, profile.Keys.DecreaseConvergence);
				SetHotkeyControlValue(tbIncSeparation, profile.Keys.IncreaseSeparation);
				SetHotkeyControlValue(tbDecSeparation, profile.Keys.DecreaseSeparation);
				SetHotkeyControlValue(tbPreset1, profile.Keys.Preset1);
				SetHotkeyControlValue(tbPreset2, profile.Keys.Preset2);
				SetHotkeyControlValue(tbPreset3, profile.Keys.Preset3);
				SetHotkeyControlValue(tbSwapLR, profile.Keys.SwapEyes);
				SetHotkeyControlValue(tbToggleStereo, profile.Keys.ToggleStereo);
				SetHotkeyControlValue(tbToggleLaserSight, profile.Keys.ToggleLasersight);
				SetHotkeyControlValue(tbToggleAutofocus, profile.Keys.ToggleAutofocus);
				SetHotkeyControlValue(tbStereoScreenshot, profile.Keys.StereoScreenshot);
				SetHotkeyControlValue(tbWizard, profile.Keys.ToggleWizard);
				SetHotkeyControlValue(tbShowFPS, profile.Keys.ToggleFPS);
				SetHotkeyControlValue(tbToggleHotkeysOSD, profile.Keys.ToggleHotkeysOSD);

				tbProfileName.Text = profile.Name;
				tbExeFileName.Text = profile.Filename;
			}
			catch
			{

			}
		}

		private void HotKeyBoxKeyDown(object sender, KeyEventArgs e)
		{
			e.Handled = true;

			KryptonTextBox textBox = (KryptonTextBox)sender;
			HotKey hk = (HotKey)textBox.Tag;
			if (e.KeyCode == Keys.Escape) // unbind key
			{
				hk.VKKey = 0;
				textBox.Text = hk.ToString();
				textBox.Tag = hk;
				return;
			}

			HotKey newhk = new HotKey(e.Control, e.Alt, e.Shift, e.KeyCode);
			foreach (HotKey oldhk in allHotKeys)
			{
				if (!Object.ReferenceEquals(hk, oldhk))
				{
					// Key already used
					if (newhk == oldhk)
						return;
				}
			}

            if (e.KeyCode != Keys.ControlKey && e.KeyCode != Keys.ShiftKey && e.KeyCode != Keys.Menu)
			{
				hk.VKKey = newhk.VKKey;
				textBox.Text = hk.ToString();
				textBox.Tag = hk;
			}
		}

		private void HotKeyBoxKeyPress(object sender, KeyPressEventArgs e)
		{
			e.Handled = true;
		}

		private void tbProfileName_KeyPress(object sender, KeyPressEventArgs e)
		{
			string s = "\\/:*?\"<>|";
			if (s.IndexOf(e.KeyChar) >= 0)
				e.Handled = true;
		}

		private void udStereoSeparation_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (!Char.IsDigit(e.KeyChar) && !Char.IsControl(e.KeyChar))
				e.Handled = true;
		}

		private void AddProfile(string filename)
		{
			filename = Utils.ResolveLink(filename);
			string shortfilename = Path.GetFileName(filename);
			Profile profile = DataHolder.GetInstance().UserProfiles.Find(p => (String.Compare(p.Filename, shortfilename, true) == 0));
			if (profile != null)
			{
				cbProfiles.SelectedItem = profile;
				return;
			}

			profile = new Profile();
			profile.Changed = true;
			profile.Filename = shortfilename;
			profile.Name = Path.GetFileNameWithoutExtension(profile.Filename);
			profile.PreviousName = profile.Name;
			DataHolder.GetInstance().UserProfiles.Add(profile);

			UpdateProfilesList();
			cbProfiles.SelectedItem = profile;
			DataHolder.GetInstance().SaveAllData();
		}

		private void btnBrowseExe_Click(object sender, EventArgs e)
		{
			if (openFileDialog.ShowDialog() == DialogResult.OK)
				AddProfile(openFileDialog.FileName);
		}

		private void tbExeFileName_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop))
				e.Effect = DragDropEffects.Copy;
			else
				e.Effect = DragDropEffects.None;
		}

		private void tbExeFileName_DragDrop(object sender, DragEventArgs e)
		{
			try
			{
				Array a = (Array)e.Data.GetData(DataFormats.FileDrop);

				if (a != null)
				{
					AddProfile((string)a.GetValue(0));
					this.Activate();
				}
			}
			catch (Exception)
			{
			}
		}

		void SetHotkeyControlValue(KryptonTextBox tb, uint value)
		{
			HotKey hk = tb.Tag as HotKey;
			Debug.Assert(hk != null);
			hk.VKKey = value;
			tb.Text = hk.ToString();
		}

		private void btnRestoreDefaults_Click(object sender, EventArgs e)
		{
			// Default values from S3DAPI\GlobalData.h HotKeyInfo hotKeyList[MAX_EVENTS] = ...
			SetHotkeyControlValue(tbIncConvergence, 363);
			SetHotkeyControlValue(tbDecConvergence, 365);
			SetHotkeyControlValue(tbIncSeparation, 107);
			SetHotkeyControlValue(tbDecSeparation, 109);
			SetHotkeyControlValue(tbPreset1, 103);
			SetHotkeyControlValue(tbPreset2, 104);
			SetHotkeyControlValue(tbPreset3, 105);
			SetHotkeyControlValue(tbSwapLR, 631);
			SetHotkeyControlValue(tbToggleStereo, 106);
			SetHotkeyControlValue(tbToggleLaserSight, 623);
			SetHotkeyControlValue(tbToggleAutofocus, 111);
			SetHotkeyControlValue(tbStereoScreenshot, 44);
			SetHotkeyControlValue(tbWizard, 362);
			SetHotkeyControlValue(tbShowFPS, 0); //123
			SetHotkeyControlValue(tbToggleHotkeysOSD, 368);
		}

		private void tbClippingWidth_ValueChanged(object sender, EventArgs e)
		{
			lblClipping.Text = tbClippingWidth.Value.ToString() + "%";
		}
		#endregion

		#region Page - Exceptions
		private void InitException()
		{
			lbEceptionsList.Items.Clear();
			List<ExceptionElement> tempList = new List<ExceptionElement>();
			var up = DataHolder.GetInstance().UserProfileExceptionsList;
			tempList.AddRange(up);
			var bp = DataHolder.GetInstance().BaseProfilesExceptionsList;
			tempList.AddRange(bp);
			AddToExceptions(tempList);
		}

		private void kpException_ApplyChanges()
		{
			var up = DataHolder.GetInstance().UserProfileExceptionsList;
			up.Clear();
			foreach (ExceptionElement p in lbEceptionsList.Items)
			{
				if (!p.FromBaseProfile)
					up.Add(p);
			}
			DataHolder.GetInstance().SaveAllData();
		}

		private void btnBrowse_Click(object sender, EventArgs e)
		{
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				string filename = Utils.ResolveLink(openFileDialog.FileName);
				tbExceptionFilename.Text = Path.GetFileName(filename);
				tbExceptionName.Text = Path.GetFileNameWithoutExtension(tbExceptionFilename.Text);
			}                
		}

		private void tbExceptionFilename_DragDrop(object sender, DragEventArgs e)
		{
			try
			{
				string[] a = (string[])e.Data.GetData(DataFormats.FileDrop);

				if (a != null && a.Length > 0)
				{
					string filename = Utils.ResolveLink(a[0]);
					tbExceptionFilename.Text = Path.GetFileName(filename);
					tbExceptionName.Text = Path.GetFileNameWithoutExtension(tbExceptionFilename.Text);
					this.Activate();
				}
			}
			catch (Exception)
			{
			}
		}

		private void lbEceptionsList_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop))
				e.Effect = DragDropEffects.Copy;
			else
				e.Effect = DragDropEffects.None;
		}

		private void AddToExceptions(IEnumerable<ExceptionElement> list)
		{
			List<ExceptionElement> tempList = new List<ExceptionElement>();
			foreach (ExceptionElement ex in lbEceptionsList.Items)
				tempList.Add(ex);
			tempList.AddRange(list);
			tempList.Sort();
			lbEceptionsList.BeginUpdate();
			lbEceptionsList.Items.Clear();
			lbEceptionsList.Items.AddRange(tempList.ToArray());
			lbEceptionsList.EndUpdate();
		}

		private void lbEceptionsList_DragDrop(object sender, DragEventArgs e)
		{
			try
			{
				Array a = (Array)e.Data.GetData(DataFormats.FileDrop);

				if (a != null)
				{
					List<ExceptionElement> list = new List<ExceptionElement>();
					foreach (string filename in a)
					{
						var fn = Utils.ResolveLink(filename);
						fn = Path.GetFileName(fn);
						var appName = Path.GetFileNameWithoutExtension(fn);
						ExceptionElement ex = new ExceptionElement(appName, fn);
						list.Add(ex);
					}
					AddToExceptions(list);
					this.Activate();
				}
			}
			catch (Exception)
			{
			}
		}

		private void btnAdd_Click(object sender, EventArgs e)
		{
			if (String.IsNullOrEmpty(tbExceptionName.Text))
			{
				string str = StringLocalization.GetString("//ControlCenter/AppExceptions/EnterValidNameOfApplication");
				MessageBox.Show(this, str);
				return;
			}
			if (String.IsNullOrEmpty(tbExceptionFilename.Text))
			{
				string str = StringLocalization.GetString("//ControlCenter/AppExceptions/YouMustChooseAFileFirst");
				MessageBox.Show(this, str);
				return;
			}
		
			ExceptionElement ex = new ExceptionElement(tbExceptionName.Text, tbExceptionFilename.Text);
			AddToExceptions(new [] { ex });
		}

		private void btnRemove_Click(object sender, EventArgs e)
		{
			if (lbEceptionsList.SelectedItem == null)
				return;
			ExceptionElement ex = lbEceptionsList.SelectedItem as ExceptionElement;
			if (!ex.FromBaseProfile)
				lbEceptionsList.Items.Remove(lbEceptionsList.SelectedItem);
		}

		private void lbEceptionsList_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Delete)
				btnRemove_Click(sender, e);
		}
		#endregion

		#region Page - Update
#if !CMO_BUILD
		const string url = "http://update.iz3d.com/files/";
#else
		const string url = "http://updater.neurok.ru/files/";
#endif
		private void InitUpdater()
		{
#if !(YGT_BUILD || TAERIM_BUILD)
			webBrowser1.Url = new Uri(url + "ccnews.html");
			ThreadPool.QueueUserWorkItem(new WaitCallback(CheckForUpdates));
#endif
		}

		bool updatesAvailable = false;
		List<Utils.UpdateData> updatesData = null;

		private void CheckForUpdates(Object stateInfo)
		{
			string fileUrl = url + "update_v2.xml";
			if (DataHolder.GetInstance().Settings.TestUpdaterURL)
				fileUrl = url + "update_test.xml";
			updatesAvailable = Utils.UpdatesAvailable(fileUrl, out updatesData, DownloadProgressCallback);
			BeginInvoke(new InvokeDelegate(ShowUpdateMessage));
		}

		private void DownloadProgressCallback(object sender, DownloadProgressChangedEventArgs e)
		{
			progressBar1.BeginInvoke(new DownloadProgressChangedEventHandler(SetProgressBarValues), sender, e);
		}

		private void SetProgressBarValues(object sender, DownloadProgressChangedEventArgs e)
		{
			if (IsWin7OrAbove())
				Windows7Taskbar.SetProgressValue(Handle, (ulong)e.BytesReceived, (ulong)e.TotalBytesToReceive);
			progressBar1.Maximum = (int)e.TotalBytesToReceive;
			progressBar1.Value = (int)e.BytesReceived;
		}

		private void DownloadComplete()
		{
			progressBar1.Value = 0;
			if (IsWin7OrAbove())
				Windows7Taskbar.SetProgressState(Handle, Windows7Taskbar.ThumbnailProgressState.NoProgress);
		}

		public delegate void InvokeDelegate();

		private void ShowUpdateMessage()
		{
			DownloadComplete();
			btnUpdate.Enabled = updatesAvailable;
			if (updatesAvailable)
				lblUpdatesStatus.Text = "";
			else
				lblUpdatesStatus.Text = StringLocalization.GetString("//ControlCenter/Update/msgNoUpdates");
			if (!DataHolder.GetInstance().Settings.ShowUpdates || !updatesAvailable)
				return;
			string text = StringLocalization.GetString("//ControlCenter/Update/msgNewUpdates");
			string caption = StringLocalization.GetString("//ControlCenter/Update/textUpdateWindowTitle");
			if (MessageBox.Show(this, text, caption, MessageBoxButtons.YesNo) == DialogResult.Yes)
			{
				ThreadPool.QueueUserWorkItem(new WaitCallback(RunUpdates));
			}
		}

		private void btnUpdate_Click(object sender, EventArgs e)
		{
			ThreadPool.QueueUserWorkItem(new WaitCallback(RunUpdates));
		}

		private void RunUpdates(Object stateInfo)
		{
			if (updatesData == null || updatesData.Count == 0)
				return;

			string appDir = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\iZ3D Driver\\Updater\\";
			Directory.CreateDirectory(appDir);
			string filename = appDir + updatesData[0].src;
			string fileurl = url + updatesData[0].src;

			bool bSuccess = Utils.DownloadFile(fileurl, filename, DownloadProgressCallback);
			BeginInvoke(new InvokeDelegate(DownloadComplete));
			if (bSuccess)
				Process.Start(filename);
		}
		#endregion

		#region Page - Settings
		private void InitSettings()
		{
			DriverSettings Set = Data.DataHolder.GetInstance().Settings;

			chkbLaunchWithWindows.Checked = Utils.IsAutoStartUpEnabled(Application.ExecutablePath);

			string appDir = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\iZ3D Driver\\Language\\";

			string[] PathFiles = System.IO.Directory.GetFiles(appDir, "*.xml");
			char[] Delimiters = { '\\', '.' };

			string[] Files = new string[PathFiles.Length];

			for (int i = 0; i < PathFiles.Length; i++)
			{
				int firstIndex = PathFiles[i].LastIndexOf('\\') + 1;
				int lastIndex = PathFiles[i].LastIndexOf('.');
				Files[i] = PathFiles[i].Substring(firstIndex, lastIndex - firstIndex);
				cbLanguages.Items.Add(Files[i]);
			}

			string language = char.ToUpper(Set.Language[0]) + Set.Language.Substring(1);
			cbLanguages.SelectedItem = language;
			chkbLaunchWithWindows.Checked = Set.LaunchWithWindows;
			chkbUpdateOnHold.Checked = Set.ShowUpdates;
			chkbLockMouse.Checked = Set.LockMouse;
			chkbEnableD3D1xDriver.Checked = Set.EnableD3D1xDriver;
			if (!File.Exists(Application.StartupPath + @"\Win32\S3DWrapperD3D10.dll"))
				chkbEnableD3D1xDriver.Hide();

			Utils.LaunchMouseLock(chkbLockMouse.Checked);

			//////////////////////////////////////////////////////////////////////////
#if CMO_BUILD || YGT_BUILD || TAERIM_BUILD
			chkbLockMouse.Hide();

			/*typedef IDirect3D9* (WINAPI *PFDIRECT3DCREATE)(UINT sdkVersion);
			PFDIRECT3DCREATE pfnCreateD3D	= NULL;
			bool bFoundNVIDIAAdapter		= true;

			HMODULE hD3D9Dll = LoadLibrary( _T( "d3d9.dll" )  );
			if ( hD3D9Dll )
				pfnCreateD3D = (PFDIRECT3DCREATE)GetProcAddress( hD3D9Dll, "Direct3DCreate9" );

			LPDIRECT3D9 pD3D = pfnCreateD3D( D3D_SDK_VERSION );
			if ( pD3D )
			{

				const DWORD ATIVendorId	= 0x1002;
				UINT AdapterCount		= pD3D->GetAdapterCount();
				D3DADAPTER_IDENTIFIER9	Adapter_Identify;
				for( UINT i = 0; i < AdapterCount; i++ )
				{
					pD3D->GetAdapterIdentifier( i, 0, &Adapter_Identify );
					if (Adapter_Identify.VendorId == ATIVendorId )
						bFoundNVIDIAAdapter = false;
				}

				pD3D->Release();
				pD3D = NULL;
			}

			FreeLibrary( hD3D9Dll );

			if ( bFoundNVIDIAAdapter )
				enableD3D10Driver_->Enable( false );*/
#endif//CMO_BUILD || YGT_BUILD || TAERIM_BUILD
		}

		private void kpSettings_ApplyChanges()
		{
			DriverSettings Set = Data.DataHolder.GetInstance().Settings;

			Utils.AutoStartUp(Application.ExecutablePath, chkbLaunchWithWindows.Checked);

			Set.Language = (string)cbLanguages.SelectedItem;
			Set.LaunchWithWindows = chkbLaunchWithWindows.Checked;
			Set.ShowUpdates = chkbUpdateOnHold.Checked;
			Set.LockMouse = chkbLockMouse.Checked;
			Set.EnableD3D1xDriver = chkbEnableD3D1xDriver.Checked;
			DataHolder.GetInstance().SaveAllData();

			Utils.LaunchMouseLock(chkbLockMouse.Checked);

			if (!String.Equals(Set.Language, StringLocalization.Language))
			{
				StringLocalization.Language = Set.Language;
				Internationalize();
				Utils.GenerateFont();
			}
		}
		#endregion

		#region Page - Help
		public void InitHelp()
		{
#if CMO_BUILD || YGT_BUILD || TAERIM_BUILD
			btnSetupScreens.Enabled = false;
#endif
#if CMO_BUILD || YGT_BUILD
			btnActivate.Visible = false;
#endif
		}

		private void btnDTest_Click(object sender, EventArgs e)
		{
			bool bFullscreen = false;
			OutputParams param = GetOutputParams(cbOutputs.Items, Data.DataHolder.GetInstance().StereoSettings.Output);
			if ((param & OutputParams.Shutter) != 0)
				bFullscreen = true;
			Utils.RunDynTest(bFullscreen, this);
		}

		private void btnStaticTest_Click(object sender, EventArgs e)
		{
			Utils.RunStaticTest();
		}

		private void btnDiagnostic_Click(object sender, EventArgs e)
		{
			Utils.CreateDXDiagLog();
		}

		private void btnSetupScreens_Click(object sender, EventArgs e)
		{
			Utils.SetupS3DMonitor();
		}

		private void btnUserGuide_Click(object sender, EventArgs e)
		{
			Utils.OpenDocument("Description.CHM");
		}

		private void btnTrobleshooting_Click(object sender, EventArgs e)
		{
			Utils.OpenDocument("Troubleshooting.CHM");
		}

		private void btnActivate_Click(object sender, EventArgs e)
		{
			Utils.RunDynTestWithActivation(false);
		}
		#endregion

		#region Page - About
		private void InitAbout()
		{
			string suffix = "";
#if ANAGLYPH_BUILD
			suffix = " - Anaglyph Edition";
#elif CMO_BUILD
			suffix = " - CMI Edition";
#elif LENOVO_BUILD
			suffix = " - Lenovo Edition";
#elif YGT_BUILD
			suffix = " - YGT Edition";
#elif TAERIM_BUILD
			suffix = " - 3D 24Inch LED (FPR) Edition";
#endif
#if !FINAL_RELEASE
			suffix += " DEBUG";
#endif
			string branch = "";
			int ind = SVN.BRANCH_URL.LastIndexOf("/dev/driver/");
			if (ind >= 0)
			{
				string s = SVN.BRANCH_URL.Substring(ind + "/dev/driver/".Length);
				var a = s.Split('/', '\\');
				if (a.Length > 1 && a[0].StartsWith("branch"))
					branch = " (branch " + a[1] + ")";
				else if (a.Length > 1 && a[0].StartsWith("tags") && a[1] != "stable")
					branch = " (tag " + a[1] + ")";
			}
			lblVersion.Text = DriverVersion.DISPLAYED_VERSION + suffix + branch;
			lblReleaseDate.Text = SVN.RELEASE_DATA + " \x00A9 2005 - 2011 iZ3D Inc.";
		}
		#endregion
	}
}
