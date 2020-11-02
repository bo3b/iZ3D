/* IZ3D_FILE: $Id: DataHolder.cs 4265 2010-12-03 18:43:42Z dragon $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: dragon $
* $Revision: 4265 $
* $Date: 2010-12-03 21:43:42 +0300 (Ïò, 03 äåê 2010) $
* $LastChangedBy: dragon $
* $URL: svn://svn.neurok.ru/dev/driver/branch/CCRedesignCSharp/ControlCenter/Data/DataHolder.cs $
*/
using System;
using System.Collections.Generic;

namespace ControlCenter.Data
{
	public class ExceptionElement : IComparable, IComparable<ExceptionElement>
	{
		public ExceptionElement()
		{
		}

		public ExceptionElement(string name, string filename, bool fromBaseProfile = false)
		{
			_name = name;
			_filename = filename;
			_fromBaseProfile = fromBaseProfile;
		}

		private string _name;
		public string AppName
		{
			get { return _name; }
			set { _name = value; }
		}

		private string _filename;
		public string FileName
		{
			get { return _filename; }
			set { _filename = value; }
		}

		private bool _fromBaseProfile;
		public bool FromBaseProfile
		{
			get { return _fromBaseProfile; }
			set { _fromBaseProfile = value; }
		}

		public override string ToString()
		{
			return _name;
		}

		#region IComparable<ExceptionElement> Members

		public int CompareTo(ExceptionElement other)
		{
			int result = _fromBaseProfile.CompareTo(other._fromBaseProfile);
			if (result != 0)
				return result;
			result = _name.CompareTo(other._name);
			if (result != 0)
				return result;
			result = _filename.CompareTo(other._filename);
			return result;
		}

		#endregion

		#region IComparable Members

		public int CompareTo(object obj)
		{
			ExceptionElement ex = obj as ExceptionElement;
			if (ex != null)
				return CompareTo(obj as ExceptionElement);
			else
                throw new ArgumentException("Object is not a ExceptionElement");
		}

		#endregion
	}
}