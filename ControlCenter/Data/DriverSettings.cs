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
	public class DriverSettings : INotifyPropertyChanged, ICloneable
	{
		#region Data

        [ProfileValue("AutoStart")]
        bool _LaunchWithWindows;
        [ProfileValue("ShowUpdates")]
        bool _ShowUpdates;
        [ProfileValue("TestUpdaterURL")]
        bool _TestUpdaterURL;

        [ProfileValue("LockCursor")]
        bool _LockMouse;
        [ProfileValue("Language")]
        string _Language;

        [ProfileValue("DisableD3D10Driver")]
        bool _DisableD3D1xDriver;

		#endregion

		#region Properties

		public bool LaunchWithWindows
		{
			get { return _LaunchWithWindows; }
			set
			{
				_LaunchWithWindows = value;

				OnPropertyChanged("LaunchWithWindows");
			}
        }
        public bool ShowUpdates
        {
            get { return _ShowUpdates; }
            set
            {
                _ShowUpdates = value;

                OnPropertyChanged("ShowUpdates");
            }
        }
        public bool TestUpdaterURL
        {
            get { return _TestUpdaterURL; }
            set
            {
                _TestUpdaterURL = value;

                OnPropertyChanged("TestUpdaterURL");
            }
        }
        public bool LockMouse
        {
            get { return _LockMouse; }
            set
            {
                _LockMouse = value;

                OnPropertyChanged("LockMouse");
            }
        }
		public bool EnableD3D1xDriver
		{
			get { return !_DisableD3D1xDriver; }
			set
			{
				_DisableD3D1xDriver = !value;

				OnPropertyChanged("EnableD3D1xDriver");
			}
		}

		public string Language
		{
			get { return _Language; }
			set
			{
				_Language = value;

				OnPropertyChanged("Language");
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
			DriverSettings NewSettings = new DriverSettings();

			NewSettings._LaunchWithWindows = _LaunchWithWindows;
			NewSettings._LockMouse = _LockMouse;
			NewSettings._ShowUpdates = _ShowUpdates;
			NewSettings._TestUpdaterURL = _TestUpdaterURL;
            NewSettings._DisableD3D1xDriver = _DisableD3D1xDriver;
			NewSettings._Language = _Language;


			return NewSettings;
		}

		#endregion
	}
}
