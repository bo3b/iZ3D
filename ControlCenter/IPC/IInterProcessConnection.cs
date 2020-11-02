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
using Microsoft.Win32.SafeHandles;

namespace AppModule.InterProcessComm
{
	#region Comments
	/// <summary>
	/// 
	/// </summary>
	#endregion
	public interface IInterProcessConnection : IDisposable
	{
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		#endregion
		SafeFileHandle NativeHandle { get; }
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		#endregion
		void Connect();
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		#endregion
		void Close();
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		#endregion
		string Read();
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		#endregion
		byte[] ReadBytes();
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		/// <param name="text"></param>
		#endregion
		void Write(string text);
		#region Comments
		/// <summary>
		/// 
		/// </summary>
		/// <param name="bytes"></param>
		#endregion
		void WriteBytes(byte[] bytes);
		void WriteBytes(byte[] bytes, int offset, int size);

		void Flush();

		#region Comments
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		#endregion
		InterProcessConnectionState GetState();
	}
}