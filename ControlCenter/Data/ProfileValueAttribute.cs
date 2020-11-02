/* IZ3D_FILE: $Id: DataHolder.cs 4309 2010-12-10 18:33:07Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 4309 $
* $Date: 2010-12-10 21:33:07 +0300 (Ïò, 10 äåê 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/branch/CCRedesignCSharp/ControlCenter/Data/DataHolder.cs $
*/
using System;
using System.Reflection;

namespace ControlCenter.Data
{
    [AttributeUsage(AttributeTargets.Field)]
    class ProfileValueAttribute : System.Attribute
    {
        public ProfileValueAttribute(string name)
        {
            this.Name = name;
        }

        public ProfileValueAttribute(string name, object defaultValue)
        {
            this.Name = name;
            this.DefaultValue = defaultValue;
        }

        public static bool IsProfileKey(FieldInfo field, out ProfileValueAttribute attr)
        {
            foreach (object attribute in field.GetCustomAttributes(false))
            {
                attr = attribute as ProfileValueAttribute;
                if (attr != null)
                    return true;
            }
            attr = null;
            return false;
        }

        public string Name
        {
            get;
            set;
        }

        public object DefaultValue
        {
            get;
            set;
        }

        public bool UserProfilesOnly
        {
            get;
            set;
        }

		public bool BaseProfileOnly
		{
			get;
			set;
		}
    }
}
