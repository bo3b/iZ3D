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
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.Reflection;

namespace ControlCenter.Data
{
	public class DataHolder : INotifyPropertyChanged
	{
		private DataHolder()
		{
		}

		static private DataHolder _Instance;
		static public DataHolder GetInstance()
		{
			if (_Instance == null)
				_Instance = new DataHolder();

			return _Instance;
		}

		private Profile _DefaultProfile;
		public Profile DefaultProfile
		{
			get { return _DefaultProfile; }
			set
			{
				_DefaultProfile = value;

				OnPropertyChanged("DefaultProfile");
			}
		}

		private List<Profile> _BaseProfiles = new List<Profile>();
		public List<Profile> BaseProfiles
		{
			get { return _BaseProfiles; }
			set
			{
				_BaseProfiles = value;

				OnPropertyChanged("BaseProfiles");
			}
		}

		private List<ExceptionElement> _BaseProfilesExceptionsList = new List<ExceptionElement>();
		public List<ExceptionElement> BaseProfilesExceptionsList
		{
			get { return _BaseProfilesExceptionsList; }
			set
			{
				_BaseProfilesExceptionsList = value;

				OnPropertyChanged("BaseProfilesExceptionsList");
			}
		}

		private List<Profile> _UserProfiles = new List<Profile>();
		public List<Profile> UserProfiles
		{
			get { return _UserProfiles; }
			set
			{
				_UserProfiles = value;

				OnPropertyChanged("UserProfiles");
			}
		}

		private List<ExceptionElement> _UserProfilesExceptionsList = new List<ExceptionElement>();
		public List<ExceptionElement> UserProfileExceptionsList
		{
			get { return _UserProfilesExceptionsList; }
			set
			{
				_UserProfilesExceptionsList = value;

				OnPropertyChanged("UserProfileExceptionsList");
			}
		}

		private DriverSettings _Settings = new DriverSettings();
		public DriverSettings Settings
		{
			get { return _Settings; }
			set
			{
				_Settings = value;

				OnPropertyChanged("Settings");
			}
		}

		private StereoSettings _StereoSettings = new StereoSettings();
		public StereoSettings StereoSettings
		{
			get { return _StereoSettings; }
			set
			{
				_StereoSettings = value;

				OnPropertyChanged("StereoSettings");
			}
		}

		private Object StringToValue(string s, Type targetType)
		{
			if (targetType == typeof(Int32) || targetType == typeof(Int32?))
			{
				int i;
				Int32.TryParse(s, out i);
				return i;
			}
			else if (targetType == typeof(Single) || targetType == typeof(Single?))
			{
				float f;
				Single.TryParse(s, NumberStyles.Number, CultureInfo.InvariantCulture, out f);
				return f;
			}
			else if (targetType == typeof(Double) || targetType == typeof(Double?))
			{
				double d;
				Double.TryParse(s, NumberStyles.Number, CultureInfo.InvariantCulture, out d);
				return d;
			}
			else if (targetType == typeof(Boolean) || targetType == typeof(Boolean?))
			{
				bool result;
				int i;
				if (Int32.TryParse(s, out i))
					result = i != 0;
				else
					Boolean.TryParse(s, out result);
				return result;
			}
			else if (targetType == typeof(HotKey))
			{
				uint result;
				UInt32.TryParse(s, out result);
				return new HotKey(result);
			}
			Object o = null;
			try
			{
				o = TypeDescriptor.GetConverter(targetType).ConvertFromString(s);
			}
			catch
			{
			}
			return o;
		}

		private void ReadKeys(ProfileKeys profile, XmlReader reader)
		{
			reader.Skip();
			while (reader.Read())
			{
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				ProfileValueAttribute attr = null;
				FieldInfo field = GetFieldInfo(reader.Name, typeof(ProfileKeys), out attr);
				Debug.Assert(field != null);
				if (field == null)
					continue;
				reader.MoveToAttribute(0);
				field.SetValue(profile,
					StringToValue(reader.Value, field.FieldType));
			}
		}

		private void ReadPreset(ProfilePreset preset, XmlReader reader)
		{
			reader.Skip();
			while (reader.Read())
			{
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				ProfileValueAttribute attr = null;
				FieldInfo field = GetFieldInfo(reader.Name, typeof(ProfilePreset), out attr);
				Debug.Assert(field != null);
				if (field == null)
					continue;
				reader.MoveToAttribute(0);
				field.SetValue(preset,
					StringToValue(reader.Value, field.FieldType));
			}
		}

		private void ReadPresets(List<ProfilePreset> presets, XmlReader reader)
		{
			reader.Skip();
			int i = 0;
			while (reader.Read())
			{
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				if (reader.Name == "Preset")
				{
					ProfilePreset pr;
					if (i < presets.Count)
						pr = presets[i];
					else
					{
						pr = new ProfilePreset();
						presets.Add(pr);
					}
					ReadPreset(pr, reader.ReadSubtree());
					i++;
				}
			}
		}

		private static FieldInfo GetFieldInfo(string name, Type type, out ProfileValueAttribute attr)
		{
			foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
			{
				if (ProfileValueAttribute.IsProfileKey(field, out attr) && 
					String.Compare(attr.Name, name, true) == 0)
					return field;
			}
			attr = null;
			return null;
		}

		private void ReadProfile(Profile CurrentProfile, XmlReader reader)
		{
			while (reader.Read())
			{
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				ProfileValueAttribute attr = null;
				FieldInfo field = GetFieldInfo(reader.Name, typeof(Profile), out attr);
				if (field == null)
				{
					switch (reader.Name)
					{
						case "Keys":
							ReadKeys(CurrentProfile.Keys, reader.ReadSubtree());
							break;
						case "Presets":
							ReadPresets(CurrentProfile.Presets, reader.ReadSubtree());
							break;
						case "File":
							var rec = new FileData(reader.GetAttribute("Name"), reader.GetAttribute("CmdLine"));
							CurrentProfile.Files.Add(rec);
							break;
					}
					continue;
				}
				reader.MoveToAttribute(0);
				field.SetValue(CurrentProfile,
					StringToValue(reader.Value, field.FieldType));
			}
		}

		private void ReadExceptionsList(XmlReader reader, List<ExceptionElement> list, bool bBaseList)
		{
			bool bFoundRouterType = false;
			while (reader.Read())
			{
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				if (reader.Name == "Profile" && reader.NodeType == XmlNodeType.EndElement)
					break;
				if (reader.NodeType != XmlNodeType.Element)
					continue;
				if (reader.Name == "RouterType")
				{
					bFoundRouterType = true;
					continue;
				}
				if (reader.Name != "File")
					continue;
				reader.MoveToAttribute("AppName");
				string appName = reader.Value;
				reader.MoveToAttribute("Name");
				string filename = reader.Value;
				if (String.IsNullOrEmpty(appName))
					appName = Path.GetFileNameWithoutExtension(filename);
				ExceptionElement ex = new ExceptionElement(appName, reader.Value, bBaseList);
				list.Add(ex);
			}
			Debug.Assert(bFoundRouterType);
		}

		public void LoadDefaultProfile()
		{
			DefaultProfile = Profile.CreateDefaultProfile();

			try
			{
				XmlTextReader reader = new XmlTextReader(GetConfigFilename());

				while (reader.Read())
				{
					if (reader.NodeType != XmlNodeType.Element)
						continue;

					if (String.Compare(reader.Name, "DefaultProfile") == 0)
						ReadProfile(DefaultProfile, reader.ReadSubtree());
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine("LoadDefaultProfile: " + ex.Message);
			}

			DefaultProfile.Name = "Default profile";
			DefaultProfile.PreviousName = DefaultProfile.Name;
			DefaultProfile.Filename = "";
		}

		public void LoadBaseProfiles()
		{
			string appDir = GetAppDataFolder();
			string filename = appDir + "BaseProfile.xml";
			LoadProfiles(filename, _BaseProfiles, _BaseProfilesExceptionsList, null, true);
		}

		public void LoadUserProfiles()
		{
			string userDir = GetUserAppDataFolder();
			string filename = userDir + "UserProfile.xml";
			LoadProfiles(filename, _UserProfiles, _UserProfilesExceptionsList, _BaseProfiles, false);

			foreach(Profile baseProfile in _BaseProfiles)
			{
				if (_UserProfiles.Find(pf => pf.BaseProfile == baseProfile) == null)
				{
					// create empty user profile
					_UserProfiles.Add(baseProfile.CreateUserProfile());
				}            
			}
		}

		// FIXME: separation and convergence in last preset will overwrite values in first preset
		private void LoadProfiles(string filename, List<Profile> profiles, List<ExceptionElement> exceptions, 
			List<Profile> baseProfiles, bool bBaseList)
		{
			Debug.Assert(profiles.Count == 0);
			try
			{
				XmlTextReader reader = new XmlTextReader(filename);

				while (reader.Read())
				{
					if (reader.NodeType != XmlNodeType.Element)
						continue;

					if (reader.Name != "Profile")
						continue;

					XmlReader profile = reader.ReadSubtree();
					profile.Read();
					profile.MoveToAttribute(0);
					if (String.Compare(reader.Value, DisableDriver, true) == 0)
					{
						ReadExceptionsList(profile, exceptions, bBaseList);
						continue;
					}

					Profile CurrentProfile = new Profile();
					CurrentProfile.Name = reader.Value;
					CurrentProfile.PreviousName = CurrentProfile.Name;
					ReadProfile(CurrentProfile, profile);
					if (baseProfiles != null)
					{
						CurrentProfile.BaseProfile = baseProfiles.Find(
						delegate(Profile pf)
						{
							foreach (var rec in pf.Files)
							{
								if (CurrentProfile.Files.Exists(srcRec => srcRec.CompareTo(rec) == 0))
									return true;
							}
							return false;
						});
						if (CurrentProfile.BaseProfile != null && !(CurrentProfile.UserChangeProfileName ?? false))
							CurrentProfile.Name = CurrentProfile.BaseProfile.Name;
					}
					profiles.Add(CurrentProfile);
				}

				reader.Close();
			}
			catch (Exception ex)
			{
				Debug.WriteLine("LoadBaseProfiles: " + ex.Message);
			}
		}

		private string GetConfigFilename()
		{
			string file;
#if !FINAL_RELEASE
			file = Application.StartupPath + "\\Config.xml";
			if (!File.Exists(file))
#endif
			{
				file = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\iZ3D Driver\\Config.xml";
			}
			return file;
		}

		private string GetAppDataFolder()
		{
			string applicationData =
			Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
			applicationData += "\\iZ3D Driver\\";
			return applicationData;
		}

		private string GetUserAppDataFolder()
		{
			string applicationData =
			Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
			applicationData += "\\iZ3D Driver\\";
			return applicationData;
		}

		private const string DisableDriver = "Disable driver";

		public void LoadSettings()
		{
			try
			{
				XmlTextReader reader = new XmlTextReader(GetConfigFilename());

				while (reader.Read())
				{
					if (reader.NodeType != XmlNodeType.Element)
						continue;

					ProfileValueAttribute attr = null;
					FieldInfo field = GetFieldInfo(reader.Name, typeof(DriverSettings), out attr);
					if (field == null)
						continue;
					reader.MoveToAttribute(0);
					field.SetValue(Settings,
						StringToValue(reader.Value, field.FieldType));
				}

				reader.Close();
			}
			catch (Exception ex)
			{
				Debug.WriteLine("LoadSettings: " + ex.Message);
			}
		}
		

		public void LoadStereoState()
		{
			try
			{
				XmlTextReader reader = new XmlTextReader(GetConfigFilename());

				while (reader.Read())
				{
					if (reader.NodeType != XmlNodeType.Element)
						continue;

					ProfileValueAttribute attr = null;
					FieldInfo field = GetFieldInfo(reader.Name, typeof(StereoSettings), out attr);
					if (field == null)
						continue;
					reader.MoveToAttribute(0);
					field.SetValue(StereoSettings,
						StringToValue(reader.Value, field.FieldType));
				}

				reader.Close();
			}
			catch (Exception ex)
			{
				Debug.WriteLine("LoadStereoState: " + ex.Message);
			}
		}

		private string ValueToString(Object arg)
		{
			if (arg.GetType() == typeof(Single) || arg.GetType() == typeof(Single?))
			{
				Single d = (Single)arg;
				return d.ToString("F", CultureInfo.InvariantCulture);
			}
			else if (arg.GetType() == typeof(Double) || arg.GetType() == typeof(Double?))
			{
				Double d = (Double)arg;
				return d.ToString("F", CultureInfo.InvariantCulture);
			}
			else if (arg.GetType() == typeof(Boolean) || arg.GetType() == typeof(Boolean?))
			{
				Boolean b = (Boolean)arg;
				return b ? "1" : "0";
			}
			else if (arg.GetType() == typeof(HotKey))
			{
				HotKey hk = (HotKey)arg;
				return hk.VKKey.ToString();
			}
			else
			{
				return arg.ToString();
			}
		}

		private bool IsEqual(Object x, Object y)
		{
			if (x.GetType() == typeof(Single) && y.GetType() == typeof(Single))
			{
				return Math.Abs((float)x - (float)y) < 1e-7;
			}
			else if (x.GetType() == typeof(Double) && y.GetType() == typeof(Double))
			{
				return Math.Abs((double)x - (double)y) < 1e-7;
			}
			return object.Equals(x, y);
		}

		void SetValue(XmlNode node, string name, Object value)
		{
			XmlElement child = FindNode(node.ChildNodes, name);
			if (child == null)
				child = CreateChildNodeWithAttribute(node, name, "Value");

			string s = ValueToString(value);
			child.Attributes[0].Value = s;
		}

		void RemoveValue(XmlNode node, string name)
		{
			XmlElement child = FindNode(node.ChildNodes, name);
			if (child != null)
				node.RemoveChild(child);
		}

		public void SaveSettings()
		{
			string configFile = GetConfigFilename();

			XmlDocument doc = new XmlDocument();
			doc.Load(configFile);

			XmlNodeList list = doc.GetElementsByTagName("ControlCenter");
			XmlNode node = list[0];

			SetValue(node, "AutoStart", Settings.LaunchWithWindows);
			SetValue(node, "ShowUpdates", Settings.ShowUpdates);
			SetValue(node, "TestUpdaterURL", Settings.ShowUpdates);

			list = doc.GetElementsByTagName("GlobalSettings");
			node = list[0];

			SetValue(node, "Language", Settings.Language);
			SetValue(node, "LockCursor", Settings.LockMouse);

			list = doc.GetElementsByTagName("DefaultProfile");
			node = list[0];

			SetValue(node, "DisableD3D10Driver", !Settings.EnableD3D1xDriver);

			try
			{
				doc.Save(configFile);
			}
			catch (System.IO.IOException ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		public void SaveStereoState()
		{
			string configFile = GetConfigFilename();	

			XmlDocument doc = new XmlDocument();
			doc.Load(configFile);

			XmlNodeList list = doc.GetElementsByTagName("DefaultProfile");
			XmlNode node = list[0];

			SetValue(node, "EnableStereo", StereoSettings.StereoEnabled);

			list = doc.GetElementsByTagName("GlobalSettings");
			node = list[0];

			SetValue(node, "RunAtStartup", StereoSettings.DriverEnabled);
			SetValue(node, "OutputMethodDll", StereoSettings.Output);
			SetValue(node, "OutputMode", StereoSettings.Mode);
			SetValue(node, "DisplayScalingMode", StereoSettings.ScalingMode);

			try
			{
				doc.Save(configFile);
			}
			catch (System.IO.IOException ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		public void SaveAllData()
		{
			SaveDefaultProfile();
			SaveUserProfiles();
			SaveSettings();
			SaveStereoState();
		}

		private void SaveExceptionsList(XmlElement node, List<ExceptionElement> list, bool bBaseList)
		{
			XmlDocument doc = node.OwnerDocument;
			List<ExceptionElement> lst = new List<ExceptionElement>();
			lst.AddRange(list);
			List<XmlNode> removeNodes = new List<XmlNode>();
			// TODO: Check RouterType
			foreach (XmlNode n in node.GetElementsByTagName("File"))
			{
				XmlElement el = n as XmlElement;
				if (el == null)
					continue;
				string filename = el.GetAttribute("Name");
				var ex = lst.Find(p => (String.Compare(p.FileName, filename) == 0));
				if (ex != null)
					el.SetAttribute("AppName", ex.AppName);
				else
					removeNodes.Add(n);
				lst.Remove(ex);
			}
			foreach (XmlNode n in removeNodes)
				node.RemoveChild(n);
			foreach (ExceptionElement ex in lst)
			{
				XmlElement el = doc.CreateElement("File", doc.DocumentElement.NamespaceURI);
				node.AppendChild(el);
				el.SetAttribute("AppName", ex.AppName);
				el.SetAttribute("Name", ex.FileName);
			}
		}

		public void SaveUserProfiles()
		{
			string userDir = GetUserAppDataFolder();

			XmlDocument doc = new XmlDocument();// (userDir + "UserProfile.xml");
			doc.Load(userDir + "UserProfile.xml");

			XmlNodeList list = doc.GetElementsByTagName("Profile");

			foreach (Profile User in UserProfiles)
			{
				if (!User.Changed)
					continue;

				string Name = User.PreviousName;
				Profile Base = User.BaseProfile;

				XmlNode profileNode = FindNode(list, node => String.Compare(node.Attributes[0].Value, Name, true) == 0);

				if (profileNode == null) // create new profile
				{
					profileNode = doc.CreateElement("Profile", doc.DocumentElement.NamespaceURI);
					XmlAttribute name = doc.CreateAttribute("Name");
					name.Value = User.Name;
					profileNode.Attributes.Append(name);
					doc.LastChild.AppendChild(profileNode);
				}

				SaveProfile(User, profileNode);
			}

			XmlElement disableDriverNode = FindNode(list, node => String.Compare(node.Attributes[0].Value, DisableDriver, true) == 0);

			if (disableDriverNode != null || _UserProfilesExceptionsList.Count > 0)
			{
				if (disableDriverNode == null)
				{
					disableDriverNode = doc.CreateElement("Profile", doc.DocumentElement.NamespaceURI);
					XmlAttribute name = doc.CreateAttribute("Name");
					name.Value = DisableDriver;
					disableDriverNode.Attributes.Append(name);

					XmlElement rt = doc.CreateElement("RouterType", doc.DocumentElement.NamespaceURI);
					XmlAttribute value = doc.CreateAttribute("Value");
					value.Value = "2";
					rt.Attributes.Append(value);
					disableDriverNode.PrependChild(rt);

					XmlElement dontLockMouse = doc.CreateElement("DontLockMouse", doc.DocumentElement.NamespaceURI);
					value = doc.CreateAttribute("Value");
					value.Value = "1";
					dontLockMouse.Attributes.Append(value);
					disableDriverNode.PrependChild(dontLockMouse);

					doc.LastChild.PrependChild(disableDriverNode);
				}
				SaveExceptionsList(disableDriverNode, _UserProfilesExceptionsList, false);
			}

			try
			{
				doc.Save(userDir + "UserProfile.xml");
			}
			catch (System.IO.IOException ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		private void SaveProfile(Profile profile, XmlNode node)
		{
			bool bDefProfile = (profile == DefaultProfile);
			XmlDocument doc = node.OwnerDocument;
			if (profile.PreviousName != profile.Name)
			{
				if (!bDefProfile)
					node.Attributes["Name"].Value = profile.Name;
				profile.PreviousName = profile.Name;
				profile.UserChangeProfileName = true;
			}

			if (profile.Files.Count > 0)
			{
				XmlElement child = FindNode(node.ChildNodes, "File");
				if (child == null)
					child = CreateChildNodeWithAttribute(node, "File", "Name");

				// TODO: Support multiple files
				child.Attributes[0].Value = profile.Files[0].filename;
				if (profile.Files[0].cmdLine != null)
					child.SetAttribute("CmdLine", profile.Files[0].cmdLine);
			}

			Type type = typeof(Profile);
			foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
			{
				ProfileValueAttribute attr = null;
				if (ProfileValueAttribute.IsProfileKey(field, out attr))
				{
					bool bSetValue = false;
					bool bRemoveValue = false;
					if (bDefProfile)
						bSetValue = !attr.UserProfilesOnly;
					else if (!attr.BaseProfileOnly)
					{
						bSetValue = !IsEqual(field.GetValue(profile), field.GetValue(DefaultProfile));
						bRemoveValue = true;
					}
					if (bSetValue)
						SetValue(node, attr.Name, field.GetValue(profile));
					else if (bRemoveValue)
						RemoveValue(node, attr.Name);
					
				}
			}

			// PRESETS SECTION
			SavePresets(profile.Presets, DefaultProfile.Presets, node);

			// KEYS SECTION
			SaveKeys(profile.Keys, node);
		}

		public void SaveDefaultProfile()
		{
			string configFile = GetConfigFilename();

			XmlDocument doc = new XmlDocument();
			doc.Load(configFile);

			XmlNodeList list = doc.GetElementsByTagName("DefaultProfile");
			XmlNode node = list[0];

			if (DefaultProfile.Changed)
				SaveProfile(DefaultProfile, node);

			try
			{
				doc.Save(configFile);
			}
			catch (System.IO.IOException ex)
			{
				Debug.WriteLine(ex.Message);
			}
		}

		void SetKey(XmlNode profileNode, ref XmlNode node, string name, Object value)
		{
			XmlDocument doc = profileNode.OwnerDocument;
			if (node == null)
			{
				node = doc.CreateElement("Keys", doc.DocumentElement.NamespaceURI);
				profileNode.AppendChild(node);
			}

			XmlElement child = FindNode(node.ChildNodes, name);
			if (child == null)
				child = CreateChildNodeWithAttribute(node, name, "Key");

			string s = ValueToString(value);
			child.Attributes[0].Value = s;
		}

		void RemoveKey(XmlNode node, string name)
		{
			if (node == null)
				return;
			XmlElement child = FindNode(node.ChildNodes, name);
			if (child != null)
				node.RemoveChild(child);
		}

		private void SaveKeys(ProfileKeys profile, XmlNode node)
		{
			XmlNode InsideNode = FindNode(node.ChildNodes, "Keys");

			ProfileKeys defaultKeys = DefaultProfile.Keys;
			bool bDefProfile = (profile == defaultKeys);
			
			Type type = typeof(ProfileKeys);
			foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
			{
				ProfileValueAttribute attr = null;
				if (ProfileValueAttribute.IsProfileKey(field, out attr))
				{
					bool bSetValue = false;
					bool bRemoveValue = false;
					if (bDefProfile)
						bSetValue = !attr.UserProfilesOnly;
					else if (!attr.BaseProfileOnly)
					{
						bSetValue = !IsEqual(field.GetValue(profile), field.GetValue(defaultKeys));
						bRemoveValue = true;
					}
					if (bSetValue)
						SetKey(node, ref InsideNode, attr.Name, field.GetValue(profile));
					else if (bRemoveValue)
						RemoveKey(InsideNode, attr.Name);
				}
			}
		}

		private void SavePreset(ProfilePreset preset, ProfilePreset defPreset, XmlNode node)
		{
			bool bDefPreset = (preset == defPreset);
			Type type = typeof(ProfilePreset);
			foreach (FieldInfo field in type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance))
			{
				ProfileValueAttribute attr = null;
				if (ProfileValueAttribute.IsProfileKey(field, out attr))
				{
					bool bSetValue = false;
					bool bRemoveValue = false;
					if (bDefPreset)
						bSetValue = !attr.UserProfilesOnly;
					else if (!attr.BaseProfileOnly)
					{
						if (defPreset == null)
							bSetValue = true;
						else
						{
							bSetValue = !IsEqual(field.GetValue(preset), field.GetValue(defPreset));
							bRemoveValue = true;
						}
					}
					if (bSetValue)
						SetValue(node, attr.Name, field.GetValue(preset));
					else if (bRemoveValue)
						RemoveValue(node, attr.Name);
				}
			}
		}

		private void SavePresets(List<ProfilePreset> presets, List<ProfilePreset> defPresets, XmlNode node)
		{
			List<ProfilePreset> defaultPresets = DefaultProfile.Presets;
			bool bDefProfile = (presets == defaultPresets);

			XmlDocument doc = node.OwnerDocument;
			XmlNode InsideNode = FindNode(node.ChildNodes, "Presets");
			if (InsideNode == null)
			{
				InsideNode = doc.CreateElement("Presets", doc.DocumentElement.NamespaceURI);
				node.AppendChild(InsideNode);
			}

			XmlNode presetNode = FindNode(InsideNode.ChildNodes, "Preset");
			for (int i = 0; i < 3; i++)
			{
				if (i != 0)
				{
					do 
					{                        
						presetNode = presetNode.NextSibling;
					} while (presetNode != null && String.Compare(presetNode.Name, "Preset") != 0);
				}
				ProfilePreset defPreset = i < defPresets.Count ? defPresets[i] : null;
				if (presetNode == null)
				{
					presetNode = doc.CreateElement("Preset", doc.DocumentElement.NamespaceURI);
					XmlAttribute attr = doc.CreateAttribute("Index");
					attr.Value = i.ToString();
					presetNode.Attributes.Append(attr);
					InsideNode.AppendChild(presetNode);
				}
				SavePreset(presets[i], defPreset, presetNode);
			}            
		}

		private delegate bool FindDelegate(XmlNode node);

		private XmlElement FindNode(XmlNodeList list, FindDelegate find)
		{
			XmlElement result = null;
			foreach (XmlNode node in list)
			{
				if (find(node))
				{
					result = node as XmlElement;
					break;
				}
			}
			return result;
		}

		private XmlElement FindNode(XmlNodeList list, string nodeName)
		{
			return FindNode(list, node => String.Compare(node.Name, nodeName, true) == 0);
		}

		private XmlElement CreateChildNodeWithAttribute(XmlNode node, string elem, string attr)
		{
			XmlDocument doc = node.OwnerDocument;
			XmlElement child = doc.CreateElement(elem, doc.DocumentElement.NamespaceURI);
			XmlAttribute a = doc.CreateAttribute(attr);
			child.Attributes.Append(a);
			node.AppendChild(child);
			return child;
		}

		public void LoadAllData()
		{
			LoadDefaultProfile();
			LoadBaseProfiles();
			LoadUserProfiles();
			LoadStereoState();
			LoadSettings();
		}

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
	}
}
