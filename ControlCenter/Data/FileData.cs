using System;
using System.Collections.Generic;
using System.Text;

namespace ControlCenter.Data
{
    public class FileData : ICloneable, IComparable<FileData>
	{
		public FileData()
		{ }
		public FileData(string Filename, string CmdLine)
		{ filename = Filename; cmdLine = CmdLine; }
		public string filename;
		public string cmdLine;

		#region ICloneable Members

		public object Clone()
		{
			return new FileData(filename, cmdLine);
		}

		#endregion

        #region IComparable<FileData> Members

        public int CompareTo(FileData other)
        {
            int result = String.Compare(filename, other.filename, true);
            if (result == 0)
                result = String.Compare(cmdLine, other.cmdLine, true);
            return result;
        }

        #endregion
    }
}
