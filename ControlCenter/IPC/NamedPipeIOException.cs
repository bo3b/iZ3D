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
using System.Runtime.Serialization;

using AppModule.InterProcessComm;

namespace AppModule.NamedPipes
{
	#region Comments
	/// <summary>
	/// This exception is thrown by named pipes communication methods.
	/// </summary>
	#endregion
	public class NamedPipeIOException : InterProcessIOException
	{
		#region Comments
		/// <summary>
		/// Creates a NamedPipeIOException instance.
		/// </summary>
		/// <param name="text">The error message text.</param>
		#endregion
		public NamedPipeIOException(String text)
			: base(text)
		{
		}
		#region Comments
		/// <summary>
		/// Creates a NamedPipeIOException instance.
		/// </summary>
		/// <param name="text">The error message text.</param>
		/// <param name="errorCode">The native error code.</param>
		#endregion
		public NamedPipeIOException(String text, int errorCode)
			: base(text)
		{
			this.ErrorCode = errorCode;
			if (errorCode == NativeMethods.ERROR_CANNOT_CONNECT_TO_PIPE)
			{
				this.IsServerAvailable = false;
			}
		}
		#region Comments
		/// <summary>
		/// Creates a NamedPipeIOException instance.
		/// </summary>
		/// <param name="info">The serialization information.</param>
		/// <param name="context">The streaming context.</param>
		#endregion
		protected NamedPipeIOException(SerializationInfo info, StreamingContext context)
			: base(info, context)
		{
		}
	}
}