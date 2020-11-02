namespace ControlCenter
{
	partial class MainForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.btnMinimize = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnClose = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnApply = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.chkbtnSimple = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnAdvanced = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.csTabs = new ComponentFactory.Krypton.Toolkit.KryptonCheckSet(this.components);
			this.chkbtnStatus = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnInGame = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnExceptions = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnUpdate = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnSettings = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnHelp = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.chkbtnAbout = new ComponentFactory.Krypton.Toolkit.KryptonCheckButton();
			this.csModes = new ComponentFactory.Krypton.Toolkit.KryptonCheckSet(this.components);
			this.knPages = new ComponentFactory.Krypton.Navigator.KryptonNavigator();
			this.kpStatus = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.gbOutput = new ComponentFactory.Krypton.Toolkit.KryptonGroupBox();
			this.cbOutputModes = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.cbOutputs = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.gbStereoStatus = new ComponentFactory.Krypton.Toolkit.KryptonGroupBox();
			this.rbDisableDriver = new ComponentFactory.Krypton.Toolkit.KryptonRadioButton();
			this.cbEnableStereoAtStartup = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.rbEnableDriver = new ComponentFactory.Krypton.Toolkit.KryptonRadioButton();
			this.kpInGame = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.knInGame = new ComponentFactory.Krypton.Navigator.KryptonNavigator();
			this.kpGame = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.lblClipping = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.cbSeparationMode = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.lblSeparationMod = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.cbWindowMode = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.lblWindowedMode = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.udSeparationScale = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.lblSeparationScale = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.udStereoSeparation = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.lblStereoSeparation = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblClippingWidth = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbClippingWidth = new ComponentFactory.Krypton.Toolkit.KryptonTrackBar();
			this.chkbShadowsMono = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbUseSimpleStereoProjectionMethod = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbForceVSyncOff = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbShowOSDMessages = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbEnableClipping = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbShowWizard = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbDisableMouseLock = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbShowFPS = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.kpAutofocus = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.kryptonPanel1 = new ComponentFactory.Krypton.Toolkit.KryptonPanel();
			this.lblAutofocusBottom = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblAutofocusTop = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbTopBottom = new ComponentFactory.Krypton.Toolkit.KryptonTrackBar();
			this.udAutofocusMaxShift = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.udAutofocusSpeed = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.udAutofocusHeight = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.udAutofocusWidth = new ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown();
			this.lblAutofocusMaximumShift = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblAutofocusSpeed = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblAutofocusHeight = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblAutofocusWidth = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.kpHotkeys = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.tbToggleHotkeysOSD = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleHotkeysOSD = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.btnRestoreDefaults = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.tbWizard = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleWizard = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbStereoScreenshot = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblTakeScreenshot = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbToggleLaserSight = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleLasersight = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbShowFPS = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleFPS = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbSwapLR = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblSwapLR = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbToggleAutofocus = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleAutofocus = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbPreset3 = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblPreset3 = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbPreset2 = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblPreset2 = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbPreset1 = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblPreset1 = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbToggleStereo = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblToggleStereo = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbDecSeparation = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblDecSep = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbIncSeparation = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblIncSep = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbDecConvergence = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblDecConv = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.tbIncConvergence = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.lblIncConv = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.btnBrowseExe = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.tbExeFileName = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.tbProfileName = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.cbProfiles = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.kpUpdate = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.btnUpdate = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.lblUpdatesStatus = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.webBrowser1 = new System.Windows.Forms.WebBrowser();
			this.kpSettings = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.gbLanguage = new ComponentFactory.Krypton.Toolkit.KryptonGroupBox();
			this.cbLanguages = new ComponentFactory.Krypton.Toolkit.KryptonComboBox();
			this.gbSettings = new ComponentFactory.Krypton.Toolkit.KryptonGroupBox();
			this.chkbEnableD3D1xDriver = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbLockMouse = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbUpdateOnHold = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.chkbLaunchWithWindows = new ComponentFactory.Krypton.Toolkit.KryptonCheckBox();
			this.kpHelp = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.btnActivate = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnSetupScreens = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnTrobleshooting = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnDiagnostic = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnUserGuide = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnStaticTest = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnDynamicTest = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.kpAbout = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.lblReleaseDate = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblVersion = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblCopyright = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblReleaseDateC = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.lblVersionC = new ComponentFactory.Krypton.Toolkit.KryptonLabel();
			this.kpExceptions = new ComponentFactory.Krypton.Navigator.KryptonPage();
			this.btnRemove = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.btnAdd = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.lbEceptionsList = new ComponentFactory.Krypton.Toolkit.KryptonListBox();
			this.tbExceptionName = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.btnBrowse = new ComponentFactory.Krypton.Toolkit.KryptonButton();
			this.tbExceptionFilename = new ComponentFactory.Krypton.Toolkit.KryptonTextBox();
			this.kryptonManager1 = new ComponentFactory.Krypton.Toolkit.KryptonManager(this.components);
			this.kryptonPalette1 = new ComponentFactory.Krypton.Toolkit.KryptonPalette(this.components);
			this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
			this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.miEnableDriver = new System.Windows.Forms.ToolStripMenuItem();
			this.miDisableDriver = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.miExit = new System.Windows.Forms.ToolStripMenuItem();
			this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
			((System.ComponentModel.ISupportInitialize)(this.csTabs)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.csModes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.knPages)).BeginInit();
			this.knPages.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpStatus)).BeginInit();
			this.kpStatus.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gbOutput)).BeginInit();
			this.gbOutput.Panel.SuspendLayout();
			this.gbOutput.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbOutputModes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.cbOutputs)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.gbStereoStatus)).BeginInit();
			this.gbStereoStatus.Panel.SuspendLayout();
			this.gbStereoStatus.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpInGame)).BeginInit();
			this.kpInGame.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.knInGame)).BeginInit();
			this.knInGame.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpGame)).BeginInit();
			this.kpGame.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbSeparationMode)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.cbWindowMode)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.kpAutofocus)).BeginInit();
			this.kpAutofocus.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kryptonPanel1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.kpHotkeys)).BeginInit();
			this.kpHotkeys.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbProfiles)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.kpUpdate)).BeginInit();
			this.kpUpdate.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpSettings)).BeginInit();
			this.kpSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.gbLanguage)).BeginInit();
			this.gbLanguage.Panel.SuspendLayout();
			this.gbLanguage.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbLanguages)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.gbSettings)).BeginInit();
			this.gbSettings.Panel.SuspendLayout();
			this.gbSettings.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpHelp)).BeginInit();
			this.kpHelp.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpAbout)).BeginInit();
			this.kpAbout.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpExceptions)).BeginInit();
			this.kpExceptions.SuspendLayout();
			this.contextMenuStrip1.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnMinimize
			// 
			this.btnMinimize.Location = new System.Drawing.Point(632, 35);
			this.btnMinimize.Margin = new System.Windows.Forms.Padding(0);
			this.btnMinimize.Name = "btnMinimize";
			this.btnMinimize.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnMinimize.Size = new System.Drawing.Size(36, 28);
			this.btnMinimize.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnMin_norm;
			this.btnMinimize.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnMinimize.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnMinimize.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnMinimize.StateCommon.Border.Width = 0;
			this.btnMinimize.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnMin_pressed;
			this.btnMinimize.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnMin_over;
			this.btnMinimize.TabIndex = 1;
			this.btnMinimize.TabStop = false;
			this.btnMinimize.Values.Text = "";
			this.btnMinimize.Click += new System.EventHandler(this.btnMinimize_Click);
			// 
			// btnClose
			// 
			this.btnClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnClose.Location = new System.Drawing.Point(668, 35);
			this.btnClose.Margin = new System.Windows.Forms.Padding(0);
			this.btnClose.Name = "btnClose";
			this.btnClose.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnClose.Size = new System.Drawing.Size(36, 28);
			this.btnClose.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnClose_norm;
			this.btnClose.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnClose.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnClose.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnClose.StateCommon.Border.Width = 0;
			this.btnClose.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnClose_pressed;
			this.btnClose.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnClose_over;
			this.btnClose.TabIndex = 2;
			this.btnClose.TabStop = false;
			this.btnClose.Values.Text = "";
			this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
			// 
			// btnApply
			// 
			this.btnApply.Location = new System.Drawing.Point(584, 523);
			this.btnApply.Margin = new System.Windows.Forms.Padding(0);
			this.btnApply.Name = "btnApply";
			this.btnApply.Size = new System.Drawing.Size(124, 33);
			this.btnApply.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnApply_norm;
			this.btnApply.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnApply.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnApply.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnApply.StateCommon.Border.Width = 0;
			this.btnApply.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnApply.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnApply.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnApply_pressed;
			this.btnApply.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnApply_over;
			this.btnApply.TabIndex = 3;
			this.btnApply.Values.Text = "Apply";
			this.btnApply.Click += new System.EventHandler(this.Apply_Click);
			// 
			// chkbtnSimple
			// 
			this.chkbtnSimple.Location = new System.Drawing.Point(56, 151);
			this.chkbtnSimple.Name = "chkbtnSimple";
			this.chkbtnSimple.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSimple.Size = new System.Drawing.Size(74, 21);
			this.chkbtnSimple.StateCheckedNormal.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnSimple.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnSimpleAdvanced_pressed;
			this.chkbtnSimple.StateCheckedTracking.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnSimple.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSimpleAdvanced_pressed;
			this.chkbtnSimple.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSimple.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnSimple.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnSimple.StateCommon.Border.Width = 0;
			this.chkbtnSimple.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.chkbtnSimple.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnSimple.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnSimple.TabIndex = 5;
			this.chkbtnSimple.Values.Text = "Simple";
			// 
			// chkbtnAdvanced
			// 
			this.chkbtnAdvanced.Checked = true;
			this.chkbtnAdvanced.Location = new System.Drawing.Point(136, 151);
			this.chkbtnAdvanced.Name = "chkbtnAdvanced";
			this.chkbtnAdvanced.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAdvanced.Size = new System.Drawing.Size(74, 21);
			this.chkbtnAdvanced.StateCheckedNormal.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnAdvanced.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnSimpleAdvanced_pressed;
			this.chkbtnAdvanced.StateCheckedTracking.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnAdvanced.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSimpleAdvanced_pressed;
			this.chkbtnAdvanced.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAdvanced.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnAdvanced.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAdvanced.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnAdvanced.StateCommon.Border.Width = 0;
			this.chkbtnAdvanced.StateCommon.Content.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnAdvanced.StateCommon.Content.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
			this.chkbtnAdvanced.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnAdvanced.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnAdvanced.TabIndex = 6;
			this.chkbtnAdvanced.Values.Text = "Advanced";
			// 
			// csTabs
			// 
			this.csTabs.CheckButtons.Add(this.chkbtnStatus);
			this.csTabs.CheckButtons.Add(this.chkbtnInGame);
			this.csTabs.CheckButtons.Add(this.chkbtnExceptions);
			this.csTabs.CheckButtons.Add(this.chkbtnUpdate);
			this.csTabs.CheckButtons.Add(this.chkbtnSettings);
			this.csTabs.CheckButtons.Add(this.chkbtnHelp);
			this.csTabs.CheckButtons.Add(this.chkbtnAbout);
			this.csTabs.CheckedButton = this.chkbtnStatus;
			// 
			// chkbtnStatus
			// 
			this.chkbtnStatus.Checked = true;
			this.chkbtnStatus.Location = new System.Drawing.Point(32, 192);
			this.chkbtnStatus.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnStatus.Name = "chkbtnStatus";
			this.chkbtnStatus.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnStatus.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnStatus.OverrideDefault.Border.Width = 0;
			this.chkbtnStatus.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnStatus.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnStatus.Size = new System.Drawing.Size(198, 34);
			this.chkbtnStatus.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_pressed;
			this.chkbtnStatus.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_pressed;
			this.chkbtnStatus.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_pressed;
			this.chkbtnStatus.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnStatus.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_normal;
			this.chkbtnStatus.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnStatus.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnStatus.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnStatus.StateCommon.Border.Width = 0;
			this.chkbtnStatus.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnStatus.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnStatus.StateCommon.Content.Padding = new System.Windows.Forms.Padding(20, -1, -1, -1);
			this.chkbtnStatus.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnStatus.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnStatus.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnStatus.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_normal;
			this.chkbtnStatus.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnStatus_normal;
			this.chkbtnStatus.TabIndex = 7;
			this.chkbtnStatus.UseMnemonic = false;
			this.chkbtnStatus.Values.Image = global::ControlCenter.Properties.Resources.StatusAndMethods;
			this.chkbtnStatus.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnStatus.Values.Text = "Status && Methods";
			this.chkbtnStatus.Click += new System.EventHandler(this.chkbtnStatus_Click);
			// 
			// chkbtnInGame
			// 
			this.chkbtnInGame.Location = new System.Drawing.Point(31, 226);
			this.chkbtnInGame.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnInGame.Name = "chkbtnInGame";
			this.chkbtnInGame.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnInGame.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnInGame.OverrideDefault.Border.Width = 0;
			this.chkbtnInGame.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnInGame.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnInGame.Size = new System.Drawing.Size(199, 34);
			this.chkbtnInGame.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_pressed;
			this.chkbtnInGame.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_pressed;
			this.chkbtnInGame.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_pressed;
			this.chkbtnInGame.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnInGame.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_normal;
			this.chkbtnInGame.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnInGame.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnInGame.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnInGame.StateCommon.Border.Width = 0;
			this.chkbtnInGame.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnInGame.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnInGame.StateCommon.Content.Padding = new System.Windows.Forms.Padding(12, -1, -1, -1);
			this.chkbtnInGame.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnInGame.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnInGame.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnInGame.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_normal;
			this.chkbtnInGame.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnInGame_normal;
			this.chkbtnInGame.TabIndex = 8;
			this.chkbtnInGame.UseMnemonic = false;
			this.chkbtnInGame.Values.Image = global::ControlCenter.Properties.Resources.InGameSettings;
			this.chkbtnInGame.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnInGame.Values.Text = "In-Game Settings";
			this.chkbtnInGame.Click += new System.EventHandler(this.chktbtnInGame_Click);
			// 
			// chkbtnExceptions
			// 
			this.chkbtnExceptions.Location = new System.Drawing.Point(28, 260);
			this.chkbtnExceptions.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnExceptions.Name = "chkbtnExceptions";
			this.chkbtnExceptions.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnExceptions.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnExceptions.OverrideDefault.Border.Width = 0;
			this.chkbtnExceptions.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnExceptions.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnExceptions.Size = new System.Drawing.Size(202, 34);
			this.chkbtnExceptions.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnException_pressed;
			this.chkbtnExceptions.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnException_pressed;
			this.chkbtnExceptions.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnException_pressed;
			this.chkbtnExceptions.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnExceptions.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnException_normal;
			this.chkbtnExceptions.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnExceptions.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnExceptions.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnExceptions.StateCommon.Border.Width = 0;
			this.chkbtnExceptions.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnExceptions.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnExceptions.StateCommon.Content.Padding = new System.Windows.Forms.Padding(10, -1, -1, -1);
			this.chkbtnExceptions.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnExceptions.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnExceptions.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnExceptions.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnException_normal;
			this.chkbtnExceptions.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnException_normal;
			this.chkbtnExceptions.TabIndex = 9;
			this.chkbtnExceptions.UseMnemonic = false;
			this.chkbtnExceptions.Values.Image = global::ControlCenter.Properties.Resources.Exceptions;
			this.chkbtnExceptions.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnExceptions.Values.Text = "Exceptions";
			this.chkbtnExceptions.Click += new System.EventHandler(this.chktbtnExceptions_Click);
			// 
			// chkbtnUpdate
			// 
			this.chkbtnUpdate.Location = new System.Drawing.Point(29, 294);
			this.chkbtnUpdate.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnUpdate.Name = "chkbtnUpdate";
			this.chkbtnUpdate.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnUpdate.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnUpdate.OverrideDefault.Border.Width = 0;
			this.chkbtnUpdate.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnUpdate.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnUpdate.Size = new System.Drawing.Size(201, 34);
			this.chkbtnUpdate.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_pressed;
			this.chkbtnUpdate.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_pressed;
			this.chkbtnUpdate.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_pressed;
			this.chkbtnUpdate.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnUpdate.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_normal;
			this.chkbtnUpdate.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnUpdate.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnUpdate.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnUpdate.StateCommon.Border.Width = 0;
			this.chkbtnUpdate.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnUpdate.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnUpdate.StateCommon.Content.Padding = new System.Windows.Forms.Padding(12, -1, -1, -1);
			this.chkbtnUpdate.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnUpdate.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnUpdate.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnUpdate.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_normal;
			this.chkbtnUpdate.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_normal;
			this.chkbtnUpdate.TabIndex = 10;
			this.chkbtnUpdate.UseMnemonic = false;
			this.chkbtnUpdate.Values.Image = global::ControlCenter.Properties.Resources.Update;
			this.chkbtnUpdate.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnUpdate.Values.Text = "Update";
			this.chkbtnUpdate.Click += new System.EventHandler(this.chktbtnUpdate_Click);
			// 
			// chkbtnSettings
			// 
			this.chkbtnSettings.Location = new System.Drawing.Point(35, 328);
			this.chkbtnSettings.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnSettings.Name = "chkbtnSettings";
			this.chkbtnSettings.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSettings.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnSettings.OverrideDefault.Border.Width = 0;
			this.chkbtnSettings.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSettings.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSettings.Size = new System.Drawing.Size(195, 34);
			this.chkbtnSettings.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_pressed;
			this.chkbtnSettings.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_pressed;
			this.chkbtnSettings.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_pressed;
			this.chkbtnSettings.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnSettings.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_normal;
			this.chkbtnSettings.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnSettings.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnSettings.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnSettings.StateCommon.Border.Width = 0;
			this.chkbtnSettings.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnSettings.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnSettings.StateCommon.Content.Padding = new System.Windows.Forms.Padding(9, -1, -1, -1);
			this.chkbtnSettings.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnSettings.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnSettings.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnSettings.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_normal;
			this.chkbtnSettings.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSetting_normal;
			this.chkbtnSettings.TabIndex = 11;
			this.chkbtnSettings.UseMnemonic = false;
			this.chkbtnSettings.Values.Image = global::ControlCenter.Properties.Resources.DriverSettings;
			this.chkbtnSettings.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnSettings.Values.Text = "Driver Settings";
			this.chkbtnSettings.Click += new System.EventHandler(this.chktbtnSettings_Click);
			// 
			// chkbtnHelp
			// 
			this.chkbtnHelp.Location = new System.Drawing.Point(35, 362);
			this.chkbtnHelp.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnHelp.Name = "chkbtnHelp";
			this.chkbtnHelp.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnHelp.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnHelp.OverrideDefault.Border.Width = 0;
			this.chkbtnHelp.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnHelp.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnHelp.Size = new System.Drawing.Size(195, 34);
			this.chkbtnHelp.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_pressed;
			this.chkbtnHelp.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_pressed;
			this.chkbtnHelp.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_pressed;
			this.chkbtnHelp.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnHelp.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_normal;
			this.chkbtnHelp.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnHelp.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnHelp.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnHelp.StateCommon.Border.Width = 0;
			this.chkbtnHelp.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnHelp.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnHelp.StateCommon.Content.Padding = new System.Windows.Forms.Padding(20, -1, -1, -1);
			this.chkbtnHelp.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnHelp.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnHelp.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnHelp.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_normal;
			this.chkbtnHelp.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnHelp_normal;
			this.chkbtnHelp.TabIndex = 12;
			this.chkbtnHelp.UseMnemonic = false;
			this.chkbtnHelp.Values.Image = global::ControlCenter.Properties.Resources.Help;
			this.chkbtnHelp.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnHelp.Values.Text = "Help";
			this.chkbtnHelp.Click += new System.EventHandler(this.chktbtnHelp_Click);
			// 
			// chkbtnAbout
			// 
			this.chkbtnAbout.Location = new System.Drawing.Point(47, 396);
			this.chkbtnAbout.Margin = new System.Windows.Forms.Padding(0);
			this.chkbtnAbout.Name = "chkbtnAbout";
			this.chkbtnAbout.OverrideDefault.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAbout.OverrideDefault.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnAbout.OverrideDefault.Border.Width = 0;
			this.chkbtnAbout.OverrideDefault.Content.ShortText.MultiLine = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAbout.OverrideFocus.Content.DrawFocus = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAbout.Size = new System.Drawing.Size(183, 34);
			this.chkbtnAbout.StateCheckedNormal.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_pressed;
			this.chkbtnAbout.StateCheckedPressed.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_pressed;
			this.chkbtnAbout.StateCheckedTracking.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_pressed;
			this.chkbtnAbout.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.chkbtnAbout.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_normal;
			this.chkbtnAbout.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.chkbtnAbout.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.chkbtnAbout.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.chkbtnAbout.StateCommon.Border.Width = 0;
			this.chkbtnAbout.StateCommon.Content.AdjacentGap = 10;
			this.chkbtnAbout.StateCommon.Content.Image.ImageH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnAbout.StateCommon.Content.Padding = new System.Windows.Forms.Padding(26, -1, -1, -1);
			this.chkbtnAbout.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.chkbtnAbout.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbtnAbout.StateCommon.Content.ShortText.TextH = ComponentFactory.Krypton.Toolkit.PaletteRelativeAlign.Near;
			this.chkbtnAbout.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_normal;
			this.chkbtnAbout.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnAbout_normal;
			this.chkbtnAbout.TabIndex = 13;
			this.chkbtnAbout.UseMnemonic = false;
			this.chkbtnAbout.Values.Image = global::ControlCenter.Properties.Resources.About;
			this.chkbtnAbout.Values.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.chkbtnAbout.Values.Text = "About";
			this.chkbtnAbout.Click += new System.EventHandler(this.chktbtnAbout_Click);
			// 
			// csModes
			// 
			this.csModes.CheckButtons.Add(this.chkbtnSimple);
			this.csModes.CheckButtons.Add(this.chkbtnAdvanced);
			this.csModes.CheckedButton = this.chkbtnAdvanced;
			this.csModes.CheckedButtonChanged += new System.EventHandler(this.csModes_CheckedButtonChanged);
			// 
			// knPages
			// 
			this.knPages.Location = new System.Drawing.Point(250, 129);
			this.knPages.Name = "knPages";
			this.knPages.NavigatorMode = ComponentFactory.Krypton.Navigator.NavigatorMode.Panel;
			this.knPages.PageBackStyle = ComponentFactory.Krypton.Toolkit.PaletteBackStyle.PanelClient;
			this.knPages.Pages.AddRange(new ComponentFactory.Krypton.Navigator.KryptonPage[] {
            this.kpStatus,
            this.kpInGame,
            this.kpUpdate,
            this.kpSettings,
            this.kpHelp,
            this.kpAbout,
            this.kpExceptions});
			this.knPages.SelectedIndex = 1;
			this.knPages.Size = new System.Drawing.Size(459, 385);
			this.knPages.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.tabsBackground;
			this.knPages.StateCommon.Page.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.knPages.TabIndex = 14;
			// 
			// kpStatus
			// 
			this.kpStatus.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpStatus.Controls.Add(this.gbOutput);
			this.kpStatus.Controls.Add(this.gbStereoStatus);
			this.kpStatus.Flags = 65534;
			this.kpStatus.LastVisibleSet = true;
			this.kpStatus.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpStatus.Name = "kpStatus";
			this.kpStatus.Size = new System.Drawing.Size(459, 385);
			this.kpStatus.StateCommon.Page.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.kpStatus.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.pageStatus;
			this.kpStatus.Text = "";
			this.kpStatus.ToolTipTitle = "Page ToolTip";
			this.kpStatus.UniqueName = "94A5862ABCA54B216881640E067D10E4";
			// 
			// gbOutput
			// 
			this.gbOutput.Location = new System.Drawing.Point(3, 152);
			this.gbOutput.Name = "gbOutput";
			// 
			// gbOutput.Panel
			// 
			this.gbOutput.Panel.Controls.Add(this.cbOutputModes);
			this.gbOutput.Panel.Controls.Add(this.cbOutputs);
			this.gbOutput.Size = new System.Drawing.Size(456, 233);
			this.gbOutput.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbOutput.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbOutput.StateCommon.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.gbOutput.TabIndex = 1;
			this.gbOutput.Values.Heading = "Output";
			// 
			// cbOutputModes
			// 
			this.cbOutputModes.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbOutputModes.DropDownWidth = 195;
			this.cbOutputModes.Location = new System.Drawing.Point(225, 12);
			this.cbOutputModes.Name = "cbOutputModes";
			this.cbOutputModes.Size = new System.Drawing.Size(222, 24);
			this.cbOutputModes.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbOutputModes.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbOutputModes.TabIndex = 1;
			// 
			// cbOutputs
			// 
			this.cbOutputs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbOutputs.DropDownWidth = 208;
			this.cbOutputs.Location = new System.Drawing.Point(11, 12);
			this.cbOutputs.Name = "cbOutputs";
			this.cbOutputs.Size = new System.Drawing.Size(208, 24);
			this.cbOutputs.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbOutputs.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbOutputs.TabIndex = 0;
			this.cbOutputs.SelectedIndexChanged += new System.EventHandler(this.OutputChanged);
			// 
			// gbStereoStatus
			// 
			this.gbStereoStatus.Location = new System.Drawing.Point(4, 7);
			this.gbStereoStatus.Name = "gbStereoStatus";
			// 
			// gbStereoStatus.Panel
			// 
			this.gbStereoStatus.Panel.Controls.Add(this.rbDisableDriver);
			this.gbStereoStatus.Panel.Controls.Add(this.cbEnableStereoAtStartup);
			this.gbStereoStatus.Panel.Controls.Add(this.rbEnableDriver);
			this.gbStereoStatus.Size = new System.Drawing.Size(455, 121);
			this.gbStereoStatus.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbStereoStatus.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbStereoStatus.StateCommon.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.gbStereoStatus.TabIndex = 0;
			this.gbStereoStatus.Values.Heading = "Stereo Status";
			// 
			// rbDisableDriver
			// 
			this.rbDisableDriver.Location = new System.Drawing.Point(22, 71);
			this.rbDisableDriver.Name = "rbDisableDriver";
			this.rbDisableDriver.Size = new System.Drawing.Size(123, 22);
			this.rbDisableDriver.TabIndex = 2;
			this.rbDisableDriver.Values.Text = "Disable Driver";
			// 
			// cbEnableStereoAtStartup
			// 
			this.cbEnableStereoAtStartup.Location = new System.Drawing.Point(45, 41);
			this.cbEnableStereoAtStartup.Name = "cbEnableStereoAtStartup";
			this.cbEnableStereoAtStartup.Size = new System.Drawing.Size(277, 24);
			this.cbEnableStereoAtStartup.TabIndex = 1;
			this.cbEnableStereoAtStartup.Values.Text = "Enable stereo at application startup";
			// 
			// rbEnableDriver
			// 
			this.rbEnableDriver.Checked = true;
			this.rbEnableDriver.Location = new System.Drawing.Point(22, 15);
			this.rbEnableDriver.Name = "rbEnableDriver";
			this.rbEnableDriver.Size = new System.Drawing.Size(119, 22);
			this.rbEnableDriver.TabIndex = 0;
			this.rbEnableDriver.Values.Text = "Enable Driver";
			// 
			// kpInGame
			// 
			this.kpInGame.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpInGame.Controls.Add(this.knInGame);
			this.kpInGame.Controls.Add(this.btnBrowseExe);
			this.kpInGame.Controls.Add(this.tbExeFileName);
			this.kpInGame.Controls.Add(this.tbProfileName);
			this.kpInGame.Controls.Add(this.cbProfiles);
			this.kpInGame.Flags = 65534;
			this.kpInGame.LastVisibleSet = true;
			this.kpInGame.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpInGame.Name = "kpInGame";
			this.kpInGame.Size = new System.Drawing.Size(459, 385);
			this.kpInGame.Text = "";
			this.kpInGame.ToolTipTitle = "Page ToolTip";
			this.kpInGame.UniqueName = "D958ECE1EC944E19EEAFDB6F2E1B5C5B";
			// 
			// knInGame
			// 
			this.knInGame.Button.ButtonDisplayLogic = ComponentFactory.Krypton.Navigator.ButtonDisplayLogic.None;
			this.knInGame.Button.CloseButtonDisplay = ComponentFactory.Krypton.Navigator.ButtonDisplay.Hide;
			this.knInGame.Button.NextButtonDisplay = ComponentFactory.Krypton.Navigator.ButtonDisplay.Hide;
			this.knInGame.Button.PreviousButtonDisplay = ComponentFactory.Krypton.Navigator.ButtonDisplay.Hide;
			this.knInGame.Location = new System.Drawing.Point(3, 82);
			this.knInGame.Name = "knInGame";
			this.knInGame.NavigatorMode = ComponentFactory.Krypton.Navigator.NavigatorMode.BarRibbonTabGroup;
			this.knInGame.Pages.AddRange(new ComponentFactory.Krypton.Navigator.KryptonPage[] {
            this.kpGame,
            this.kpAutofocus,
            this.kpHotkeys});
			this.knInGame.SelectedIndex = 2;
			this.knInGame.Size = new System.Drawing.Size(456, 300);
			this.knInGame.StateCommon.Page.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.knInGame.StateCommon.Panel.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.knInGame.StateCommon.Tab.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.knInGame.StateCommon.Tab.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.knInGame.TabIndex = 5;
			this.knInGame.Text = "kryptonNavigator1";
			// 
			// kpGame
			// 
			this.kpGame.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpGame.Controls.Add(this.lblClipping);
			this.kpGame.Controls.Add(this.cbSeparationMode);
			this.kpGame.Controls.Add(this.lblSeparationMod);
			this.kpGame.Controls.Add(this.cbWindowMode);
			this.kpGame.Controls.Add(this.lblWindowedMode);
			this.kpGame.Controls.Add(this.udSeparationScale);
			this.kpGame.Controls.Add(this.lblSeparationScale);
			this.kpGame.Controls.Add(this.udStereoSeparation);
			this.kpGame.Controls.Add(this.lblStereoSeparation);
			this.kpGame.Controls.Add(this.lblClippingWidth);
			this.kpGame.Controls.Add(this.tbClippingWidth);
			this.kpGame.Controls.Add(this.chkbShadowsMono);
			this.kpGame.Controls.Add(this.chkbUseSimpleStereoProjectionMethod);
			this.kpGame.Controls.Add(this.chkbForceVSyncOff);
			this.kpGame.Controls.Add(this.chkbShowOSDMessages);
			this.kpGame.Controls.Add(this.chkbEnableClipping);
			this.kpGame.Controls.Add(this.chkbShowWizard);
			this.kpGame.Controls.Add(this.chkbDisableMouseLock);
			this.kpGame.Controls.Add(this.chkbShowFPS);
			this.kpGame.Flags = 63998;
			this.kpGame.LastVisibleSet = true;
			this.kpGame.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpGame.Name = "kpGame";
			this.kpGame.Size = new System.Drawing.Size(454, 267);
			this.kpGame.StateCommon.Page.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.kpGame.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.panelBackground;
			this.kpGame.Text = "Game";
			this.kpGame.ToolTipTitle = "Page ToolTip";
			this.kpGame.UniqueName = "8E7E73EED3E9437200BCDCBB3EFD3543";
			// 
			// lblClipping
			// 
			this.lblClipping.Location = new System.Drawing.Point(124, 142);
			this.lblClipping.Name = "lblClipping";
			this.lblClipping.Size = new System.Drawing.Size(6, 2);
			this.lblClipping.TabIndex = 18;
			this.lblClipping.Values.Text = "";
			// 
			// cbSeparationMode
			// 
			this.cbSeparationMode.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this.cbSeparationMode.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.cbSeparationMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbSeparationMode.DropDownWidth = 234;
			this.cbSeparationMode.Location = new System.Drawing.Point(203, 237);
			this.cbSeparationMode.Name = "cbSeparationMode";
			this.cbSeparationMode.Size = new System.Drawing.Size(233, 24);
			this.cbSeparationMode.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbSeparationMode.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbSeparationMode.TabIndex = 17;
			// 
			// lblSeparationMod
			// 
			this.lblSeparationMod.Location = new System.Drawing.Point(203, 212);
			this.lblSeparationMod.Name = "lblSeparationMod";
			this.lblSeparationMod.Size = new System.Drawing.Size(127, 22);
			this.lblSeparationMod.TabIndex = 16;
			this.lblSeparationMod.Values.Text = "Separation mode";
			// 
			// cbWindowMode
			// 
			this.cbWindowMode.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this.cbWindowMode.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.cbWindowMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbWindowMode.DropDownWidth = 181;
			this.cbWindowMode.Location = new System.Drawing.Point(12, 237);
			this.cbWindowMode.Name = "cbWindowMode";
			this.cbWindowMode.Size = new System.Drawing.Size(181, 24);
			this.cbWindowMode.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbWindowMode.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbWindowMode.TabIndex = 15;
			// 
			// lblWindowedMode
			// 
			this.lblWindowedMode.Location = new System.Drawing.Point(12, 212);
			this.lblWindowedMode.Name = "lblWindowedMode";
			this.lblWindowedMode.Size = new System.Drawing.Size(109, 22);
			this.lblWindowedMode.TabIndex = 14;
			this.lblWindowedMode.Values.Text = "Window mode";
			// 
			// udSeparationScale
			// 
			this.udSeparationScale.DecimalPlaces = 2;
			this.udSeparationScale.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
			this.udSeparationScale.Location = new System.Drawing.Point(331, 173);
			this.udSeparationScale.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
			this.udSeparationScale.Name = "udSeparationScale";
			this.udSeparationScale.Size = new System.Drawing.Size(106, 25);
			this.udSeparationScale.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udSeparationScale.TabIndex = 13;
			// 
			// lblSeparationScale
			// 
			this.lblSeparationScale.Location = new System.Drawing.Point(203, 172);
			this.lblSeparationScale.Name = "lblSeparationScale";
			this.lblSeparationScale.Size = new System.Drawing.Size(94, 22);
			this.lblSeparationScale.TabIndex = 12;
			this.lblSeparationScale.Values.Text = "Stereo scale";
			// 
			// udStereoSeparation
			// 
			this.udStereoSeparation.DecimalPlaces = 2;
			this.udStereoSeparation.Increment = new decimal(new int[] {
            5,
            0,
            0,
            65536});
			this.udStereoSeparation.Location = new System.Drawing.Point(330, 138);
			this.udStereoSeparation.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
			this.udStereoSeparation.Name = "udStereoSeparation";
			this.udStereoSeparation.Size = new System.Drawing.Size(106, 25);
			this.udStereoSeparation.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udStereoSeparation.TabIndex = 11;
			this.udStereoSeparation.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
			this.udStereoSeparation.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.udStereoSeparation_KeyPress);
			// 
			// lblStereoSeparation
			// 
			this.lblStereoSeparation.Location = new System.Drawing.Point(203, 144);
			this.lblStereoSeparation.Name = "lblStereoSeparation";
			this.lblStereoSeparation.Size = new System.Drawing.Size(131, 22);
			this.lblStereoSeparation.TabIndex = 10;
			this.lblStereoSeparation.Values.Text = "Stereo separation";
			// 
			// lblClippingWidth
			// 
			this.lblClippingWidth.Location = new System.Drawing.Point(15, 143);
			this.lblClippingWidth.Name = "lblClippingWidth";
			this.lblClippingWidth.Size = new System.Drawing.Size(108, 22);
			this.lblClippingWidth.TabIndex = 9;
			this.lblClippingWidth.Values.Text = "Clipping width";
			// 
			// tbClippingWidth
			// 
			this.tbClippingWidth.DrawBackground = true;
			this.tbClippingWidth.Location = new System.Drawing.Point(15, 172);
			this.tbClippingWidth.Maximum = 25;
			this.tbClippingWidth.Name = "tbClippingWidth";
			this.tbClippingWidth.Size = new System.Drawing.Size(178, 27);
			this.tbClippingWidth.TabIndex = 8;
			this.tbClippingWidth.Value = 3;
			this.tbClippingWidth.ValueChanged += new System.EventHandler(this.tbClippingWidth_ValueChanged);
			// 
			// chkbSwapLR
			// 
			this.chkbShadowsMono.Location = new System.Drawing.Point(203, 105);
			this.chkbShadowsMono.Name = "chkbShadowsMono";
			this.chkbShadowsMono.Size = new System.Drawing.Size(105, 24);
			this.chkbShadowsMono.TabIndex = 7;
			this.chkbShadowsMono.Values.Text = "Render shadows in mono";
			// 
			// chkbUseSimpleStereoProjectionMethod
			// 
			this.chkbUseSimpleStereoProjectionMethod.Location = new System.Drawing.Point(203, 75);
			this.chkbUseSimpleStereoProjectionMethod.Name = "chkbUseSimpleStereoProjectionMethod";
			this.chkbUseSimpleStereoProjectionMethod.Size = new System.Drawing.Size(285, 24);
			this.chkbUseSimpleStereoProjectionMethod.TabIndex = 6;
			this.chkbUseSimpleStereoProjectionMethod.Values.Text = "Use simple stereo projection method";
			// 
			// chkbForceVSyncOff
			// 
			this.chkbForceVSyncOff.Location = new System.Drawing.Point(203, 45);
			this.chkbForceVSyncOff.Name = "chkbForceVSyncOff";
			this.chkbForceVSyncOff.Size = new System.Drawing.Size(150, 24);
			this.chkbForceVSyncOff.TabIndex = 5;
			this.chkbForceVSyncOff.Values.Text = "Force V-Sync Off";
			// 
			// chkbShowOSDMessages
			// 
			this.chkbShowOSDMessages.Location = new System.Drawing.Point(203, 17);
			this.chkbShowOSDMessages.Name = "chkbShowOSDMessages";
			this.chkbShowOSDMessages.Size = new System.Drawing.Size(216, 24);
			this.chkbShowOSDMessages.TabIndex = 4;
			this.chkbShowOSDMessages.Values.Text = "Show on-screen messages";
			// 
			// chkbEnableClipping
			// 
			this.chkbEnableClipping.Location = new System.Drawing.Point(15, 75);
			this.chkbEnableClipping.Name = "chkbEnableClipping";
			this.chkbEnableClipping.Size = new System.Drawing.Size(138, 24);
			this.chkbEnableClipping.TabIndex = 3;
			this.chkbEnableClipping.Values.Text = "Enable clipping";
			// 
			// chkbShowWizard
			// 
			this.chkbShowWizard.Location = new System.Drawing.Point(15, 45);
			this.chkbShowWizard.Name = "chkbShowWizard";
			this.chkbShowWizard.Size = new System.Drawing.Size(124, 24);
			this.chkbShowWizard.TabIndex = 2;
			this.chkbShowWizard.Values.Text = "Show Wizard";
			// 
			// chkbDisableMouseLock
			// 
			this.chkbDisableMouseLock.Location = new System.Drawing.Point(15, 105);
			this.chkbDisableMouseLock.Name = "chkbDisableMouseLock";
			this.chkbDisableMouseLock.Size = new System.Drawing.Size(166, 24);
			this.chkbDisableMouseLock.TabIndex = 1;
			this.chkbDisableMouseLock.Values.Text = "Disable mouse lock";
			// 
			// chkbShowFPS
			// 
			this.chkbShowFPS.Location = new System.Drawing.Point(15, 17);
			this.chkbShowFPS.Name = "chkbShowFPS";
			this.chkbShowFPS.Size = new System.Drawing.Size(103, 24);
			this.chkbShowFPS.TabIndex = 0;
			this.chkbShowFPS.Values.Text = "Show FPS";
			// 
			// kpAutofocus
			// 
			this.kpAutofocus.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpAutofocus.Controls.Add(this.kryptonPanel1);
			this.kpAutofocus.Controls.Add(this.lblAutofocusBottom);
			this.kpAutofocus.Controls.Add(this.lblAutofocusTop);
			this.kpAutofocus.Controls.Add(this.tbTopBottom);
			this.kpAutofocus.Controls.Add(this.udAutofocusMaxShift);
			this.kpAutofocus.Controls.Add(this.udAutofocusSpeed);
			this.kpAutofocus.Controls.Add(this.udAutofocusHeight);
			this.kpAutofocus.Controls.Add(this.udAutofocusWidth);
			this.kpAutofocus.Controls.Add(this.lblAutofocusMaximumShift);
			this.kpAutofocus.Controls.Add(this.lblAutofocusSpeed);
			this.kpAutofocus.Controls.Add(this.lblAutofocusHeight);
			this.kpAutofocus.Controls.Add(this.lblAutofocusWidth);
			this.kpAutofocus.Flags = 65534;
			this.kpAutofocus.LastVisibleSet = true;
			this.kpAutofocus.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpAutofocus.Name = "kpAutofocus";
			this.kpAutofocus.Size = new System.Drawing.Size(454, 267);
			this.kpAutofocus.StateCommon.Page.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.kpAutofocus.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.panelBackground;
			this.kpAutofocus.Text = "Autofocus";
			this.kpAutofocus.ToolTipTitle = "Page ToolTip";
			this.kpAutofocus.UniqueName = "B38CF591549140CAC3864792B88089CF";
			// 
			// kryptonPanel1
			// 
			this.kryptonPanel1.Location = new System.Drawing.Point(212, 40);
			this.kryptonPanel1.Name = "kryptonPanel1";
			this.kryptonPanel1.Size = new System.Drawing.Size(160, 145);
			this.kryptonPanel1.StateCommon.Image = global::ControlCenter.Properties.Resources.panelAutofocus;
			this.kryptonPanel1.TabIndex = 11;
			// 
			// lblAutofocusBottom
			// 
			this.lblAutofocusBottom.Location = new System.Drawing.Point(385, 179);
			this.lblAutofocusBottom.Name = "lblAutofocusBottom";
			this.lblAutofocusBottom.Size = new System.Drawing.Size(61, 22);
			this.lblAutofocusBottom.TabIndex = 10;
			this.lblAutofocusBottom.Values.Text = "Bottom";
			// 
			// lblAutofocusTop
			// 
			this.lblAutofocusTop.Location = new System.Drawing.Point(400, 15);
			this.lblAutofocusTop.Name = "lblAutofocusTop";
			this.lblAutofocusTop.Size = new System.Drawing.Size(37, 22);
			this.lblAutofocusTop.TabIndex = 9;
			this.lblAutofocusTop.Values.Text = "Top";
			// 
			// tbTopBottom
			// 
			this.tbTopBottom.DrawBackground = true;
			this.tbTopBottom.Location = new System.Drawing.Point(400, 43);
			this.tbTopBottom.Maximum = 200;
			this.tbTopBottom.Name = "tbTopBottom";
			this.tbTopBottom.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.tbTopBottom.Size = new System.Drawing.Size(27, 128);
			this.tbTopBottom.TabIndex = 8;
			this.tbTopBottom.Value = 99;
			// 
			// udAutofocusMaxShift
			// 
			this.udAutofocusMaxShift.DecimalPlaces = 2;
			this.udAutofocusMaxShift.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
			this.udAutofocusMaxShift.Location = new System.Drawing.Point(76, 137);
			this.udAutofocusMaxShift.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
			this.udAutofocusMaxShift.Name = "udAutofocusMaxShift";
			this.udAutofocusMaxShift.Size = new System.Drawing.Size(121, 25);
			this.udAutofocusMaxShift.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udAutofocusMaxShift.TabIndex = 7;
			// 
			// udAutofocusSpeed
			// 
			this.udAutofocusSpeed.DecimalPlaces = 2;
			this.udAutofocusSpeed.Location = new System.Drawing.Point(77, 107);
			this.udAutofocusSpeed.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.udAutofocusSpeed.Name = "udAutofocusSpeed";
			this.udAutofocusSpeed.Size = new System.Drawing.Size(120, 25);
			this.udAutofocusSpeed.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udAutofocusSpeed.TabIndex = 6;
			// 
			// udAutofocusHeight
			// 
			this.udAutofocusHeight.Location = new System.Drawing.Point(77, 79);
			this.udAutofocusHeight.Maximum = new decimal(new int[] {
            40000,
            0,
            0,
            0});
			this.udAutofocusHeight.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.udAutofocusHeight.Name = "udAutofocusHeight";
			this.udAutofocusHeight.Size = new System.Drawing.Size(120, 25);
			this.udAutofocusHeight.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udAutofocusHeight.TabIndex = 5;
			this.udAutofocusHeight.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// udAutofocusWidth
			// 
			this.udAutofocusWidth.Location = new System.Drawing.Point(77, 50);
			this.udAutofocusWidth.Maximum = new decimal(new int[] {
            4000,
            0,
            0,
            0});
			this.udAutofocusWidth.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.udAutofocusWidth.Name = "udAutofocusWidth";
			this.udAutofocusWidth.Size = new System.Drawing.Size(120, 25);
			this.udAutofocusWidth.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.udAutofocusWidth.TabIndex = 4;
			this.udAutofocusWidth.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			// 
			// lblAutofocusMaximumShift
			// 
			this.lblAutofocusMaximumShift.Location = new System.Drawing.Point(12, 137);
			this.lblAutofocusMaximumShift.Name = "lblAutofocusMaximumShift";
			this.lblAutofocusMaximumShift.Size = new System.Drawing.Size(75, 22);
			this.lblAutofocusMaximumShift.TabIndex = 3;
			this.lblAutofocusMaximumShift.Values.Text = "Max Shift";
			// 
			// lblAutofocusSpeed
			// 
			this.lblAutofocusSpeed.Location = new System.Drawing.Point(12, 111);
			this.lblAutofocusSpeed.Name = "lblAutofocusSpeed";
			this.lblAutofocusSpeed.Size = new System.Drawing.Size(53, 22);
			this.lblAutofocusSpeed.TabIndex = 2;
			this.lblAutofocusSpeed.Values.Text = "Speed";
			// 
			// lblAutofocusHeight
			// 
			this.lblAutofocusHeight.Location = new System.Drawing.Point(12, 81);
			this.lblAutofocusHeight.Name = "lblAutofocusHeight";
			this.lblAutofocusHeight.Size = new System.Drawing.Size(56, 22);
			this.lblAutofocusHeight.TabIndex = 1;
			this.lblAutofocusHeight.Values.Text = "Height";
			// 
			// lblAutofocusWidth
			// 
			this.lblAutofocusWidth.Location = new System.Drawing.Point(12, 53);
			this.lblAutofocusWidth.Name = "lblAutofocusWidth";
			this.lblAutofocusWidth.Size = new System.Drawing.Size(51, 22);
			this.lblAutofocusWidth.TabIndex = 0;
			this.lblAutofocusWidth.Values.Text = "Width";
			// 
			// kpHotkeys
			// 
			this.kpHotkeys.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpHotkeys.Controls.Add(this.tbToggleHotkeysOSD);
			this.kpHotkeys.Controls.Add(this.lblToggleHotkeysOSD);
			this.kpHotkeys.Controls.Add(this.btnRestoreDefaults);
			this.kpHotkeys.Controls.Add(this.tbWizard);
			this.kpHotkeys.Controls.Add(this.lblToggleWizard);
			this.kpHotkeys.Controls.Add(this.tbStereoScreenshot);
			this.kpHotkeys.Controls.Add(this.lblTakeScreenshot);
			this.kpHotkeys.Controls.Add(this.tbToggleLaserSight);
			this.kpHotkeys.Controls.Add(this.lblToggleLasersight);
			this.kpHotkeys.Controls.Add(this.tbShowFPS);
			this.kpHotkeys.Controls.Add(this.lblToggleFPS);
			this.kpHotkeys.Controls.Add(this.tbSwapLR);
			this.kpHotkeys.Controls.Add(this.lblSwapLR);
			this.kpHotkeys.Controls.Add(this.tbToggleAutofocus);
			this.kpHotkeys.Controls.Add(this.lblToggleAutofocus);
			this.kpHotkeys.Controls.Add(this.tbPreset3);
			this.kpHotkeys.Controls.Add(this.lblPreset3);
			this.kpHotkeys.Controls.Add(this.tbPreset2);
			this.kpHotkeys.Controls.Add(this.lblPreset2);
			this.kpHotkeys.Controls.Add(this.tbPreset1);
			this.kpHotkeys.Controls.Add(this.lblPreset1);
			this.kpHotkeys.Controls.Add(this.tbToggleStereo);
			this.kpHotkeys.Controls.Add(this.lblToggleStereo);
			this.kpHotkeys.Controls.Add(this.tbDecSeparation);
			this.kpHotkeys.Controls.Add(this.lblDecSep);
			this.kpHotkeys.Controls.Add(this.tbIncSeparation);
			this.kpHotkeys.Controls.Add(this.lblIncSep);
			this.kpHotkeys.Controls.Add(this.tbDecConvergence);
			this.kpHotkeys.Controls.Add(this.lblDecConv);
			this.kpHotkeys.Controls.Add(this.tbIncConvergence);
			this.kpHotkeys.Controls.Add(this.lblIncConv);
			this.kpHotkeys.Flags = 65534;
			this.kpHotkeys.LastVisibleSet = true;
			this.kpHotkeys.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpHotkeys.Name = "kpHotkeys";
			this.kpHotkeys.Size = new System.Drawing.Size(454, 267);
			this.kpHotkeys.StateCommon.Page.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.True;
			this.kpHotkeys.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.panelBackground;
			this.kpHotkeys.Text = "Hotkeys";
			this.kpHotkeys.ToolTipTitle = "Page ToolTip";
			this.kpHotkeys.UniqueName = "18D7A456D022470CC9AE291BE1B28829";
			// 
			// tbToggleHotkeysOSD
			// 
			this.tbToggleHotkeysOSD.Location = new System.Drawing.Point(149, 203);
			this.tbToggleHotkeysOSD.Name = "tbToggleHotkeysOSD";
			this.tbToggleHotkeysOSD.Size = new System.Drawing.Size(75, 20);
			this.tbToggleHotkeysOSD.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbToggleHotkeysOSD.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbToggleHotkeysOSD.TabIndex = 32;
            this.tbToggleHotkeysOSD.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
            this.tbToggleHotkeysOSD.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleHotkeysOSD
			// 
			this.lblToggleHotkeysOSD.Location = new System.Drawing.Point(12, 205);
			this.lblToggleHotkeysOSD.Name = "lblToggleHotkeysOSD";
			this.lblToggleHotkeysOSD.Size = new System.Drawing.Size(123, 18);
			this.lblToggleHotkeysOSD.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleHotkeysOSD.TabIndex = 31;
			this.lblToggleHotkeysOSD.Values.Text = "Toggle Hotkeys OSD";
			// 
			// btnRestoreDefaults
			// 
			this.btnRestoreDefaults.Location = new System.Drawing.Point(339, 240);
			this.btnRestoreDefaults.Name = "btnRestoreDefaults";
			this.btnRestoreDefaults.Size = new System.Drawing.Size(98, 19);
			this.btnRestoreDefaults.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnRestore_norm;
			this.btnRestoreDefaults.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnRestoreDefaults.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnRestoreDefaults.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnRestoreDefaults.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnRestoreDefaults.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnRestoreDefaults.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnRestore_pressed;
			this.btnRestoreDefaults.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnRestore_over;
			this.btnRestoreDefaults.TabIndex = 30;
			this.btnRestoreDefaults.Values.Text = "Restore default";
			this.btnRestoreDefaults.Click += new System.EventHandler(this.btnRestoreDefaults_Click);
			// 
			// tbWizard
			// 
			this.tbWizard.Location = new System.Drawing.Point(149, 177);
			this.tbWizard.Name = "tbWizard";
			this.tbWizard.Size = new System.Drawing.Size(76, 20);
			this.tbWizard.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbWizard.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbWizard.TabIndex = 29;
			this.tbWizard.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbWizard.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleWizard
			// 
			this.lblToggleWizard.Location = new System.Drawing.Point(12, 179);
			this.lblToggleWizard.Name = "lblToggleWizard";
			this.lblToggleWizard.Size = new System.Drawing.Size(76, 18);
			this.lblToggleWizard.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleWizard.TabIndex = 28;
			this.lblToggleWizard.Values.Text = "iZ3D Wizard";
			// 
			// tbStereoScreenshot
			// 
			this.tbStereoScreenshot.Location = new System.Drawing.Point(365, 149);
			this.tbStereoScreenshot.Name = "tbStereoScreenshot";
			this.tbStereoScreenshot.Size = new System.Drawing.Size(76, 20);
			this.tbStereoScreenshot.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbStereoScreenshot.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbStereoScreenshot.TabIndex = 27;
			this.tbStereoScreenshot.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbStereoScreenshot.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblTakeScreenshot
			// 
			this.lblTakeScreenshot.Location = new System.Drawing.Point(233, 150);
			this.lblTakeScreenshot.Name = "lblTakeScreenshot";
			this.lblTakeScreenshot.Size = new System.Drawing.Size(110, 18);
			this.lblTakeScreenshot.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblTakeScreenshot.TabIndex = 26;
			this.lblTakeScreenshot.Values.Text = "Stereo Screenshot";
			// 
			// tbToggleLaserSight
			// 
			this.tbToggleLaserSight.Location = new System.Drawing.Point(365, 122);
			this.tbToggleLaserSight.Name = "tbToggleLaserSight";
			this.tbToggleLaserSight.Size = new System.Drawing.Size(76, 20);
			this.tbToggleLaserSight.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbToggleLaserSight.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbToggleLaserSight.TabIndex = 25;
			this.tbToggleLaserSight.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbToggleLaserSight.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleLasersight
			// 
			this.lblToggleLasersight.Location = new System.Drawing.Point(233, 123);
			this.lblToggleLasersight.Name = "lblToggleLasersight";
			this.lblToggleLasersight.Size = new System.Drawing.Size(107, 18);
			this.lblToggleLasersight.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleLasersight.TabIndex = 24;
			this.lblToggleLasersight.Values.Text = "Toggle Lasersight";
			// 
			// tbShowFPS
			// 
			this.tbShowFPS.Location = new System.Drawing.Point(365, 177);
			this.tbShowFPS.Name = "tbShowFPS";
			this.tbShowFPS.Size = new System.Drawing.Size(75, 20);
			this.tbShowFPS.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbShowFPS.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbShowFPS.TabIndex = 21;
			this.tbShowFPS.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbShowFPS.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleFPS
			// 
			this.lblToggleFPS.Location = new System.Drawing.Point(233, 179);
			this.lblToggleFPS.Name = "lblToggleFPS";
			this.lblToggleFPS.Size = new System.Drawing.Size(64, 18);
			this.lblToggleFPS.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleFPS.TabIndex = 20;
			this.lblToggleFPS.Values.Text = "Show FPS";
			// 
			// tbSwapLR
			// 
			this.tbSwapLR.Location = new System.Drawing.Point(149, 151);
			this.tbSwapLR.Name = "tbSwapLR";
			this.tbSwapLR.Size = new System.Drawing.Size(75, 20);
			this.tbSwapLR.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbSwapLR.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbSwapLR.TabIndex = 19;
			this.tbSwapLR.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbSwapLR.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblSwapLR
			// 
			this.lblSwapLR.Location = new System.Drawing.Point(12, 152);
			this.lblSwapLR.Name = "lblSwapLR";
			this.lblSwapLR.Size = new System.Drawing.Size(74, 18);
			this.lblSwapLR.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblSwapLR.TabIndex = 18;
			this.lblSwapLR.Values.Text = "Swap L && R";
			// 
			// tbToggleAutofocus
			// 
			this.tbToggleAutofocus.Location = new System.Drawing.Point(149, 124);
			this.tbToggleAutofocus.Name = "tbToggleAutofocus";
			this.tbToggleAutofocus.Size = new System.Drawing.Size(75, 20);
			this.tbToggleAutofocus.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbToggleAutofocus.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbToggleAutofocus.TabIndex = 17;
			this.tbToggleAutofocus.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbToggleAutofocus.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleAutofocus
			// 
			this.lblToggleAutofocus.Location = new System.Drawing.Point(12, 125);
			this.lblToggleAutofocus.Name = "lblToggleAutofocus";
			this.lblToggleAutofocus.Size = new System.Drawing.Size(105, 18);
			this.lblToggleAutofocus.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleAutofocus.TabIndex = 16;
			this.lblToggleAutofocus.Values.Text = "Toggle Autofocus";
			// 
			// tbPreset3
			// 
			this.tbPreset3.Location = new System.Drawing.Point(365, 97);
			this.tbPreset3.Name = "tbPreset3";
			this.tbPreset3.Size = new System.Drawing.Size(76, 20);
			this.tbPreset3.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbPreset3.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbPreset3.TabIndex = 15;
			this.tbPreset3.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbPreset3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblPreset3
			// 
			this.lblPreset3.Location = new System.Drawing.Point(233, 98);
			this.lblPreset3.Name = "lblPreset3";
			this.lblPreset3.Size = new System.Drawing.Size(55, 18);
			this.lblPreset3.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblPreset3.TabIndex = 14;
			this.lblPreset3.Values.Text = "Preset 3";
			// 
			// tbPreset2
			// 
			this.tbPreset2.Location = new System.Drawing.Point(365, 70);
			this.tbPreset2.Name = "tbPreset2";
			this.tbPreset2.Size = new System.Drawing.Size(76, 20);
			this.tbPreset2.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbPreset2.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbPreset2.TabIndex = 13;
			this.tbPreset2.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbPreset2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblPreset2
			// 
			this.lblPreset2.Location = new System.Drawing.Point(233, 71);
			this.lblPreset2.Name = "lblPreset2";
			this.lblPreset2.Size = new System.Drawing.Size(55, 18);
			this.lblPreset2.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblPreset2.TabIndex = 12;
			this.lblPreset2.Values.Text = "Preset 2";
			// 
			// tbPreset1
			// 
			this.tbPreset1.Location = new System.Drawing.Point(365, 43);
			this.tbPreset1.Name = "tbPreset1";
			this.tbPreset1.Size = new System.Drawing.Size(76, 20);
			this.tbPreset1.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbPreset1.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbPreset1.TabIndex = 11;
			this.tbPreset1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbPreset1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblPreset1
			// 
			this.lblPreset1.Location = new System.Drawing.Point(233, 44);
			this.lblPreset1.Name = "lblPreset1";
			this.lblPreset1.Size = new System.Drawing.Size(55, 18);
			this.lblPreset1.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblPreset1.TabIndex = 10;
			this.lblPreset1.Values.Text = "Preset 1";
			// 
			// tbToggleStereo
			// 
			this.tbToggleStereo.Location = new System.Drawing.Point(365, 16);
			this.tbToggleStereo.Name = "tbToggleStereo";
			this.tbToggleStereo.Size = new System.Drawing.Size(75, 20);
			this.tbToggleStereo.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbToggleStereo.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbToggleStereo.TabIndex = 9;
			this.tbToggleStereo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbToggleStereo.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblToggleStereo
			// 
			this.lblToggleStereo.Location = new System.Drawing.Point(233, 17);
			this.lblToggleStereo.Name = "lblToggleStereo";
			this.lblToggleStereo.Size = new System.Drawing.Size(86, 18);
			this.lblToggleStereo.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblToggleStereo.TabIndex = 8;
			this.lblToggleStereo.Values.Text = "Toggle Stereo";
			// 
			// tbDecSeparation
			// 
			this.tbDecSeparation.Location = new System.Drawing.Point(149, 97);
			this.tbDecSeparation.Name = "tbDecSeparation";
			this.tbDecSeparation.Size = new System.Drawing.Size(75, 20);
			this.tbDecSeparation.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbDecSeparation.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbDecSeparation.TabIndex = 7;
			this.tbDecSeparation.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbDecSeparation.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblDecSep
			// 
			this.lblDecSep.Location = new System.Drawing.Point(12, 98);
			this.lblDecSep.Name = "lblDecSep";
			this.lblDecSep.Size = new System.Drawing.Size(123, 18);
			this.lblDecSep.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblDecSep.TabIndex = 6;
			this.lblDecSep.Values.Text = "Decrease Separation";
			// 
			// tbIncSeparation
			// 
			this.tbIncSeparation.Location = new System.Drawing.Point(149, 70);
			this.tbIncSeparation.Name = "tbIncSeparation";
			this.tbIncSeparation.Size = new System.Drawing.Size(75, 20);
			this.tbIncSeparation.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbIncSeparation.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbIncSeparation.TabIndex = 5;
			this.tbIncSeparation.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbIncSeparation.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblIncSep
			// 
			this.lblIncSep.Location = new System.Drawing.Point(12, 71);
			this.lblIncSep.Name = "lblIncSep";
			this.lblIncSep.Size = new System.Drawing.Size(119, 18);
			this.lblIncSep.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblIncSep.TabIndex = 4;
			this.lblIncSep.Values.Text = "Increase Separation";
			// 
			// tbDecConvergence
			// 
			this.tbDecConvergence.Location = new System.Drawing.Point(149, 43);
			this.tbDecConvergence.Name = "tbDecConvergence";
			this.tbDecConvergence.Size = new System.Drawing.Size(75, 20);
			this.tbDecConvergence.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbDecConvergence.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbDecConvergence.TabIndex = 3;
			this.tbDecConvergence.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbDecConvergence.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblDecConv
			// 
			this.lblDecConv.Location = new System.Drawing.Point(12, 44);
			this.lblDecConv.Name = "lblDecConv";
			this.lblDecConv.Size = new System.Drawing.Size(135, 18);
			this.lblDecConv.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblDecConv.TabIndex = 2;
			this.lblDecConv.Values.Text = "Decrease Convergence";
			// 
			// tbIncConvergence
			// 
			this.tbIncConvergence.Location = new System.Drawing.Point(149, 16);
			this.tbIncConvergence.Name = "tbIncConvergence";
			this.tbIncConvergence.Size = new System.Drawing.Size(75, 20);
			this.tbIncConvergence.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbIncConvergence.StateCommon.Content.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.tbIncConvergence.TabIndex = 1;
			this.tbIncConvergence.KeyDown += new System.Windows.Forms.KeyEventHandler(this.HotKeyBoxKeyDown);
			this.tbIncConvergence.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.HotKeyBoxKeyPress);
			// 
			// lblIncConv
			// 
			this.lblIncConv.Location = new System.Drawing.Point(12, 17);
			this.lblIncConv.Name = "lblIncConv";
			this.lblIncConv.Size = new System.Drawing.Size(132, 18);
			this.lblIncConv.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblIncConv.TabIndex = 0;
			this.lblIncConv.Values.Text = "Increase Convergence";
			// 
			// btnBrowseExe
			// 
			this.btnBrowseExe.Location = new System.Drawing.Point(376, 39);
			this.btnBrowseExe.Name = "btnBrowseExe";
			this.btnBrowseExe.Size = new System.Drawing.Size(78, 19);
			this.btnBrowseExe.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_norm;
			this.btnBrowseExe.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnBrowseExe.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnBrowseExe.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnBrowseExe.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnBrowseExe.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnBrowseExe.StateDisabled.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_disabl;
			this.btnBrowseExe.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_pressed;
			this.btnBrowseExe.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_over;
			this.btnBrowseExe.TabIndex = 3;
			this.btnBrowseExe.Values.Text = "Browse";
			this.btnBrowseExe.Click += new System.EventHandler(this.btnBrowseExe_Click);
			// 
			// tbExeFileName
			// 
			this.tbExeFileName.AllowDrop = true;
			this.tbExeFileName.Location = new System.Drawing.Point(197, 38);
			this.tbExeFileName.Name = "tbExeFileName";
			this.tbExeFileName.ReadOnly = true;
			this.tbExeFileName.Size = new System.Drawing.Size(173, 23);
			this.tbExeFileName.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbExeFileName.TabIndex = 2;
			this.tbExeFileName.DragDrop += new System.Windows.Forms.DragEventHandler(this.tbExeFileName_DragDrop);
			this.tbExeFileName.DragEnter += new System.Windows.Forms.DragEventHandler(this.tbExeFileName_DragEnter);
			// 
			// tbProfileName
			// 
			this.tbProfileName.Location = new System.Drawing.Point(0, 38);
			this.tbProfileName.Name = "tbProfileName";
			this.tbProfileName.Size = new System.Drawing.Size(191, 23);
			this.tbProfileName.StateCommon.Content.Color1 = System.Drawing.Color.Black;
			this.tbProfileName.TabIndex = 1;
			this.tbProfileName.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbProfileName_KeyPress);
			// 
			// cbProfiles
			// 
			this.cbProfiles.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.RecentlyUsedList;
			this.cbProfiles.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbProfiles.DropDownWidth = 454;
			this.cbProfiles.Location = new System.Drawing.Point(0, 0);
			this.cbProfiles.Name = "cbProfiles";
			this.cbProfiles.Size = new System.Drawing.Size(456, 26);
			this.cbProfiles.StateCommon.ComboBox.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.cbProfiles.StateCommon.ComboBox.Border.Rounding = 3;
			this.cbProfiles.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbProfiles.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbProfiles.TabIndex = 0;
			this.cbProfiles.SelectedIndexChanged += new System.EventHandler(this.cbProfiles_SelectedIndexChanged);
			// 
			// kpUpdate
			// 
			this.kpUpdate.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpUpdate.Controls.Add(this.btnUpdate);
			this.kpUpdate.Controls.Add(this.progressBar1);
			this.kpUpdate.Controls.Add(this.lblUpdatesStatus);
			this.kpUpdate.Controls.Add(this.webBrowser1);
			this.kpUpdate.Flags = 65534;
			this.kpUpdate.LastVisibleSet = true;
			this.kpUpdate.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpUpdate.Name = "kpUpdate";
			this.kpUpdate.Size = new System.Drawing.Size(459, 385);
			this.kpUpdate.Text = "";
			this.kpUpdate.ToolTipTitle = "Page ToolTip";
			this.kpUpdate.UniqueName = "72D6CC682F48425BD390009903561957";
			// 
			// btnUpdate
			// 
			this.btnUpdate.Enabled = false;
			this.btnUpdate.Location = new System.Drawing.Point(189, 354);
			this.btnUpdate.Name = "btnUpdate";
			this.btnUpdate.Size = new System.Drawing.Size(80, 21);
			this.btnUpdate.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_norm;
			this.btnUpdate.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnUpdate.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnUpdate.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnUpdate.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnUpdate.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnUpdate.StateDisabled.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_disabled;
			this.btnUpdate.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_pressed1;
			this.btnUpdate.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnUpdate_over;
			this.btnUpdate.TabIndex = 3;
			this.btnUpdate.Values.Text = "Update";
			this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
			// 
			// progressBar1
			// 
			this.progressBar1.Location = new System.Drawing.Point(7, 325);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(445, 23);
			this.progressBar1.TabIndex = 2;
			// 
			// lblUpdatesStatus
			// 
			this.lblUpdatesStatus.Location = new System.Drawing.Point(166, 286);
			this.lblUpdatesStatus.Name = "lblUpdatesStatus";
			this.lblUpdatesStatus.Size = new System.Drawing.Size(6, 2);
			this.lblUpdatesStatus.TabIndex = 1;
			this.lblUpdatesStatus.Values.Text = "";
			// 
			// webBrowser1
			// 
			this.webBrowser1.Dock = System.Windows.Forms.DockStyle.Top;
			this.webBrowser1.Location = new System.Drawing.Point(0, 0);
			this.webBrowser1.MinimumSize = new System.Drawing.Size(20, 20);
			this.webBrowser1.Name = "webBrowser1";
			this.webBrowser1.Size = new System.Drawing.Size(459, 267);
			this.webBrowser1.TabIndex = 0;
			// 
			// kpSettings
			// 
			this.kpSettings.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpSettings.Controls.Add(this.gbLanguage);
			this.kpSettings.Controls.Add(this.gbSettings);
			this.kpSettings.Flags = 65534;
			this.kpSettings.LastVisibleSet = true;
			this.kpSettings.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpSettings.Name = "kpSettings";
			this.kpSettings.Size = new System.Drawing.Size(459, 385);
			this.kpSettings.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.pageSettings;
			this.kpSettings.Text = "";
			this.kpSettings.ToolTipTitle = "Page ToolTip";
			this.kpSettings.UniqueName = "0DAC43981FC14B97D18930E85A327241";
			// 
			// gbLanguage
			// 
			this.gbLanguage.CaptionOverlap = 1D;
			this.gbLanguage.Location = new System.Drawing.Point(3, 152);
			this.gbLanguage.Name = "gbLanguage";
			// 
			// gbLanguage.Panel
			// 
			this.gbLanguage.Panel.Controls.Add(this.cbLanguages);
			this.gbLanguage.Size = new System.Drawing.Size(456, 233);
			this.gbLanguage.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbLanguage.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbLanguage.StateCommon.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.gbLanguage.TabIndex = 2;
			this.gbLanguage.Values.Heading = "Language";
			// 
			// cbLanguages
			// 
			this.cbLanguages.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this.cbLanguages.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
			this.cbLanguages.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cbLanguages.DropDownWidth = 167;
			this.cbLanguages.Location = new System.Drawing.Point(11, 19);
			this.cbLanguages.Name = "cbLanguages";
			this.cbLanguages.Size = new System.Drawing.Size(208, 24);
			this.cbLanguages.Sorted = true;
			this.cbLanguages.StateCommon.ComboBox.Content.Color1 = System.Drawing.Color.Black;
			this.cbLanguages.StateCommon.Item.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.cbLanguages.TabIndex = 0;
			// 
			// gbSettings
			// 
			this.gbSettings.Location = new System.Drawing.Point(4, 8);
			this.gbSettings.Name = "gbSettings";
			// 
			// gbSettings.Panel
			// 
			this.gbSettings.Panel.Controls.Add(this.chkbEnableD3D1xDriver);
			this.gbSettings.Panel.Controls.Add(this.chkbLockMouse);
			this.gbSettings.Panel.Controls.Add(this.chkbUpdateOnHold);
			this.gbSettings.Panel.Controls.Add(this.chkbLaunchWithWindows);
			this.gbSettings.Size = new System.Drawing.Size(455, 115);
			this.gbSettings.StateCommon.Back.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbSettings.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.gbSettings.StateCommon.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.gbSettings.TabIndex = 1;
			this.gbSettings.Values.Heading = "Control Center Settings";
			// 
			// chkbEnableD3D1xDriver
			// 
			this.chkbEnableD3D1xDriver.Location = new System.Drawing.Point(260, 52);
			this.chkbEnableD3D1xDriver.Name = "chkbEnableD3D1xDriver";
			this.chkbEnableD3D1xDriver.Size = new System.Drawing.Size(186, 24);
			this.chkbEnableD3D1xDriver.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbEnableD3D1xDriver.TabIndex = 3;
			this.chkbEnableD3D1xDriver.Values.Text = "Enable Direct3D 1x driver";
			// 
			// chkbLockMouse
			// 
			this.chkbLockMouse.Location = new System.Drawing.Point(10, 52);
			this.chkbLockMouse.Name = "chkbLockMouse";
			this.chkbLockMouse.Size = new System.Drawing.Size(223, 24);
			this.chkbLockMouse.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbLockMouse.TabIndex = 2;
			this.chkbLockMouse.Values.Text = "Lock mouse on primary monitor";
			// 
			// chkbUpdateOnHold
			// 
			this.chkbUpdateOnHold.Location = new System.Drawing.Point(260, 19);
			this.chkbUpdateOnHold.Name = "chkbUpdateOnHold";
			this.chkbUpdateOnHold.Size = new System.Drawing.Size(174, 24);
			this.chkbUpdateOnHold.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbUpdateOnHold.TabIndex = 1;
			this.chkbUpdateOnHold.Values.Text = "Show update messages";
			// 
			// chkbLaunchWithWindows
			// 
			this.chkbLaunchWithWindows.Location = new System.Drawing.Point(10, 19);
			this.chkbLaunchWithWindows.Name = "chkbLaunchWithWindows";
			this.chkbLaunchWithWindows.Size = new System.Drawing.Size(253, 24);
			this.chkbLaunchWithWindows.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.chkbLaunchWithWindows.TabIndex = 0;
			this.chkbLaunchWithWindows.Values.Text = "Launch Control Center with Windows";
			// 
			// kpHelp
			// 
			this.kpHelp.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpHelp.Controls.Add(this.btnActivate);
			this.kpHelp.Controls.Add(this.btnSetupScreens);
			this.kpHelp.Controls.Add(this.btnTrobleshooting);
			this.kpHelp.Controls.Add(this.btnDiagnostic);
			this.kpHelp.Controls.Add(this.btnUserGuide);
			this.kpHelp.Controls.Add(this.btnStaticTest);
			this.kpHelp.Controls.Add(this.btnDynamicTest);
			this.kpHelp.Flags = 65534;
			this.kpHelp.LastVisibleSet = true;
			this.kpHelp.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpHelp.Name = "kpHelp";
			this.kpHelp.Size = new System.Drawing.Size(459, 385);
			this.kpHelp.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.pageHelp;
			this.kpHelp.Text = "";
			this.kpHelp.ToolTipTitle = "Page ToolTip";
			this.kpHelp.UniqueName = "9BFEC28C6E4246E77E9AAB540E7B5E7C";
			// 
			// btnActivate
			// 
			this.btnActivate.Location = new System.Drawing.Point(275, 283);
			this.btnActivate.Name = "btnActivate";
			this.btnActivate.Size = new System.Drawing.Size(120, 22);
			this.btnActivate.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnActivation_norm;
			this.btnActivate.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnActivate.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnActivate.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnActivation_pressed;
			this.btnActivate.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnActivation_over;
			this.btnActivate.TabIndex = 12;
			this.btnActivate.Values.Text = "Activate Driver";
			this.btnActivate.Click += new System.EventHandler(this.btnActivate_Click);
			// 
			// btnSetupScreens
			// 
			this.btnSetupScreens.Location = new System.Drawing.Point(275, 249);
			this.btnSetupScreens.Name = "btnSetupScreens";
			this.btnSetupScreens.Size = new System.Drawing.Size(120, 22);
			this.btnSetupScreens.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnSetupScreens_norm;
			this.btnSetupScreens.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnSetupScreens.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnSetupScreens.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnSetupScreens.StateDisabled.Back.Image = global::ControlCenter.Properties.Resources.btnSetupScreens_norm;
			this.btnSetupScreens.StateDisabled.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnSetupScreens.StateDisabled.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.btnSetupScreens.StateDisabled.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnSetupScreens.StateDisabled.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnSetupScreens.StateDisabled.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F);
			this.btnSetupScreens.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnSetupScreens_pressed;
			this.btnSetupScreens.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSetupScreens_over;
			this.btnSetupScreens.TabIndex = 11;
			this.btnSetupScreens.Values.Image = global::ControlCenter.Properties.Resources.SetupScreens;
			this.btnSetupScreens.Values.Text = "Setup Screens";
			this.btnSetupScreens.Click += new System.EventHandler(this.btnSetupScreens_Click);
			// 
			// btnTrobleshooting
			// 
			this.btnTrobleshooting.Location = new System.Drawing.Point(74, 250);
			this.btnTrobleshooting.Name = "btnTrobleshooting";
			this.btnTrobleshooting.Size = new System.Drawing.Size(120, 22);
			this.btnTrobleshooting.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnTrobleshooting_norm;
			this.btnTrobleshooting.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnTrobleshooting.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnTrobleshooting.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnTrobleshooting_pressed;
			this.btnTrobleshooting.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnTrobleshooting_over;
			this.btnTrobleshooting.TabIndex = 10;
			this.btnTrobleshooting.Values.Image = global::ControlCenter.Properties.Resources.Troubleshooting;
			this.btnTrobleshooting.Values.Text = "Troubleshooting";
			this.btnTrobleshooting.Click += new System.EventHandler(this.btnTrobleshooting_Click);
			// 
			// btnDiagnostic
			// 
			this.btnDiagnostic.Location = new System.Drawing.Point(275, 216);
			this.btnDiagnostic.Name = "btnDiagnostic";
			this.btnDiagnostic.Size = new System.Drawing.Size(120, 22);
			this.btnDiagnostic.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnDiagnostic_norm;
			this.btnDiagnostic.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnDiagnostic.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnDiagnostic.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnDiagnostic_pressed;
			this.btnDiagnostic.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnDiagnostic_over;
			this.btnDiagnostic.TabIndex = 9;
			this.btnDiagnostic.Values.Image = global::ControlCenter.Properties.Resources.Diagnostic;
			this.btnDiagnostic.Values.Text = "Diagnostic";
			this.btnDiagnostic.Click += new System.EventHandler(this.btnDiagnostic_Click);
			// 
			// btnUserGuide
			// 
			this.btnUserGuide.Location = new System.Drawing.Point(74, 214);
			this.btnUserGuide.Name = "btnUserGuide";
			this.btnUserGuide.Size = new System.Drawing.Size(120, 22);
			this.btnUserGuide.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnUserGuide_norm;
			this.btnUserGuide.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.Black;
			this.btnUserGuide.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnUserGuide.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnUserGuide_pressed;
			this.btnUserGuide.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnUserGuide_over;
			this.btnUserGuide.TabIndex = 8;
			this.btnUserGuide.Values.Image = global::ControlCenter.Properties.Resources.UserGuide;
			this.btnUserGuide.Values.Text = "User Guide";
			this.btnUserGuide.Click += new System.EventHandler(this.btnUserGuide_Click);
			// 
			// btnStaticTest
			// 
			this.btnStaticTest.Location = new System.Drawing.Point(242, 66);
			this.btnStaticTest.Name = "btnStaticTest";
			this.btnStaticTest.Size = new System.Drawing.Size(200, 122);
			this.btnStaticTest.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnSTest_norm;
			this.btnStaticTest.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.FromArgb(((int)(((byte)(108)))), ((int)(((byte)(108)))), ((int)(((byte)(108)))));
			this.btnStaticTest.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnStaticTest.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnSTest_pressed;
			this.btnStaticTest.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnSTest_over;
			this.btnStaticTest.TabIndex = 7;
			this.btnStaticTest.Values.Text = "Static Test";
			this.btnStaticTest.Click += new System.EventHandler(this.btnStaticTest_Click);
			// 
			// btnDynamicTest
			// 
			this.btnDynamicTest.Location = new System.Drawing.Point(28, 65);
			this.btnDynamicTest.Name = "btnDynamicTest";
			this.btnDynamicTest.Size = new System.Drawing.Size(200, 122);
			this.btnDynamicTest.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnDTest_norm;
			this.btnDynamicTest.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.FromArgb(((int)(((byte)(108)))), ((int)(((byte)(108)))), ((int)(((byte)(108)))));
			this.btnDynamicTest.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnDynamicTest.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnDTest_pressed;
			this.btnDynamicTest.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnDTest_over;
			this.btnDynamicTest.TabIndex = 6;
			this.btnDynamicTest.Values.Text = "Dynamic Test";
			this.btnDynamicTest.Click += new System.EventHandler(this.btnDTest_Click);
			// 
			// kpAbout
			// 
			this.kpAbout.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpAbout.Controls.Add(this.lblReleaseDate);
			this.kpAbout.Controls.Add(this.lblVersion);
			this.kpAbout.Controls.Add(this.lblCopyright);
			this.kpAbout.Controls.Add(this.lblReleaseDateC);
			this.kpAbout.Controls.Add(this.lblVersionC);
			this.kpAbout.Flags = 65534;
			this.kpAbout.LastVisibleSet = true;
			this.kpAbout.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpAbout.Name = "kpAbout";
			this.kpAbout.Size = new System.Drawing.Size(459, 385);
			this.kpAbout.StateCommon.Page.Image = global::ControlCenter.Properties.Resources.pageAbout;
			this.kpAbout.Text = "";
			this.kpAbout.ToolTipTitle = "Page ToolTip";
			this.kpAbout.UniqueName = "780F5746383B418089A860BC04952AF5";
			// 
			// lblReleaseDate
			// 
			this.lblReleaseDate.Location = new System.Drawing.Point(153, 229);
			this.lblReleaseDate.Name = "lblReleaseDate";
			this.lblReleaseDate.Size = new System.Drawing.Size(263, 20);
			this.lblReleaseDate.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F);
			this.lblReleaseDate.TabIndex = 4;
			this.lblReleaseDate.Values.Text = "2010/09/22 12:20:39 2005-2010 iZ3D Inc.";
			// 
			// lblVersion
			// 
			this.lblVersion.Location = new System.Drawing.Point(126, 201);
			this.lblVersion.Name = "lblVersion";
			this.lblVersion.Size = new System.Drawing.Size(37, 20);
			this.lblVersion.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F);
			this.lblVersion.TabIndex = 3;
			this.lblVersion.Values.Text = "1.13";
			// 
			// lblCopyright
			// 
			this.lblCopyright.Location = new System.Drawing.Point(83, 257);
			this.lblCopyright.Name = "lblCopyright";
			this.lblCopyright.Size = new System.Drawing.Size(328, 20);
			this.lblCopyright.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblCopyright.TabIndex = 2;
			this.lblCopyright.Values.Text = "All right reserved. iZ3DTM is a trademark of iZ3D Inc.";
			// 
			// lblReleaseDateC
			// 
			this.lblReleaseDateC.Location = new System.Drawing.Point(83, 229);
			this.lblReleaseDateC.Name = "lblReleaseDateC";
			this.lblReleaseDateC.Size = new System.Drawing.Size(91, 20);
			this.lblReleaseDateC.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblReleaseDateC.TabIndex = 1;
			this.lblReleaseDateC.Values.Text = "Release date:";
			// 
			// lblVersionC
			// 
			this.lblVersionC.Location = new System.Drawing.Point(83, 201);
			this.lblVersionC.Name = "lblVersionC";
			this.lblVersionC.Size = new System.Drawing.Size(54, 20);
			this.lblVersionC.StateCommon.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.lblVersionC.TabIndex = 0;
			this.lblVersionC.Values.Text = "Version";
			// 
			// kpExceptions
			// 
			this.kpExceptions.AutoHiddenSlideSize = new System.Drawing.Size(200, 200);
			this.kpExceptions.Controls.Add(this.btnRemove);
			this.kpExceptions.Controls.Add(this.btnAdd);
			this.kpExceptions.Controls.Add(this.lbEceptionsList);
			this.kpExceptions.Controls.Add(this.tbExceptionName);
			this.kpExceptions.Controls.Add(this.btnBrowse);
			this.kpExceptions.Controls.Add(this.tbExceptionFilename);
			this.kpExceptions.Flags = 65534;
			this.kpExceptions.LastVisibleSet = true;
			this.kpExceptions.MinimumSize = new System.Drawing.Size(50, 50);
			this.kpExceptions.Name = "kpExceptions";
			this.kpExceptions.Size = new System.Drawing.Size(459, 385);
			this.kpExceptions.Text = "";
			this.kpExceptions.ToolTipTitle = "Page ToolTip";
			this.kpExceptions.UniqueName = "43CBF90BE5D34DB5E4A21A4F02849789";
			// 
			// btnRemove
			// 
			this.btnRemove.Location = new System.Drawing.Point(355, 129);
			this.btnRemove.Name = "btnRemove";
			this.btnRemove.Size = new System.Drawing.Size(80, 21);
			this.btnRemove.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnRemove_exceptions_norm;
			this.btnRemove.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnRemove.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnRemove.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnRemove.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnRemove.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnRemove.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnRemove_exceptions_pressed;
			this.btnRemove.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnRemove_exceptions_over;
			this.btnRemove.TabIndex = 5;
			this.btnRemove.Values.Text = "Remove";
			this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
			// 
			// btnAdd
			// 
			this.btnAdd.Location = new System.Drawing.Point(355, 102);
			this.btnAdd.Name = "btnAdd";
			this.btnAdd.Size = new System.Drawing.Size(80, 21);
			this.btnAdd.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnAdd_exceptions_norm;
			this.btnAdd.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnAdd.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnAdd.StateCommon.Border.Width = 0;
			this.btnAdd.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnAdd.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnAdd.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnAdd.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnAdd_exceptions_pressed;
			this.btnAdd.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnAdd_exceptions_over;
			this.btnAdd.TabIndex = 4;
			this.btnAdd.Values.Text = "Add";
			this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
			// 
			// lbEceptionsList
			// 
			this.lbEceptionsList.AllowDrop = true;
			this.lbEceptionsList.Location = new System.Drawing.Point(26, 104);
			this.lbEceptionsList.Name = "lbEceptionsList";
			this.lbEceptionsList.Size = new System.Drawing.Size(320, 241);
			this.lbEceptionsList.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.exceptionList;
			this.lbEceptionsList.StateCommon.Border.Draw = ComponentFactory.Krypton.Toolkit.InheritBool.False;
			this.lbEceptionsList.StateCommon.Border.DrawBorders = ((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders)((((ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Top | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Bottom) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Left) 
            | ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.Right)));
			this.lbEceptionsList.TabIndex = 3;
			this.lbEceptionsList.DragDrop += new System.Windows.Forms.DragEventHandler(this.lbEceptionsList_DragDrop);
			this.lbEceptionsList.DragEnter += new System.Windows.Forms.DragEventHandler(this.lbEceptionsList_DragEnter);
			this.lbEceptionsList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lbEceptionsList_KeyDown);
			// 
			// tbExceptionName
			// 
			this.tbExceptionName.Location = new System.Drawing.Point(26, 68);
			this.tbExceptionName.Name = "tbExceptionName";
			this.tbExceptionName.Size = new System.Drawing.Size(409, 23);
			this.tbExceptionName.StateCommon.Content.Color1 = System.Drawing.SystemColors.ControlText;
			this.tbExceptionName.TabIndex = 2;
			// 
			// btnBrowse
			// 
			this.btnBrowse.Location = new System.Drawing.Point(355, 38);
			this.btnBrowse.Name = "btnBrowse";
			this.btnBrowse.Size = new System.Drawing.Size(80, 21);
			this.btnBrowse.StateCommon.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_exceptions_norm;
			this.btnBrowse.StateCommon.Back.ImageStyle = ComponentFactory.Krypton.Toolkit.PaletteImageStyle.CenterMiddle;
			this.btnBrowse.StateCommon.Border.DrawBorders = ComponentFactory.Krypton.Toolkit.PaletteDrawBorders.None;
			this.btnBrowse.StateCommon.Border.Width = 0;
			this.btnBrowse.StateCommon.Content.Padding = new System.Windows.Forms.Padding(-1, 2, -1, -1);
			this.btnBrowse.StateCommon.Content.ShortText.Color1 = System.Drawing.SystemColors.ControlText;
			this.btnBrowse.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.btnBrowse.StatePressed.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_exceptions_pressed;
			this.btnBrowse.StateTracking.Back.Image = global::ControlCenter.Properties.Resources.btnBrowse_exceptions_over;
			this.btnBrowse.TabIndex = 1;
			this.btnBrowse.Values.Text = "Browse...";
			this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
			// 
			// tbExceptionFilename
			// 
			this.tbExceptionFilename.AllowDrop = true;
			this.tbExceptionFilename.Location = new System.Drawing.Point(26, 38);
			this.tbExceptionFilename.Name = "tbExceptionFilename";
			this.tbExceptionFilename.Size = new System.Drawing.Size(321, 23);
			this.tbExceptionFilename.StateCommon.Content.Color1 = System.Drawing.SystemColors.ControlText;
			this.tbExceptionFilename.TabIndex = 0;
			this.tbExceptionFilename.DragDrop += new System.Windows.Forms.DragEventHandler(this.tbExceptionFilename_DragDrop);
			this.tbExceptionFilename.DragEnter += new System.Windows.Forms.DragEventHandler(this.lbEceptionsList_DragEnter);
			// 
			// kryptonManager1
			// 
			this.kryptonManager1.GlobalPalette = this.kryptonPalette1;
			this.kryptonManager1.GlobalPaletteMode = ComponentFactory.Krypton.Toolkit.PaletteModeManager.Custom;
			// 
			// kryptonPalette1
			// 
			this.kryptonPalette1.BasePaletteMode = ComponentFactory.Krypton.Toolkit.PaletteMode.SparklePurple;
			this.kryptonPalette1.Common.StateCommon.Content.LongText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.kryptonPalette1.Common.StateCommon.Content.ShortText.Color1 = System.Drawing.Color.White;
			this.kryptonPalette1.Common.StateCommon.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
			this.kryptonPalette1.Common.StateDisabled.Content.LongText.Font = new System.Drawing.Font("Tahoma", 9F);
			this.kryptonPalette1.Common.StateDisabled.Content.ShortText.Font = new System.Drawing.Font("Tahoma", 9F);
			this.kryptonPalette1.Images.CheckBox.CheckedDisabled = global::ControlCenter.Properties.Resources.chkbox_checked;
			this.kryptonPalette1.Images.CheckBox.CheckedNormal = global::ControlCenter.Properties.Resources.chkbox_checked;
			this.kryptonPalette1.Images.CheckBox.CheckedPressed = global::ControlCenter.Properties.Resources.chkbox_checked;
			this.kryptonPalette1.Images.CheckBox.CheckedTracking = global::ControlCenter.Properties.Resources.chkbox_checked;
			this.kryptonPalette1.Images.CheckBox.UncheckedDisabled = global::ControlCenter.Properties.Resources.chkbox_unchecked;
			this.kryptonPalette1.Images.CheckBox.UncheckedNormal = global::ControlCenter.Properties.Resources.chkbox_unchecked;
			this.kryptonPalette1.Images.CheckBox.UncheckedPressed = global::ControlCenter.Properties.Resources.chkbox_unchecked;
			this.kryptonPalette1.Images.CheckBox.UncheckedTracking = global::ControlCenter.Properties.Resources.chkbox_unchecked;
			this.kryptonPalette1.Images.RadioButton.CheckedNormal = global::ControlCenter.Properties.Resources.rb_checked;
			this.kryptonPalette1.Images.RadioButton.CheckedPressed = global::ControlCenter.Properties.Resources.rb_checked;
			this.kryptonPalette1.Images.RadioButton.CheckedTracking = global::ControlCenter.Properties.Resources.rb_checked;
			this.kryptonPalette1.Images.RadioButton.UncheckedNormal = global::ControlCenter.Properties.Resources.rb_unchecked;
			this.kryptonPalette1.Images.RadioButton.UncheckedPressed = global::ControlCenter.Properties.Resources.rb_unchecked;
			this.kryptonPalette1.Images.RadioButton.UncheckedTracking = global::ControlCenter.Properties.Resources.rb_unchecked;
			// 
			// notifyIcon1
			// 
			this.notifyIcon1.ContextMenuStrip = this.contextMenuStrip1;
			this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
			this.notifyIcon1.Text = "iZ3D Control Center";
			this.notifyIcon1.Visible = true;
			this.notifyIcon1.DoubleClick += new System.EventHandler(this.notifyIcon1_DoubleClick);
			// 
			// contextMenuStrip1
			// 
			this.contextMenuStrip1.Font = new System.Drawing.Font("Segoe UI", 9F);
			this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miEnableDriver,
            this.miDisableDriver,
            this.toolStripSeparator1,
            this.miExit});
			this.contextMenuStrip1.Name = "contextMenuStrip1";
			this.contextMenuStrip1.Size = new System.Drawing.Size(147, 76);
			// 
			// miEnableDriver
			// 
			this.miEnableDriver.Checked = true;
			this.miEnableDriver.CheckOnClick = true;
			this.miEnableDriver.CheckState = System.Windows.Forms.CheckState.Checked;
			this.miEnableDriver.Name = "miEnableDriver";
			this.miEnableDriver.Size = new System.Drawing.Size(146, 22);
			this.miEnableDriver.Text = "&Enable Driver";
			this.miEnableDriver.Click += new System.EventHandler(this.miEnableDriver_Click);
			// 
			// miDisableDriver
			// 
			this.miDisableDriver.CheckOnClick = true;
			this.miDisableDriver.Name = "miDisableDriver";
			this.miDisableDriver.Size = new System.Drawing.Size(146, 22);
			this.miDisableDriver.Text = "&Disable Driver";
			this.miDisableDriver.Click += new System.EventHandler(this.miDisableDriver_Click);
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(143, 6);
			// 
			// miExit
			// 
			this.miExit.Name = "miExit";
			this.miExit.Size = new System.Drawing.Size(146, 22);
			this.miExit.Text = "E&xit";
			this.miExit.Click += new System.EventHandler(this.miExit_Click);
			// 
			// openFileDialog
			// 
			this.openFileDialog.DefaultExt = "exe";
			this.openFileDialog.Filter = "Applications|*.exe|All files|*.*";
			// 
			// MainForm
			// 
			this.AcceptButton = this.btnApply;
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
			this.BackgroundImage = global::ControlCenter.Properties.Resources.BackG;
			this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
			this.ClientSize = new System.Drawing.Size(749, 564);
			this.Controls.Add(this.knPages);
			this.Controls.Add(this.chkbtnAbout);
			this.Controls.Add(this.chkbtnHelp);
			this.Controls.Add(this.chkbtnSettings);
			this.Controls.Add(this.chkbtnUpdate);
			this.Controls.Add(this.chkbtnExceptions);
			this.Controls.Add(this.chkbtnInGame);
			this.Controls.Add(this.chkbtnStatus);
			this.Controls.Add(this.chkbtnAdvanced);
			this.Controls.Add(this.chkbtnSimple);
			this.Controls.Add(this.btnApply);
			this.Controls.Add(this.btnClose);
			this.Controls.Add(this.btnMinimize);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.Name = "MainForm";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "iZ3D Control Center";
			this.TransparencyKey = System.Drawing.Color.Magenta;
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_Closing);
			this.Load += new System.EventHandler(this.MainForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.csTabs)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.csModes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.knPages)).EndInit();
			this.knPages.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.kpStatus)).EndInit();
			this.kpStatus.ResumeLayout(false);
			this.gbOutput.Panel.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gbOutput)).EndInit();
			this.gbOutput.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.cbOutputModes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.cbOutputs)).EndInit();
			this.gbStereoStatus.Panel.ResumeLayout(false);
			this.gbStereoStatus.Panel.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.gbStereoStatus)).EndInit();
			this.gbStereoStatus.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.kpInGame)).EndInit();
			this.kpInGame.ResumeLayout(false);
			this.kpInGame.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.knInGame)).EndInit();
			this.knInGame.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.kpGame)).EndInit();
			this.kpGame.ResumeLayout(false);
			this.kpGame.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbSeparationMode)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.cbWindowMode)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.kpAutofocus)).EndInit();
			this.kpAutofocus.ResumeLayout(false);
			this.kpAutofocus.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.kryptonPanel1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.kpHotkeys)).EndInit();
			this.kpHotkeys.ResumeLayout(false);
			this.kpHotkeys.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.cbProfiles)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.kpUpdate)).EndInit();
			this.kpUpdate.ResumeLayout(false);
			this.kpUpdate.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpSettings)).EndInit();
			this.kpSettings.ResumeLayout(false);
			this.gbLanguage.Panel.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.gbLanguage)).EndInit();
			this.gbLanguage.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.cbLanguages)).EndInit();
			this.gbSettings.Panel.ResumeLayout(false);
			this.gbSettings.Panel.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.gbSettings)).EndInit();
			this.gbSettings.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.kpHelp)).EndInit();
			this.kpHelp.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.kpAbout)).EndInit();
			this.kpAbout.ResumeLayout(false);
			this.kpAbout.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.kpExceptions)).EndInit();
			this.kpExceptions.ResumeLayout(false);
			this.kpExceptions.PerformLayout();
			this.contextMenuStrip1.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private ComponentFactory.Krypton.Toolkit.KryptonButton btnMinimize;
		private ComponentFactory.Krypton.Toolkit.KryptonButton btnClose;
		private ComponentFactory.Krypton.Toolkit.KryptonButton btnApply;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnSimple;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnAdvanced;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckSet csTabs;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnStatus;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnInGame;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnExceptions;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnUpdate;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnSettings;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnHelp;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckButton chkbtnAbout;
		private ComponentFactory.Krypton.Toolkit.KryptonCheckSet csModes;
        private ComponentFactory.Krypton.Navigator.KryptonNavigator knPages;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpStatus;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpInGame;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpUpdate;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpSettings;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpHelp;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpAbout;
        private ComponentFactory.Krypton.Navigator.KryptonNavigator knInGame;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpGame;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpAutofocus;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpHotkeys;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnBrowseExe;
		private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbExeFileName;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbProfiles;
        private ComponentFactory.Krypton.Toolkit.KryptonGroupBox gbLanguage;
        private ComponentFactory.Krypton.Toolkit.KryptonGroupBox gbSettings;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnSetupScreens;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnTrobleshooting;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnDiagnostic;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnUserGuide;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnStaticTest;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnDynamicTest;
        private ComponentFactory.Krypton.Navigator.KryptonPage kpExceptions;
        private ComponentFactory.Krypton.Toolkit.KryptonGroupBox gbOutput;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbOutputModes;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbOutputs;
        private ComponentFactory.Krypton.Toolkit.KryptonGroupBox gbStereoStatus;
        private ComponentFactory.Krypton.Toolkit.KryptonRadioButton rbDisableDriver;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox cbEnableStereoAtStartup;
        private ComponentFactory.Krypton.Toolkit.KryptonRadioButton rbEnableDriver;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnUpdate;
        private System.Windows.Forms.ProgressBar progressBar1;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblUpdatesStatus;
        private System.Windows.Forms.WebBrowser webBrowser1;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnRemove;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnAdd;
        private ComponentFactory.Krypton.Toolkit.KryptonListBox lbEceptionsList;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbExceptionName;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnBrowse;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbExceptionFilename;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbLanguages;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbEnableD3D1xDriver;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbLockMouse;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbUpdateOnHold;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbLaunchWithWindows;
        private ComponentFactory.Krypton.Toolkit.KryptonManager kryptonManager1;
        private ComponentFactory.Krypton.Toolkit.KryptonPalette kryptonPalette1;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblCopyright;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblReleaseDateC;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblVersionC;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbSeparationMode;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblSeparationMod;
        private ComponentFactory.Krypton.Toolkit.KryptonComboBox cbWindowMode;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblWindowedMode;
		private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udSeparationScale;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblSeparationScale;
		private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udStereoSeparation;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblStereoSeparation;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblClippingWidth;
        private ComponentFactory.Krypton.Toolkit.KryptonTrackBar tbClippingWidth;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbShadowsMono;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbUseSimpleStereoProjectionMethod;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbForceVSyncOff;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbShowOSDMessages;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbEnableClipping;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbShowWizard;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbDisableMouseLock;
        private ComponentFactory.Krypton.Toolkit.KryptonCheckBox chkbShowFPS;
        private ComponentFactory.Krypton.Toolkit.KryptonButton btnRestoreDefaults;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbWizard;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleWizard;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbStereoScreenshot;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblTakeScreenshot;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbToggleLaserSight;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleLasersight;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbShowFPS;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleFPS;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbSwapLR;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblSwapLR;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbToggleAutofocus;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleAutofocus;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbPreset3;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblPreset3;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbPreset2;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblPreset2;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbPreset1;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblPreset1;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbToggleStereo;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleStereo;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbDecSeparation;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblDecSep;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbIncSeparation;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblIncSep;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbDecConvergence;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblDecConv;
        private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbIncConvergence;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblIncConv;
        private ComponentFactory.Krypton.Toolkit.KryptonPanel kryptonPanel1;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusBottom;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusTop;
        private ComponentFactory.Krypton.Toolkit.KryptonTrackBar tbTopBottom;
		private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udAutofocusMaxShift;
        private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udAutofocusSpeed;
        private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udAutofocusHeight;
        private ComponentFactory.Krypton.Toolkit.KryptonNumericUpDown udAutofocusWidth;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusMaximumShift;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusSpeed;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusHeight;
        private ComponentFactory.Krypton.Toolkit.KryptonLabel lblAutofocusWidth;
		private System.Windows.Forms.NotifyIcon notifyIcon1;
		private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
		private System.Windows.Forms.ToolStripMenuItem miEnableDriver;
		private System.Windows.Forms.ToolStripMenuItem miDisableDriver;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripMenuItem miExit;
		private ComponentFactory.Krypton.Toolkit.KryptonLabel lblVersion;
		private ComponentFactory.Krypton.Toolkit.KryptonLabel lblReleaseDate;
		private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbProfileName;
		private System.Windows.Forms.OpenFileDialog openFileDialog;
		private ComponentFactory.Krypton.Toolkit.KryptonLabel lblClipping;
		private ComponentFactory.Krypton.Toolkit.KryptonButton btnActivate;
		private ComponentFactory.Krypton.Toolkit.KryptonTextBox tbToggleHotkeysOSD;
		private ComponentFactory.Krypton.Toolkit.KryptonLabel lblToggleHotkeysOSD;




	}
}

