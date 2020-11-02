/* IZ3D_FILE: $Id: Utils.cpp 3056 2010-03-12 00:23:31Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 3056 $
* $Date: 2010-03-12 03:23:31 +0300 (Ïò, 12 ìàð 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/trunk/Control%20Center/guilib/Utils.cpp $
*/
using System;
using System.Collections;
using System.Diagnostics;
using ControlCenter.Data;

namespace ControlCenter
{
    [Flags]
    public enum OutputParams
	{
		DualMonitors	= 0x01,
		Shutter			= 0x02,
        Protected       = 0x04,
        ScalingRequired = 0x08,
        iZ3DMonitor     = 0x10,
    }

	class OutputDll
	{
        public class Mode
        {
            public String   name;
            public String   localizedName;
            public int      id;

            public Mode(String name, int id)
            {
                this.name = name;
                this.id = id;
                this.localizedName = StringLocalization.Localize(this.name);
            }

			public void UpdateLocalizedText()
			{
				this.localizedName = StringLocalization.Localize(this.name);
			}
        };

		public OutputDll(string         name,
                         string         outputDllName,
                         string[]       modes, 
                         OutputParams   parameters)
		{
            this.name = name;
            this.localizedName = StringLocalization.Localize(this.name);
            this.dllName = outputDllName;
            this.modes = new Mode[modes.Length];
            for (int i = 0; i < modes.Length; ++i)
            {
                this.modes[i] = new Mode(modes[i], i);
            }
            this.parameters = parameters;
		}

        public OutputDll(string         name,
                         string         outputDllName,
                         Mode[]         modes,
                         OutputParams   parameters)
        {
            this.name = name;
            this.localizedName = StringLocalization.Localize(this.name);
            this.dllName = outputDllName;
            this.modes = modes;
            this.parameters = parameters;
        }

		public void UpdateLocalizedText()
		{
			localizedName = StringLocalization.Localize(this.name);
			foreach (var mode in modes)
				mode.UpdateLocalizedText();
		}

        private string name;
        public string Name
        {
            get { return name; }
        }
		private string dllName;
		public string DLLName
		{
            get { return dllName; }
		}
		private Mode[] modes;
        public Mode[]Modes
		{
			get { return modes; }
		}
		private OutputParams parameters;
		public OutputParams Parameters
		{
			get { return parameters; }
		}

		private string localizedName;
		public string LocalizedName
		{
			get { return localizedName; }
		}

		public override string ToString()
		{
			return localizedName;
		}
	}
}
