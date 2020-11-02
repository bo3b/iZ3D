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
using AppModule.InterProcessComm;
using AppModule.NamedPipes;

namespace ControlCenter.IPC
{
	public class IPC
	{
		private bool _Failed = false;
		public bool Failed
		{
			get
			{
				return _Failed;
			}
		}

		private string _ErrorMessage = "";
		public string ErrorMessage
		{
			get
			{
				return _ErrorMessage;
			}
		}

		private bool _D3DInjected;
		public bool D3DInjected
		{
			get
			{
				return IsInjectedD3D();
			}
			set
			{
				if (_D3DInjected != value)
					InjectD3D(value);

				_D3DInjected = value;
			}
		}

		public IPC()
		{
			_D3DInjected = IsInjectedD3D();
		}

		private void InjectD3D(bool inject)
		{
			try
			{
				// Application path
				//string path = Application.StartupPath;

				// Make IPC command
				int status = 0;
				bool result = false;
				IInterProcessConnection client = new ClientPipeConnection("S3D IPCChannel", ".");

				client.Connect();
				const int MAX_PATH = 260;
				byte[] buffer = new byte[4 + 4 + 2 * MAX_PATH];

				// 4 == inject d3d 2 == uninject
				int commandid = inject ? 4 : 2;
				BitConverter.GetBytes((Int32)commandid).CopyTo(buffer, 0);
				BitConverter.GetBytes((Int32)0).CopyTo(buffer, 4);

				byte[] ccPath = Encoding.Unicode.GetBytes(Application.StartupPath);
				ccPath.CopyTo(buffer, 8);
				int size = 4 + 4 + MAX_PATH;

				//if (state == ConfigData.StereoState.Update)
				//{
				//  ccPath.CopyTo(buffer, 8 + MAX_PATH);
				//  size += MAX_PATH;
				//}

				client.WriteBytes(buffer, 0, size);
				client.Flush();

				buffer = client.ReadBytes();

				result = (BitConverter.ToInt32(buffer, 0) == 0);
				status = BitConverter.ToInt32(buffer, 4);
				string message = BitConverter.ToString(buffer, 8, MAX_PATH);

				var umess = Encoding.Unicode.GetString(buffer, 8, MAX_PATH);

				bool injected = (status & 1) != 0;

				client.Close();
			}
			catch(Exception ex)
			{
				_Failed = true;
				_ErrorMessage = ex.Message;
			}
		}

		private bool IsInjectedD3D()
		{
			int status = 0;
			try
			{
				bool result = false;
				IInterProcessConnection client = new ClientPipeConnection("S3D IPCChannel", ".");

				client.Connect();
				const int MAX_PATH = 260;
				byte[] buffer = new byte[4 + 4 + 2 * MAX_PATH];

				// 1 == get injection status
				BitConverter.GetBytes((Int32)1).CopyTo(buffer, 0);
				BitConverter.GetBytes((Int32)0).CopyTo(buffer, 4);

				byte[] ccPath = Encoding.Unicode.GetBytes(Application.StartupPath);
				ccPath.CopyTo(buffer, 8);
				int size = 4 + 4 + MAX_PATH;

				//if (state == ConfigData.StereoState.Update)
				//{
				//  ccPath.CopyTo(buffer, 8 + MAX_PATH);
				//  size += MAX_PATH;
				//}

				client.WriteBytes(buffer, 0, size);
				client.Flush();

				buffer = client.ReadBytes();

				result = (BitConverter.ToInt32(buffer, 0) == 0);
				status = BitConverter.ToInt32(buffer, 4);

				client.Close();
			}
			catch (Exception ex)
			{
				_Failed = true;
				_ErrorMessage = ex.Message;
			}

			bool injected = (status & 1) != 0;

			return injected;
		}
	}
}
