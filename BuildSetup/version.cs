namespace ControlCenter
{
	static class DriverVersion
	{
		public const string	VERSION_NAME_SUFFIX = "";
		public const uint	PRODUCT_VERSION_MAJOR = 1;
		public const uint	PRODUCT_VERSION_MINOR = 13;
		public const uint	PRODUCT_VERSION_BUILD = 0;

        public static readonly string VERSION = PRODUCT_VERSION_MAJOR + "." + PRODUCT_VERSION_MINOR + "." +
									SVN.PRODUCT_VERSION_QFE;
        public static readonly string DISPLAYED_VERSION = PRODUCT_VERSION_MAJOR + "." + PRODUCT_VERSION_MINOR +
								VERSION_NAME_SUFFIX + "(" + SVN.PRODUCT_VERSION_QFE + ")";
	}
}