// Copyright (c) Microsoft Corporation.  All rights reserved.

//Copyright (c) Microsoft Corporation.  All rights reserved.
using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    internal class IIDGuid
    {
        // IID GUID strings for relevant COM interfaces.
        internal const string IModalWindow = "b4db1657-70d7-485e-8e3e-6fcb5a5c1802";
        internal const string IFileDialog = "42f85136-db7e-439c-85f1-e4075d135fc8";
        internal const string IFileOpenDialog = "d57c7288-d4ad-4768-be02-9d969532d960";
        internal const string IFileSaveDialog = "84bccd23-5fde-4cdb-aea4-af64b83d78ab";
        internal const string IFileDialogEvents = "973510DB-7D7F-452B-8975-74A85828D354";
        internal const string IFileDialogControlEvents = "36116642-D713-4b97-9B83-7484A9D00433";
        internal const string IFileDialogCustomize = "8016b7b3-3d49-4504-a0aa-2a37494e606f";
        internal const string IShellItem = "43826D1E-E718-42EE-BC55-A1E261C37BFE";
        internal const string IShellItemArray = "B63EA76D-1F85-456F-A19C-48159EFA858B";

        internal const string IKnownFolder = "3AA7AF7E-9B36-420c-A8E3-F77D4674A488";

        internal const string IKnownFolderManager = "8BE2D872-86AA-4d47-B776-32CCA40C7018";

        internal const string IPropertyStore = "886D8EEB-8CF2-4446-8D02-CDBA1DBDCF99";
    }

    internal class CLSIDGuid
    {
        // CLSID GUID strings for relevant coclasses.
        internal const string FileOpenDialog = "DC1C5A9C-E88A-4dde-A5A1-60F82A20AEF7";
        internal const string FileSaveDialog = "C0B4E2F3-BA21-4773-8DBA-335EC946EB8B";
        internal const string KnownFolderManager = "4df0c730-df9d-4ae3-9153-aa6b82e9795a";
    }

    internal class KFIDGuid
    {
        internal const string ComputerFolder = "0AC0837C-BBF8-452A-850D-79D08E667CA7";
        internal const string Favorites = "1777F761-68AD-4D8A-87BD-30B759FA33DD";
        internal const string Documents = "FDD39AD0-238F-46AF-ADB4-6C85480369C7";
        internal const string Profile = "5E6C858F-0E22-4760-9AFE-EA3317B67173";

#if WIN7
        internal const string DocumentsLibrary = "7b0db17d-9cd2-4a93-9733-46cc89022e7c";
        internal const string MusicLibrary = "2112AB0A-C86A-4ffe-A368-0DE96E47012E";
        internal const string PicturesLibrary = "A990AE9F-A03B-4e80-94BC-9912D7504104";
        internal const string VideosLibrary  = "491E922F-5643-4af4-A7EB-4E7A138D8174";
        internal const string RecordedTVLibrary  = "1A6FDBA2-F42D-4358-A798-B74D745926C5";
        internal const string Libraries = "1B3EA5DC-B587-4786-B4EF-BD1DC332AEAE";
#endif
    }

}