/* IZ3D_FILE: $Id: Profile.cs 4299 2010-12-10 09:38:46Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 4299 $
* $Date: 2010-12-10 12:38:46 +0300 (Ïò, 10 äåê 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/branch/CCRedesignCSharp/ControlCenter/Data/Profile.cs $
*/
using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace ControlCenter.Data
{
	public class ProfileKeys : INotifyPropertyChanged, ICloneable
	{
		public void SetDefaultValues()
		{
			_ToggleStereo = new HotKey(106);
			_IncreaseConvergence = new HotKey(363);
			_DecreaseConvergence = new HotKey(365);
			_IncreaseSeparation = new HotKey(107);
			_DecreaseSeparation = new HotKey(109);
			_Preset1 = new HotKey(103);
			_Preset2 = new HotKey(104);
			_Preset3 = new HotKey(105);
			_SwapEyes = new HotKey(631);
			_ToggleAutofocus = new HotKey(111);
			_ToggleLasersight = new HotKey(623);
			_StereoScreenshot = new HotKey(44);
			_ToggleWizard = new HotKey(362);
			_ToggleFPS = new HotKey(0);
            _ToggleHotkeysOSD = new HotKey(368);
			_IncreasePresenterSleepTime = new HotKey(221);
			_DecreasePresenterSleepTime = new HotKey(219);
			_IncreasePresenterFlushTime = new HotKey(186);
			_DecreasePresenterFlushTime = new HotKey(76);
			_ShowDebugInfo = new HotKey(618);
			_IncreaseZNear = new HotKey(875);
			_DecreaseZNear = new HotKey(877);
			_IncreaseMultiplier = new HotKey(619);
			_DecreaseMultiplier = new HotKey(621);
			_IncreaseConvergenceShift = new HotKey(1131);
			_DecreaseConvergenceShift = new HotKey(1133);
		}

		#region Data
		[ProfileValue("ToggleStereo", 106)]
		private HotKey _ToggleStereo;
		[ProfileValue("ZPSIncrease", 363)]
		private HotKey _IncreaseConvergence;
		[ProfileValue("ZPSDecrease", 365)]
		private HotKey _DecreaseConvergence;
		[ProfileValue("StereoBaseIncrease", 107)]
		private HotKey _IncreaseSeparation;
		[ProfileValue("StereoBaseDecrease", 109)]
		private HotKey _DecreaseSeparation;
		[ProfileValue("SetPreset1", 103)]
		private HotKey _Preset1;
		[ProfileValue("SetPreset2", 104)]
		private HotKey _Preset2;
		[ProfileValue("SetPreset3", 105)]
		private HotKey _Preset3;
		[ProfileValue("SwapEyes", 631)]
		private HotKey _SwapEyes;
		[ProfileValue("ToggleAutoFocus", 111)]
		private HotKey _ToggleAutofocus;
		[ProfileValue("ToggleLaserSight", 623)]
		private HotKey _ToggleLasersight;
		[ProfileValue("Screenshot", 44)]
		private HotKey _StereoScreenshot;
		[ProfileValue("ToggleWizard", 362)]
		private HotKey _ToggleWizard;
		[ProfileValue("ToggleFPS", 0)]
		private HotKey _ToggleFPS;
		[ProfileValue("ToggleHotkeysOSD", 368)]
		private HotKey _ToggleHotkeysOSD;
		[ProfileValue("PresenterSleepTimeIncrease", 221)]
		private HotKey _IncreasePresenterSleepTime;
		[ProfileValue("PresenterSleepTimeDecrease", 219)]
		private HotKey _DecreasePresenterSleepTime;
		[ProfileValue("PresenterFlushTimeIncrease", 186)]
		private HotKey _IncreasePresenterFlushTime;
		[ProfileValue("PresenterFlushTimeDecrease", 76)]
		private HotKey _DecreasePresenterFlushTime;
		[ProfileValue("ShowDebugInfo", 618)]
		private HotKey _ShowDebugInfo;
		[ProfileValue("ZNearIncrease", 875)]
		private HotKey _IncreaseZNear;
		[ProfileValue("ZNearDecrease", 877)]
		private HotKey _DecreaseZNear;
		[ProfileValue("MultiplierIncrease", 619)]
		private HotKey _IncreaseMultiplier;
		[ProfileValue("MultiplierDecrease", 621)]
		private HotKey _DecreaseMultiplier;
		[ProfileValue("ConvergenceShiftIncrease", 1131)]
		private HotKey _IncreaseConvergenceShift;
		[ProfileValue("ConvergenceShiftDecrease", 1133)]
		private HotKey _DecreaseConvergenceShift;
		#endregion

		#region Properties - HotKeys
		public HotKey IncreaseConvergence
		{
			get { return _IncreaseConvergence; }
			set
			{
				_IncreaseConvergence = value;

				OnPropertyChanged("IncreaseConvergence");
			}
		}
		public HotKey DecreaseConvergence
		{
			get { return _DecreaseConvergence; }
			set
			{
				_DecreaseConvergence = value;

				OnPropertyChanged("DecreaseConvergence");
			}
		}
		public HotKey IncreaseSeparation
		{
			get { return _IncreaseSeparation; }
			set
			{
				_IncreaseSeparation = value;

				OnPropertyChanged("IncreaseSeparation");
			}
		}
		public HotKey DecreaseSeparation
		{
			get { return _DecreaseSeparation; }
			set
			{
				_DecreaseSeparation = value;

				OnPropertyChanged("DecreaseSeparation");
			}
		}
		public HotKey ToggleAutofocus
		{
			get { return _ToggleAutofocus; }
			set
			{
				_ToggleAutofocus = value;

				OnPropertyChanged("ToggleAutofocus");
			}
		}
		public HotKey SwapEyes
		{
			get { return _SwapEyes; }
			set
			{
				_SwapEyes = value;

				OnPropertyChanged("SwapEyes");
			}
		}
		public HotKey ToggleStereo
		{
			get { return _ToggleStereo; }
			set
			{
				_ToggleStereo = value;

				OnPropertyChanged("ToggleStereo");
			}
		}
		public HotKey Preset1
		{
			get { return _Preset1; }
			set
			{
				_Preset1 = value;

				OnPropertyChanged("Preset1");
			}
		}
		public HotKey Preset2
		{
			get { return _Preset2; }
			set
			{
				_Preset2 = value;

				OnPropertyChanged("Preset2");
			}
		}
		public HotKey Preset3
		{
			get { return _Preset3; }
			set
			{
				_Preset3 = value;

				OnPropertyChanged("Preset3");
			}
		}
		public HotKey ToggleLasersight
		{
			get { return _ToggleLasersight; }
			set
			{
				_ToggleLasersight = value;

				OnPropertyChanged("ToggleLasersight");
			}
		}
		public HotKey StereoScreenshot
		{
			get { return _StereoScreenshot; }
			set
			{
				_StereoScreenshot = value;

				OnPropertyChanged("StereoScreenshot");
			}
		}
		public HotKey ToggleWizard
		{
			get { return _ToggleWizard; }
			set
			{
				_ToggleWizard = value;

				OnPropertyChanged("ToggleWizard");
			}
		}
		public HotKey ToggleFPS
		{
			get { return _ToggleFPS; }
			set
			{
				_ToggleFPS = value;

				OnPropertyChanged("ToggleFPS");
			}
		}
		public HotKey ToggleHotkeysOSD
		{
			get { return _ToggleHotkeysOSD; }
			set
			{
				_ToggleHotkeysOSD = value;

				OnPropertyChanged("ToggleHotkeysOSD");
			}
		}
		#endregion

		#region INotifyPropertyChanged Members

		public event PropertyChangedEventHandler PropertyChanged;

		protected void OnPropertyChanged(string name)
		{
			PropertyChangedEventHandler handler = PropertyChanged;
			if (handler != null)
			{
				handler(this, new PropertyChangedEventArgs(name));
			}
		}

		#endregion

		#region ICloneable Members

		public object Clone()
		{
			ProfileKeys NewKeys = new ProfileKeys();
			NewKeys._ToggleStereo = _ToggleStereo.Clone() as HotKey;
			NewKeys._IncreaseConvergence = _IncreaseConvergence.Clone() as HotKey;
			NewKeys._DecreaseConvergence = _DecreaseConvergence.Clone() as HotKey;
			NewKeys._IncreaseSeparation = _IncreaseSeparation.Clone() as HotKey;
			NewKeys._DecreaseSeparation = _DecreaseSeparation.Clone() as HotKey;
			NewKeys._Preset1 = _Preset1.Clone() as HotKey;
			NewKeys._Preset2 = _Preset2.Clone() as HotKey;
			NewKeys._Preset3 = _Preset3.Clone() as HotKey;
			NewKeys._SwapEyes = _SwapEyes.Clone() as HotKey;
			NewKeys._ToggleAutofocus = _ToggleAutofocus.Clone() as HotKey;
			NewKeys._ToggleLasersight = _ToggleLasersight.Clone() as HotKey;
			NewKeys._StereoScreenshot = _StereoScreenshot.Clone() as HotKey;
			NewKeys._ToggleWizard = _ToggleWizard.Clone() as HotKey;
			NewKeys._ToggleFPS = _ToggleFPS.Clone() as HotKey;
			NewKeys._ToggleHotkeysOSD = _ToggleHotkeysOSD.Clone() as HotKey;
			NewKeys._IncreasePresenterSleepTime = _IncreasePresenterSleepTime.Clone() as HotKey;
			NewKeys._DecreasePresenterSleepTime = _DecreasePresenterSleepTime.Clone() as HotKey;
			NewKeys._IncreasePresenterFlushTime = _IncreasePresenterFlushTime.Clone() as HotKey;
			NewKeys._DecreasePresenterFlushTime = _DecreasePresenterFlushTime.Clone() as HotKey;
			NewKeys._ShowDebugInfo = _ShowDebugInfo.Clone() as HotKey;
			NewKeys._IncreaseZNear = _IncreaseZNear.Clone() as HotKey;
			NewKeys._DecreaseZNear = _DecreaseZNear.Clone() as HotKey;
			NewKeys._IncreaseMultiplier = _IncreaseMultiplier.Clone() as HotKey;
			NewKeys._DecreaseMultiplier = _DecreaseMultiplier.Clone() as HotKey;
			NewKeys._IncreaseConvergenceShift = _IncreaseConvergenceShift.Clone() as HotKey;
			NewKeys._DecreaseConvergenceShift = _DecreaseConvergenceShift.Clone() as HotKey;
			return NewKeys;
		}

		#endregion
	}
}
