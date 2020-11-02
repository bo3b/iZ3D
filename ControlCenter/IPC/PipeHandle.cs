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

using AppModule.InterProcessComm;
using Microsoft.Win32.SafeHandles;

namespace AppModule.NamedPipes
{
	#region Comments
	/// <summary>
	/// Holds the operating system native handle and the current state of the pipe connection.
	/// </summary>
	#endregion
	public sealed class PipeHandle
	{
		#region Comments
		/// <summary>
		/// The operating system native handle.
		/// </summary>
		#endregion
		public SafeFileHandle Handle;
		#region Comments
		/// <summary>
		/// The current state of the pipe connection.
		/// </summary>
		#endregion
		public InterProcessConnectionState State;
		#region Comments
		/// <summary>
		/// Creates a PipeHandle instance using the passed native handle.
		/// </summary>
		/// <param name="hnd">The native handle.</param>
		#endregion
		public PipeHandle(SafeFileHandle hnd)
		{
			this.Handle = hnd;
			this.State = InterProcessConnectionState.NotSet;
		}
		#region Comments
		/// <summary>
		/// Creates a PipeHandle instance using the provided native handle and state.
		/// </summary>
		/// <param name="hnd">The native handle.</param>
		/// <param name="state">The state of the pipe connection.</param>
		#endregion
		public PipeHandle(SafeFileHandle hnd, InterProcessConnectionState state)
		{
			this.Handle = hnd;
			this.State = state;
		}
		#region Comments
		/// <summary>
		/// Creates a PipeHandle instance with an invalid native handle.
		/// </summary>
		#endregion
		public PipeHandle()
		{
			this.Handle = new SafeFileHandle(NativeMethods.INVALID_HANDLE_VALUE, false);
			this.State = InterProcessConnectionState.NotSet;
		}

		public bool IsInvalid
		{
			get { return Handle.IsInvalid; }
		}

		static public implicit operator SafeFileHandle(PipeHandle handle)
		{
			return handle.Handle;
		}
	}
}