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
    public class ProfilePreset : INotifyPropertyChanged, ICloneable
    {
        public void SetDefaultValues()
        {
            _One_div_ZPS = 0.1f;
            _StereoSeparation = 0.16f;
            _AutoFocusEnable = false;
        }

        #region Data
		[ProfileValue("One_div_ZPS", 0.1f, BaseProfileOnly = true)]
        private float? _One_div_ZPS;
		[ProfileValue("StereoBase", 0.16f, BaseProfileOnly = true)]
        private float? _StereoSeparation;
		[ProfileValue("AutoFocusEnable", false, BaseProfileOnly = true)]
        private bool? _AutoFocusEnable;
        #endregion

		#region Properties
		public float? One_div_ZPS
		{
			get { return _One_div_ZPS; }
			set
			{
				_One_div_ZPS = value;

				OnPropertyChanged("One_div_ZPS");
			}
		}
		public float? StereoSeparation
		{
			get { return _StereoSeparation; }
			set
			{
				_StereoSeparation = value;

				OnPropertyChanged("StereoSeparation");
			}
		}
		public bool? AutoFocusEnable
		{
			get { return _AutoFocusEnable; }
			set
			{
				_AutoFocusEnable = value;

				OnPropertyChanged("AutoFocusEnable");
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
            ProfilePreset NewPreset = new ProfilePreset();
            NewPreset._AutoFocusEnable = _AutoFocusEnable;
            NewPreset._One_div_ZPS = _One_div_ZPS;
            NewPreset._StereoSeparation = _StereoSeparation;
            return NewPreset;
        }

        #endregion
    }
}
