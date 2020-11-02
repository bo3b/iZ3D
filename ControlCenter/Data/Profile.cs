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
using System.ComponentModel;
using System.Reflection;
using System.Diagnostics;

namespace ControlCenter.Data
{
	public class Profile : INotifyPropertyChanged, ICloneable, IComparable<Profile>
    {
        public override string ToString()
		{
			return _Name;
		}

        public Profile CreateUserProfile()
        {
            Profile userProfile = new Profile();
            userProfile._Files = new List<FileData>(_Files.Count);
            for (int i = 0; i < _Files.Count; i++)
            {
                userProfile._Files.Add((FileData)_Files[i].Clone());
            }
            userProfile._Name = _Name;
            userProfile._PreviousName = _PreviousName;
            userProfile.BaseProfile = this;
            return userProfile;
        }

        public static Profile CreateDefaultProfile()
        {
            Profile p = new Profile();
            p._UserChangeProfileName = false;
            p._ShowFPS = false;
            p._ShowOnscreenMessages = false;
            p._SeparationMode = 0;
            p._EnableClipping = false;
            p._ClippingWidth = 0.03f;
            p._PresenterSleepTime = -2;
            p._WindowMode = 3;
            p._ForceVSyncOff = false;
            p._UseSimpleStereoProjectionMethod = false;
            p._SeparationScale = 1.0f;
            p._AutofocusWidth = 512;
            p._AutofocusHeight = 64;
            p._AutofocusSpeed = 1;
            p._AutofocusMaximumShift = 0.2f;
            p._AutofocusVerticalPosition = 0.495f;
            p._ShowWizard = true;
            p._SwapLnR = false;
            p._DisableMouseLock = false;
            p.Keys.SetDefaultValues();
            for (int i = 0; i < 3; i++)
            {
                ProfilePreset preset = new ProfilePreset();
                preset.SetDefaultValues();
                p.Presets.Add(preset);
            }
            return p;
        }
        
        public void ApplyValues(Profile profile)
        {
            Type type = typeof(Profile);
            foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
            {
                ProfileValueAttribute attr = null;
                if (ProfileValueAttribute.IsProfileKey(field, out attr))
                {
                    object thisValue = field.GetValue(this);
                    object defValue = field.GetValue(profile);
                    //Debug.Assert(defValue == null);
                    if (defValue != null && thisValue == null)
                        field.SetValue(this, defValue);
                }
            }

            // PRESETS SECTION
            for (int i = 0; i < 3; i++)
            {
                if (i >= profile.Presets.Count)
                    break;
                if (i >= _Presets.Count)
                {
                    ProfilePreset preset = new ProfilePreset();
                    _Presets.Add(preset);
                }
                bool bDefPreset = (_Presets[i] == profile.Presets[i]);
                type = typeof(ProfilePreset);
                foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
                {
                    ProfileValueAttribute attr = null;
                    if (ProfileValueAttribute.IsProfileKey(field, out attr))
                    {
                        object thisValue = field.GetValue(_Presets[i]);
                        object defValue = field.GetValue(profile.Presets[i]);
                        //Debug.Assert(defValue == null);
                        if (defValue != null && thisValue == null)
                            field.SetValue(_Presets[i], defValue);
                    }
                }
            }

            // KEYS SECTION
            ProfileKeys defaultKeys = profile.Keys;
            bool bDefProfile = (profile.Keys == defaultKeys);
            type = typeof(ProfileKeys);
            foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
            {
                ProfileValueAttribute attr = null;
                if (ProfileValueAttribute.IsProfileKey(field, out attr))
                {
                    object thisValue = field.GetValue(Keys);
                    object defValue = field.GetValue(defaultKeys);
                    //Debug.Assert(defValue == null);
                    if (defValue != null && thisValue == null)
                        field.SetValue(Keys, defValue);
                }
            }
        }

		//public enum WindowModeEnum { Fullscreen, Windowed, Auto }
		//public enum SeparationModeEnum { Symmetric, RightShift, LeftShift }
                
        #region Data
		private List<FileData> _Files = new List<FileData>();
        private ProfileKeys _Keys = new ProfileKeys();
        private List<ProfilePreset> _Presets = new List<ProfilePreset>(3);

		private string _Name;
		private string _PreviousName;

        [ProfileValue("UserChangeProfileName", UserProfilesOnly = true)]
        private bool? _UserChangeProfileName;
        [ProfileValue("ShowFPS")]
        private bool? _ShowFPS;
        [ProfileValue("ShowOSD")]
        private bool? _ShowOnscreenMessages;
        [ProfileValue("SeparationMode")]
        private int? _SeparationMode;
        [ProfileValue("FrustumAdjustMode")]
        private bool? _EnableClipping;
        [ProfileValue("BlackAreaWidth", 0.03f)]
        private float? _ClippingWidth;
        [ProfileValue("PresenterSleepTime", -2)]
        private int? _PresenterSleepTime;
        [ProfileValue("DeviceMode", 3)]
        private int? _WindowMode;
        [ProfileValue("ForceVSyncOff")]
        private bool? _ForceVSyncOff;
        [ProfileValue("UseSimpleStereoProjectionMethod")]
        private bool? _UseSimpleStereoProjectionMethod;
        [ProfileValue("SeparationScale", 1.0f)]
        private float? _SeparationScale;
        [ProfileValue("SearchFrameSizeX", 512, UserProfilesOnly = true)]
        private int? _AutofocusWidth;
		[ProfileValue("SearchFrameSizeY", 64, UserProfilesOnly = true)]
        private int? _AutofocusHeight;
		[ProfileValue("ShiftSpeed", 1, UserProfilesOnly = true)]
        private int? _AutofocusSpeed;
		[ProfileValue("Max_ABS_rZPS", 0.2f, UserProfilesOnly = true)]
        private float? _AutofocusMaximumShift;
		[ProfileValue("FrameYCoordinate", 0.495f, UserProfilesOnly = true)]
        private float? _AutofocusVerticalPosition;
        [ProfileValue("ShowWizardAtStartup", true)]
		private bool? _ShowWizard;
		[ProfileValue("SwapEyes")]
		private bool? _SwapLnR;
		[ProfileValue("CreateSquareRTInMono")]
		private bool? _CreateSquareRTInMono;
		[ProfileValue("CreateRTThatLessThanBBInMono")]
		private bool? _CreateRTThatLessThanBBInMono;
		[ProfileValue("CreateSquareDSInMono")]
		private bool? _CreateSquareDSInMono;
		[ProfileValue("CreateDSThatLessThanBBInMono")]
		private bool? _CreateDSThatLessThanBBInMono;
        [ProfileValue("DontLockMouse")]
		private bool? _DisableMouseLock;
        
		#endregion

        #region Properties
        public bool Changed { get; set; }
        public ProfileKeys Keys
        {
            get { return _Keys; }
        }
        public List<ProfilePreset> Presets
        {
            get { return _Presets; }
        }
        public float StereoSeparation
        {
            get
            {
                float sep = 0.16f;
                if (_Presets.Count > 0)
                    sep = (float)_Presets[0].StereoSeparation;
                return Utils.SeparationToPercents(sep);
            }
            set
            {
                float sep = Utils.PercentsToSeparation(value);
                foreach (var p in _Presets)
                {
                    p.StereoSeparation = sep;
                }
            }
        }
        public bool? ShowFPS
        {
            get { return _ShowFPS; }
            set
            {
                _ShowFPS = value;

                OnPropertyChanged("ShowFPS");
            }
        }
		public bool? DisableMouseLock
		{
			get { return _DisableMouseLock; }
			set
			{
				_DisableMouseLock = value;

				OnPropertyChanged("DisableMouseLock");
			}
		}
		public bool? ShowWizard
		{
			get { return _ShowWizard; }
			set
			{
				_ShowWizard = value;

				OnPropertyChanged("ShowWizard");
			}
		}
		public bool? EnableClipping
		{
			get { return _EnableClipping; }
			set
			{
				_EnableClipping = value;

				OnPropertyChanged("EnableClipping");
			}
		}
		public bool? ShowOnscreenMessages
		{
			get { return _ShowOnscreenMessages; }
			set
			{
				_ShowOnscreenMessages = value;

				OnPropertyChanged("ShowOnscreenMessages");
			}
		}
		public bool? ForceVSyncOff
		{
			get { return _ForceVSyncOff; }
			set
			{
				_ForceVSyncOff = value;

				OnPropertyChanged("ForceVSyncOff");
			}
		}
		public bool? UseSimpleStereoProjectionMethod
		{
			get { return _UseSimpleStereoProjectionMethod; }
			set
			{
				_UseSimpleStereoProjectionMethod = value;

				OnPropertyChanged("UseSimpleStereoProjectionMethod");
			}
		}
		public bool? SwapLnR
		{
			get { return _SwapLnR; }
			set
			{
				_SwapLnR = value;

				OnPropertyChanged("SwapLnR");
			}
		}
		public bool? CreateSquareRTInMono
		{
			get { return _CreateSquareRTInMono; }
			set
			{
				_CreateSquareRTInMono = value;

				OnPropertyChanged("CreateSquareRTInMono");
			}
		}
		public bool? CreateRTThatLessThanBBInMono
		{
			get { return _CreateRTThatLessThanBBInMono; }
			set
			{
				_CreateRTThatLessThanBBInMono = value;

				OnPropertyChanged("CreateRTThatLessThanBBInMono");
			}
		}
		public bool? CreateSquareDSInMono
		{
			get { return _CreateSquareDSInMono; }
			set
			{
				_CreateSquareDSInMono = value;

				OnPropertyChanged("CreateSquareDSInMono");
			}
		}
		public bool? CreateDSThatLessThanBBInMono
		{
			get { return _CreateDSThatLessThanBBInMono; }
			set
			{
				_CreateDSThatLessThanBBInMono = value;

				OnPropertyChanged("CreateDSThatLessThanBBInMono");
			}
		}
		private bool? cmp(params bool?[] values)
		{
			bool? result = null;
			foreach (bool? v in values)
			{				
				if (v == true)
					return true;
				if (v == false)
					result = v;
			}
			return result;
		}
		public bool? ShadowsMono
		{
			get { return cmp(_CreateSquareRTInMono, _CreateRTThatLessThanBBInMono,
				_CreateSquareDSInMono, _CreateDSThatLessThanBBInMono);
			}
			set
			{
				_CreateSquareRTInMono = value;
				_CreateRTThatLessThanBBInMono = value;
				_CreateSquareDSInMono = value;
				_CreateDSThatLessThanBBInMono = value;
			}
		}
        public bool? UserChangeProfileName
        {
            get { return _UserChangeProfileName; }
            set
            {
                _UserChangeProfileName = value;

                OnPropertyChanged("UserChangeProfileName");
            }
        }
		public float? SeparationScale
		{
			get { return _SeparationScale; }
			set
			{
				_SeparationScale = value;

				OnPropertyChanged("SeparationScale");
			}
		}
        public float? ClippingWidth
        {
            get { return _ClippingWidth; }
            set
            {
                _ClippingWidth = value;

                OnPropertyChanged("ClippingWidth");
            }
        }
        public int? PresenterSleepTime
        {
            get { return _PresenterSleepTime; }
            set
            {
                _PresenterSleepTime = value;

                OnPropertyChanged("PresenterSleepTime");
            }
        }
		public int? WindowMode
		{
            get 
            {
                if (_WindowMode != null)
                    return _WindowMode < 2 ? _WindowMode : 2;
                else
                    return null;
            }
			set
            {
                if (value != null)
                    _WindowMode = value >= 2 ? 3 : value;
                else
                    _WindowMode = null;

				OnPropertyChanged("WindowMode");
			}
		}
		public int? SeparationMode
		{
			get { return _SeparationMode; }
			set
			{
				_SeparationMode = value;

				OnPropertyChanged("SeparationMode");
			}
		}
		public int? AutofocusWidth
		{
			get { return _AutofocusWidth; }
			set
			{
				_AutofocusWidth = value;

				OnPropertyChanged("AutofocusWidth");
			}
		}
		public int? AutofocusHeight
		{
			get { return _AutofocusHeight; }
			set
			{
				_AutofocusHeight = value;

				OnPropertyChanged("AutofocusHeight");
			}
		}
		public int? AutofocusSpeed
		{
			get { return _AutofocusSpeed; }
			set
			{
				_AutofocusSpeed = value;

				OnPropertyChanged("AutofocusSpeed");
			}
		}
		public float? AutofocusMaximumShift
		{
			get { return _AutofocusMaximumShift; }
			set
			{
				_AutofocusMaximumShift = value;

				OnPropertyChanged("AutofocusMaximumShift");
			}
		}
		public float? AutofocusVerticalPosition
		{
            get 
            {
                if (_AutofocusVerticalPosition != null)
                    return 1.0f - _AutofocusVerticalPosition;
                else
                    return null;
            }
			set
            {
                if (value != null)
                    _AutofocusVerticalPosition = 1.0f - value;
                else
                    _AutofocusVerticalPosition = null;

				OnPropertyChanged("AutofocusVerticalPosition");
			}
		}
		public string Name
		{
			get { return _Name; }
			set
			{
				_Name = value;

				OnPropertyChanged("Name");
			}
		}
		public string PreviousName
		{
			get { return _PreviousName; }
			set
			{
				_PreviousName = value;

				OnPropertyChanged("PreviousName");
			}
		}
        public string Filename
        {
            get { return _Files.Count == 0 ? "" : _Files[0].filename; }
            set
            {
                if (!String.IsNullOrEmpty(value))
                {
                    if (_Files.Count == 0)
                        _Files.Add(new FileData());
                    _Files[0].filename = value;
                }
                else if (_Files.Count > 0)
                    _Files.RemoveAt(0);

                OnPropertyChanged("Filename");
            }
        }
        public List<FileData> Files
        {
            get { return _Files; }
            set
            {
                _Files = value;

				OnPropertyChanged("Files");
            }
        }
        public Profile BaseProfile
        { get; set; }
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
            Profile NewProfile = new Profile();

            NewProfile._Files = new List<FileData>(_Files.Count);
            for (int i = 0; i < _Files.Count; i++)
            {
                NewProfile._Files.Add((FileData)_Files[i].Clone());
			}
			NewProfile._Name = _Name;
            NewProfile._PreviousName = _PreviousName;

            NewProfile._Keys = Keys.Clone() as ProfileKeys;
            NewProfile._Presets = new List<ProfilePreset>(_Presets.Count);
            for (int i = 0; i < _Presets.Count; i++)
            {
                NewProfile._Presets.Add(_Presets[i].Clone() as ProfilePreset);
            }
            NewProfile._ShowFPS = _ShowFPS;
            NewProfile._DisableMouseLock = _DisableMouseLock;
            NewProfile._ShowWizard = _ShowWizard;
            NewProfile._EnableClipping = _EnableClipping;
            NewProfile._ShowOnscreenMessages = _ShowOnscreenMessages;
            NewProfile._ForceVSyncOff = _ForceVSyncOff;
            NewProfile._UseSimpleStereoProjectionMethod = _UseSimpleStereoProjectionMethod;
            NewProfile._SwapLnR = _SwapLnR;
            NewProfile._SeparationScale = _SeparationScale;
            NewProfile._ClippingWidth = _ClippingWidth;
            NewProfile._WindowMode = _WindowMode;
            NewProfile._SeparationMode = _SeparationMode;
            NewProfile._AutofocusWidth = _AutofocusWidth;
            NewProfile._AutofocusHeight = _AutofocusHeight;
            NewProfile._AutofocusSpeed = _AutofocusSpeed;
            NewProfile._AutofocusMaximumShift = _AutofocusMaximumShift;
            NewProfile._AutofocusVerticalPosition = _AutofocusVerticalPosition;
            NewProfile._PresenterSleepTime = _PresenterSleepTime;

            return NewProfile;
        }

        #endregion

		#region IComparable<Profile> Members

		public int CompareTo(Profile other)
		{
			return _Name.CompareTo(other._Name);
		}

		#endregion
	}
}
