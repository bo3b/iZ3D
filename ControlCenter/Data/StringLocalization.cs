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
using System.Diagnostics;
using System.Xml.XPath;
using System.IO;

namespace ControlCenter.Data
{
	public static class StringLocalization
	{
		//public static StringLocalization _Instance;
		//public static StringLocalization Instance
		//{
		//    get
		//    {
		//        if (_Instance == null)
		//            _Instance = new StringLocalization();

		//        return _Instance;
		//    }
		//}

		private static XPathNavigator _Nav;
		private static XPathNavigator _DefNav = null;
		private static Stack<XPathNavigator> _CurNav = new Stack<XPathNavigator>();
		private static Stack<XPathNavigator> _CurDefNav = new Stack<XPathNavigator>();
		private static string _Language;
		public static string Language
		{
			get
			{
				return _Language;
			}
			set
			{
				_Language = value;
				string userDir = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\iZ3D Driver\\Language\\";

				try
				{
					XPathDocument Doc = new XPathDocument(userDir + _Language + ".xml");
					_Nav = Doc.CreateNavigator();
				}
				catch (System.IO.FileNotFoundException /*ex*/)
				{
					Debug.WriteLine(_Language + ".xml: XML Document error");
				}
				catch (System.Xml.XmlException /*ex*/)
				{
					Debug.WriteLine(_Language + ".xml: XML Document error");
				}

				_DefNav = null;
				if (String.Compare(_Language, "English", true) != 0)
				{
					try
					{
						XPathDocument DefDoc = new XPathDocument(userDir + "English.xml");
						_DefNav = DefDoc.CreateNavigator();
					}
					catch (System.IO.FileNotFoundException /*ex*/)
					{
						Debug.WriteLine("English.xml: XML Document error");
					}
					catch (System.Xml.XmlException /*ex*/)
					{
						Debug.WriteLine("English.xml: XML Document error");
					}
				}
				SelectRootPage();
			}
		}

		public static bool FailedToLoadFile
		{
			get
			{
				return _Nav == null;
			}
		}


		public static bool FailedToLoadDefFile
		{
			get
			{
				return (_DefNav == null) && (_Nav == null);
			}
		}

		public static void SelectRootPage()
		{
			_CurNav.Clear();
			if (_Nav != null)
				_CurNav.Push(_Nav);
			_CurDefNav.Clear();
			if (_DefNav != null)
				_CurDefNav.Push(_DefNav);
		}

		public static void SelectPage(string page)
		{
			if (_CurNav.Count != 0)
			{
				XPathNodeIterator iterator = _CurNav.Peek().Select(page);
				iterator.MoveNext();
				_CurNav.Push(iterator.Current);
			}
			if (_CurDefNav.Count != 0)
			{
				XPathNodeIterator iterator = _CurDefNav.Peek().Select(page);
				iterator.MoveNext();
				_CurDefNav.Push(iterator.Current);
			}
		}

		public static void PopPage()
		{
			if (_CurNav.Count != 0)
				_CurNav.Pop();
			if (_CurDefNav.Count != 0)
				_CurDefNav.Pop();
		}

		public static string GetString(string node)
		{
			if (_CurNav.Count == 0)
				return "";

			XPathNodeIterator iterator = _CurNav.Peek().Select(node);

			if (iterator.Count == 0)
			{
				Debug.WriteLine("Node " + node + " not available in language " + _Language);
				if (_CurDefNav.Count != 0)
					iterator = _CurDefNav.Peek().Select(node);
			}

			if (iterator.Count != 0)
			{
				iterator.MoveNext();
				return (string)iterator.Current.Value;
			}

			return "";
		}

		public static string Localize(string node)
		{
			string localized = GetString("//" + node);
			if (String.IsNullOrEmpty(localized))
			{
				localized = node;
				Debug.WriteLine("Using id " + node + " value");
			}

			return localized;
		}

		public static System.Collections.IEnumerable GetStrings(string node)
		{
			if (_CurNav.Count == 0)
				yield break;

			XPathNodeIterator defIterator = null;
			XPathNodeIterator defChildrens = null;
			if (_CurDefNav.Count != 0)
			{
				defIterator = _CurDefNav.Peek().Select(node);
				if (defIterator.Count != 0)
				{
					defIterator.MoveNext();
					defChildrens = defIterator.Current.SelectChildren(XPathNodeType.All);
				}
			}

			XPathNodeIterator iterator = _CurNav.Peek().Select(node);
			if (iterator.Count == 0)
			{
				Debug.WriteLine("Node " + node + " not available in language " + _Language);
				if (_CurDefNav.Count != 0)
					iterator = defIterator;
			}

			if (iterator.Count != 0)
			{
				iterator.MoveNext();
				XPathNodeIterator childrens = iterator.Current.SelectChildren(XPathNodeType.All);
				Debug.Assert(defChildrens == null || defChildrens.Count == childrens.Count);
				foreach (XPathNavigator nav in childrens)
				{
					yield return nav.Value;
				}
			}
		}
	}
}
