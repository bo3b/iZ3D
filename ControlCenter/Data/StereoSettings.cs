/* IZ3D_FILE: $Id: Data.cs 4273 2010-12-06 06:02:46Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 4273 $
* $Date: 2010-12-06 09:02:46 +0300 (Пн, 06 дек 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/branch/CCRedesignCSharp/ControlCenter/Data/Data.cs $
*/
using System;
using System.ComponentModel;

namespace ControlCenter.Data
{
	public class StereoSettings : INotifyPropertyChanged, ICloneable
    {
		#region Data

		[ProfileValue("RunAtStartup")]
		private bool _DriverEnabled;
		[ProfileValue("EnableStereo")]
		private bool _StereoEnabled;
		[ProfileValue("OutputMethodDll")]
		private string _Output;
		[ProfileValue("OutputMode")]
		private int _Mode;
		[ProfileValue("DisplayScalingMode")]
		int _ScalingMode;

		#endregion

		#region Properties

		public bool DriverEnabled
		{
			get { return _DriverEnabled; }
			set
			{
				_DriverEnabled = value;

				OnPropertyChanged("DriverEnabled");
			}
		}
		public bool StereoEnabled
		{
			get { return _StereoEnabled; }
			set
			{
				_StereoEnabled = value;

				OnPropertyChanged("StereoEnabled");
			}
		}
		public string Output
		{
			get { return _Output; }
			set
			{
				_Output = value;

				OnPropertyChanged("Output");
			}
		}
		public int Mode
		{
			get { return _Mode; }
			set
			{
				_Mode = value;

				OnPropertyChanged("Mode");
			}
		}
		public int ScalingMode
		{
			get { return _ScalingMode; }
			set
			{
				_ScalingMode = value;

				OnPropertyChanged("ScalingMode");
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
            StereoSettings NewSettings = new StereoSettings();

            NewSettings._DriverEnabled = _DriverEnabled;
            NewSettings._StereoEnabled = _StereoEnabled;
            NewSettings._Output = _Output;
            NewSettings._Mode = _Mode;

            return NewSettings;
        }

        #endregion
    }
}
