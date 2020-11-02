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
using System.Text;
using System.Windows.Forms;

namespace ControlCenter.Data
{
	[Flags]
	public enum KeyModifiers
	{
		None = 0,
		Shift = 1,
		Control = 2,
		Alt = 4,
		Windows = 8
	}

	public class HotKey : IEquatable<HotKey>, ICloneable
	{

		public HotKey()
		{
		}

		public HotKey(uint vkkey)
		{
			VKKey = vkkey;
		}

		public HotKey(bool cntrl, bool alt, bool shift, Keys key)
		{
			SetValue(cntrl, alt, shift, key);
		}

		public void SetValue(bool cntrl, bool alt, bool shift, Keys key)
		{
			modifiers = KeyModifiers.None;
			if (cntrl)
				modifiers |= KeyModifiers.Control;
			if (alt)
				modifiers |= KeyModifiers.Alt;
			if (shift)
				modifiers |= KeyModifiers.Shift;
			Key = key;
		}

		public KeyModifiers modifiers;
		public KeyModifiers Modifiers
		{
			get { return modifiers; }
			set { modifiers = value; }
		}

		public Keys key;
		public Keys Key
		{
			get { return key; }
			set { key = value; }
		}

		public uint VKKey
		{
			get
			{
				return (uint)key | (((uint)modifiers) << 8);
			}
			set
			{
				key = (Keys)(value & 0xFF);
				modifiers = (KeyModifiers)((value & 0x0F00) >> 8);
			}
		}

		public override String ToString()
		{
			if (key == 0)
				return "(none)";

			StringBuilder strKey = new StringBuilder("");

			if ((modifiers & KeyModifiers.Control) > 0)
				strKey.Append("Ctrl + ");
			if ((modifiers & KeyModifiers.Shift) > 0)
				strKey.Append("Shift + ");
			if ((modifiers & KeyModifiers.Alt) > 0)
				strKey.Append("Alt + ");

			if (key >= Keys.NumPad0 && key <= Keys.Divide)
			{
				strKey.Append("Num ");
				if (key <= Keys.NumPad9)
					strKey.Append((key - Keys.NumPad0).ToString());
				else
					switch (key)
					{
						case Keys.Multiply:
							strKey.Append('*');
							break;
						case Keys.Add:
							strKey.Append('+');
							break;
						case Keys.Separator:
							strKey.Append('.');
							break;
						case Keys.Subtract:
							strKey.Append('-');
							break;
						case Keys.Decimal:
							strKey.Append('.');
							break;
						case Keys.Divide:
							strKey.Append('/');
							break;
					}
			}
			else if (key >= Keys.D0 && key <= Keys.D9)
			{
				strKey.Append((key - Keys.D0).ToString());
			}
			else if (key >= Keys.Oem1 && key <= Keys.OemBackslash)
			{
				switch (key)
				{
					case Keys.OemSemicolon:
						strKey.Append(';');
						break;
					case Keys.Oemplus:
						strKey.Append('=');
						break;
					case Keys.Oemcomma:
						strKey.Append(',');
						break;
					case Keys.OemMinus:
						strKey.Append('-');
						break;
					case Keys.OemPeriod:
						strKey.Append('.');
						break;
					case Keys.OemQuestion:
						strKey.Append('/');
						break;
					case Keys.OemOpenBrackets:
						strKey.Append('[');
						break;
					case Keys.Oem6:
						strKey.Append(']');
						break;
					case Keys.OemBackslash:
						strKey.Append('/');
						break;
					case Keys.Oem5:
						strKey.Append('\\');
						break;
					case Keys.Oem7:
						strKey.Append('\'');
						break;
					case Keys.Oemtilde:
						strKey.Append('~');
						break;
					default:
						strKey.Append(key.ToString());
						break;
				}
			}
			else
				strKey.Append(key.ToString());

			return strKey.ToString();
		}

		#region IEquatable<HotKey> Members

		public override bool Equals(System.Object obj)
		{
			// If parameter cannot be cast to ThreeDPoint return false:
			HotKey p = obj as HotKey;
			if ((object)p == null)
			{
				return false;
			}

			// Return true if the fields match:
			return VKKey == p.VKKey;
		}

		public bool Equals(HotKey other)
		{
			return base.Equals((HotKey)other) && VKKey == other.VKKey;
		}

		public override int GetHashCode() { return VKKey.GetHashCode(); }

		#endregion

		public static bool operator ==(HotKey a, HotKey b)
		{
			// If both are null, or both are same instance, return true.
			if (System.Object.ReferenceEquals(a, b))
			{
				return true;
			}

			// If one is null, but not both, return false.
			if (((object)a == null) || ((object)b == null))
			{
				return false;
			}

			// Return true if the fields match:
			return a.VKKey == b.VKKey;
		}

		public static bool operator !=(HotKey a, HotKey b)
		{
			return !(a == b);
		}


		#region ICloneable Members

		public object Clone()
		{
			HotKey hk = new HotKey();
			hk.key = key;
			hk.modifiers = modifiers;
			return hk;
		}

		#endregion
	}
}
