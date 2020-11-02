'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT 
'WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
'INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
'OF MERCHANTABILITY AND/OR FITNESS FOR A  PARTICULAR 
'PURPOSE

'------------------------------------------------------------------------------
'FILE DESCRIPTION: Script for registering for NNTP Protocol sinks. 
'
'File Name: nntpreg.vbs
' 
'
' Copyright (c) Microsoft Corporation 1993-1999. All rights reserved.
'------------------------------------------------------------------------------
Option Explicit

'
' these GUIDs are all defined in nntpfilt.idl
'
' the OnPostEarly event GUID
Const GUIDComCatOnPostEarly = "{C028FD86-F943-11d0-85BD-00C04FB960EA}"
' the OnPost event GUID
Const GUIDComCatOnPost = "{C028FD83-F943-11d0-85BD-00C04FB960EA}"
' the OnPostFinal event GUID
Const GUIDComCatOnPostFinal = "{C028FD85-F943-11d0-85BD-00C04FB960EA}"
' the NNTP source type
Const GUIDSourceType = "{C028FD82-F943-11d0-85BD-00C04FB960EA}"

' the NNTP service display name.  This is used to key which service to
' edit
Const szService = "nntpsvc"

' the event manager object.  This is used to communicate with the 
' event binding database.
Dim EventManager
Set EventManager = WScript.CreateObject("Event.Manager")

'
' register a new sink with event manager
'
' iInstance - the instance to work against
' szEvent - OnPostEarly, OnPost or OnPostFinal
' szDisplayName - the display name for this new sink
' szProgID - the progid to call for this event
' szRule - the rule to set for this event
'
public sub RegisterSink(iInstance, szEvent, szDisplayName, szProgID, szRule)
	Dim SourceType
	Dim szSourceDisplayName
	Dim Source
	Dim Binding
	Dim GUIDComCat
	Dim PrioVal

	' figure out which event they are trying to register with and set
	' the comcat for this event in GUIDComCat
	select case LCase(szEvent)
		case "onpostearly"
			GUIDComCat = GUIDComCatOnPostEarly
		case "onpost"
			GUIDComCat = GUIDComCatOnPost
		case "onpostfinal"
			GUIDComCat = GUIDComCatOnPostFinal
		case else
			WScript.echo "invalid event: " & szEvent
			exit sub
	end select

	' enumerate through each of the registered instances for the NNTP source
	' type and look for the display name that matches the instance display
	' name
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	szSourceDisplayName = szService & " " & iInstance
	for each Source in SourceType.Sources
		if Source.DisplayName = szSourceDisplayName then
			' we've found the desired instance.  now add a new binding
			' with the right event GUID.  by not specifying a GUID to the
			' Add method we get server events to create a new ID for this
			' event
			set Binding = Source.GetBindingManager.Bindings(GUIDComCat).Add("")
			' set the binding properties
			Binding.DisplayName = szDisplayName
			Binding.SinkClass = szProgID
			' register a rule with the binding
			Binding.SourceProperties.Add "Rule", szRule
			' register a priority with the binding
			PrioVal = GetNextPrio(Source, GUIDComCat)
			If PrioVal < 0 then
				WScript.Echo "assigning priority to default value (24575)"
				Binding.SourceProperties.Add "Priority", 24575
			else	
				WScript.Echo "assigning priority (" & PrioVal & " of 32767)"
				Binding.SourceProperties.Add "Priority", PrioVal
			end if
			' save the binding
			Binding.Save
			WScript.Echo "registered " & szDisplayName
			exit sub
		end if
	next
end sub

'
' iterate through the bindings in a source, find the binding
' with the lowest priority, and return the next priority value.
' If the next value exceeds the range, return -1.
'
public function GetNextPrio(oSource, GUIDComCat)
	' it's possible that priority values will not be
	' numbers, so we add error handling for this case
	on error resume next

	Dim Bindings
	Dim Binding
	Dim nLowestPrio
	Dim nPrioVal
	nLowestPrio = 0
	set Bindings = oSource.GetBindingManager.Bindings(GUIDComCat)
	' if the bindings collection is empty, then this is the first
	' sink.  It gets the highest priority (0).
	if Bindings.Count = 0 then
		GetNextPrio = 0
	else
		' get the lowest existing priority value
		for each Binding in Bindings
			nPrioVal = Binding.SourceProperties.Item("Priority")
			if CInt(nPrioVal) > nLowestPrio then
				if err.number = 13 then
					err.clear
				else
					nLowestPrio = CInt(nPrioVal)
				end if
			end if
		next 
		' assign priority values in increments of 10 so priorities
		' can be shuffled later without the need to reorder all
		' binding priorities.  Valid priority values are 0 - 32767
		if nLowestPrio + 10 > 32767 then
			GetNextPrio = -1
		else
			GetNextPrio = nLowestPrio + 10
		end if
	end if
end function

'
' unregister a previously registered sink
'
' iInstance - the instance to work against
' szEvent - OnPostEarly, OnPost or OnPostFinal
' szDisplayName - the display name of the event to remove
'
public sub UnregisterSink(iInstance, szEvent, szDisplayName)
	Dim SourceType
	Dim GUIDComCat
	Dim szSourceDisplayName
	Dim Source
	Dim Bindings
	Dim Binding

	select case LCase(szEvent)
		case "onpostearly"
			GUIDComCat = GUIDComCatOnPostEarly
		case "onpost"
			GUIDComCat = GUIDComCatOnPost
		case "onpostfinal"
			GUIDComCat = GUIDComCatOnPostFinal
		case else
			WScript.echo "invalid event: " & szEvent
			exit sub
	end select

	' find the source for this instance
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	szSourceDisplayName = szService & " " & iInstance
	for each Source in SourceType.Sources
		if Source.DisplayName = szSourceDisplayName then
			' find the binding by display name.  to do this we enumerate
			' all of the bindings and try to match on the display name
			set Bindings = Source.GetBindingManager.Bindings(GUIDComCat)
			for each Binding in Bindings
				if Binding.DisplayName = szDisplayName then
					' we've found the binding, now remove it
					Bindings.Remove(Binding.ID)
					WScript.Echo "removed " & szDisplayName & " " & Binding.ID
				end if
			next
		end if
	next
end sub

'
' add or remove a property from the source or sink propertybag for an event
'
' iInstance - the NNTP instance to edit
' szEvent - the event type (OnPostEarly, OnPost or OnPostFinal)
' szDisplayName - the display name of the event
' szPropertyBag - the property bag to edit ("source" or "sink")
' szOperation - "add" or "remove"
' szPropertyName - the name to edit in the property bag
' szPropertyValue - the value to assign to the name (ignored for remove)
'
public sub EditProperty(iInstance, szEvent, szDisplayName, szPropertyBag, szOperation, szPropertyName, szPropertyValue)
	Dim SourceType
	Dim GUIDComCat
	Dim szSourceDisplayName
	Dim Source
	Dim Bindings
	Dim Binding
	Dim PropertyBag

	select case LCase(szEvent)
		case "onpostearly"
			GUIDComCat = GUIDComCatOnPostEarly
		case "onpost"
			GUIDComCat = GUIDComCatOnPost
		case "onpostfinal"
			GUIDComCat = GUIDComCatOnPostFinal
		case else
			WScript.echo "invalid event: " & szEvent
			exit sub
	end select

	' find the source for this instance
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	szSourceDisplayName = szService & " " & iInstance
	for each Source in SourceType.Sources
		if Source.DisplayName = szSourceDisplayName then
			set Bindings = Source.GetBindingManager.Bindings(GUIDComCat)
			' find the binding by display name.  to do this we enumerate
			' all of the bindings and try to match on the display name
			for each Binding in Bindings
				if Binding.DisplayName = szDisplayName then
					' figure out which set of properties we want to modify
					' based on the szPropertyBag parameter
					select case LCase(szPropertyBag)
						case "source"
							set PropertyBag = Binding.SourceProperties
						case "sink"
							set PropertyBag = Binding.SinkProperties
						case else
							WScript.echo "invalid propertybag: " & szPropertyBag
							exit sub
					end select
					' figure out what operation we want to perform
					select case LCase(szOperation)
						case "remove"
							' they want to remove szPropertyName from the
							' property bag
							PropertyBag.Remove szPropertyName
							WScript.echo "removed property " & szPropertyName
						case "add"
							' add szPropertyName to the property bag and 
							' set its value to szValue.  if this value
							' already exists then this will change  the value
							' it to szValue.
							PropertyBag.Add szPropertyName, szPropertyValue
							WScript.echo "set property " & szPropertyName & " to " & szPropertyValue
						case else
							WScript.echo "invalid operation: " & szOperation
							exit sub
					end select
					' save the binding
					Binding.Save
				end if
			next
		end if
	next
end sub

'
' this helper function takes an IEventSource object and a event category
' and dumps all of the bindings for this category under the source
'
' Source - the IEventSource object to display the bindings for
' GUIDComCat - the event category to display the bindings for
'
public sub DisplaySinksHelper(Source, GUIDComCat)
	Dim Binding
	Dim propval
	' walk each of the registered bindings for this component category
	for each Binding in Source.GetBindingManager.Bindings(GUIDComCat)
		' display the binding properties
		WScript.echo "    Binding " & Binding.ID & " {"
		WScript.echo "      DisplayName = " & Binding.DisplayName
		WScript.echo "      SinkClass = " & Binding.SinkClass
		if Binding.Enabled = True then
			WScript.echo "      Status = Enabled"
		else
			WScript.echo "      Status = Disabled"
		end if

		' walk each of the source properties and display them
		WScript.echo "      SourceProperties {"
		for each propval in Binding.SourceProperties
			WScript.echo "        " & propval & " = " & Binding.SourceProperties.Item(propval)
		next
		WScript.echo "      }"

		' walk each of the sink properties and display them
		WScript.echo "      SinkProperties {"
		for each propval in Binding.SinkProperties
			WScript.echo "        " & propval & " = " & Binding.SinkProperties.Item(propval)
		next
		WScript.echo "      }"
		WScript.echo "    }"
	next
end sub

' check for a previously registered sink with the passed in name
'
' iInstance - the instance to work against
' szEvent - OnMessage
' szDisplayName - the display name of the event to check
' bCheckError - Any errors returned
public sub CheckSink(iInstance, szEvent, szDisplayName, bCheckError)
	Dim SourceType
	Dim GUIDComCat
	Dim szSourceDisplayName
	Dim Source
	Dim Bindings
	Dim Binding

	bCheckError = FALSE
	select case LCase(szEvent)
		case "onpostearly"
			GUIDComCat = GUIDComCatOnPostEarly		
		case "onpost"
			GUIDComCat = GUIDComCatOnPost
		case "onpostfinal"
			GUIDComCat = GUIDComCatOnPostFinal
		case else
			WScript.echo "invalid event: " & szEvent
			exit sub
	end select

	' find the source for this instance
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	szSourceDisplayName = szService & " " & iInstance
	for each Source in SourceType.Sources
		if Source.DisplayName = szSourceDisplayName then
			' find the binding by display name.  to do this we enumerate
			' all of the bindings and try to match on the display name
			set Bindings = Source.GetBindingManager.Bindings(GUIDComCat)
			for each Binding in Bindings
				if Binding.DisplayName = szDisplayName then
					' we've found the binding, now log an error
					WScript.Echo "Binding with the name " & szDisplayName & " already exists"
					exit sub 
				end if
			next
		end if
	next
	bCheckError = TRUE
end sub

'
'
' dumps all of the information in the binding database related to NNTP
'
public sub DisplaySinks
	Dim SourceType
	Dim Source

	' look for each of the sources registered for the NNTP source type
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	for each Source in SourceType.Sources
		' display the source properties
		WScript.echo "Source " & Source.ID & " {"
		WScript.echo "  DisplayName = " & Source.DisplayName
		' display all of the sinks registered for the OnPostEarly event
		WScript.echo "  OnPostEarly Sinks {"
		call DisplaySinksHelper(Source, GUIDComCatOnPostEarly)
		WScript.echo "  }"
		' display all of the sinks registered for the OnPost event
		WScript.echo "  OnPost Sinks {"
		call DisplaySinksHelper(Source, GUIDComCatOnPost)
		WScript.echo "  }"
		' display all of the sinks registered for the OnPostFinal event
		WScript.echo "  OnPostFinal Sinks {"
		call DisplaySinksHelper(Source, GUIDComCatOnPostFinal)
		WScript.echo "  }"
		WScript.echo "}"
	next
end sub

'
' enable/disable a registered sink
'
' iInstance - the instance to work against
' szEvent - OnArrival
' szDisplayName - the display name for this new sink
'
public sub SetSinkEnabled(iInstance, szEvent, szDisplayName, szEnable)
	Dim SourceType
	Dim GUIDComCat
	Dim szSourceDisplayName
	Dim Source
	Dim Bindings
	Dim Binding

	select case LCase(szEvent)
		case "onpostearly"
			GUIDComCat = GUIDComCatOnPostEarly		
		case "onpost"
			GUIDComCat = GUIDComCatOnPost
		case "onpostfinal"
			GUIDComCat = GUIDComCatOnPostFinal
		case else
			WScript.echo "invalid event: " + szEvent
			exit sub
	end select

	' find the source for this instance
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	szSourceDisplayName = szService + " " + iInstance
	for each Source in SourceType.Sources
		if Source.DisplayName = szSourceDisplayName then
			' find the binding by display name.  to do this we enumerate
			' all of the bindings and try to match on the display name
			set Bindings = Source.GetBindingManager.Bindings(GUIDComCat)
			for each Binding in Bindings
				if Binding.DisplayName = szDisplayName then
					' we've found the binding, now enable/disable it
					' we don't need "case else' because szEnable's value
					' is set internally, not by users
					select case LCase(szEnable)
						case "true"
							Binding.Enabled = True
							Binding.Save
							WScript.Echo "enabled " + szDisplayName + " " + Binding.ID
						case "false"
							Binding.Enabled = False
							Binding.Save
							WScript.Echo "disabled " + szDisplayName + " " + Binding.ID
						end select
				end if
			next
		end if
	next
end sub

' 
' display usage information for this script
'
public sub DisplayUsage
	WScript.echo "usage: cscript nntpreg.vbs <command> <arguments>"
	WScript.echo "  commands:"
	WScript.echo "    /add <Instance> <Event> <DisplayName> <SinkClass> <Rule>"
	WScript.echo "    /remove <Instance> <Event> <DisplayName>"
	WScript.echo "    /setprop <Instance> <Event> <DisplayName> <PropertyBag> <PropertyName> "
	WScript.echo "             <PropertyValue>"
	WScript.echo "    /delprop <Instance> <Event> <DisplayName> <PropertyBag> <PropertyName>"
	WScript.echo "    /enable <Instance> <Event> <DisplayName>"
	WScript.echo "    /disable <Instance> <Event> <DisplayName>"
	WScript.echo "    /enum"
	WScript.echo "  arguments:"
	WScript.echo "    <Instance> is the NNTP instance to work against"
	WScript.echo "    <Event> can be OnPostEarly, OnPost or OnPostFinal"
	WScript.echo "    <DisplayName> is the display name of the event to edit"
	WScript.echo "    <SinkClass> is the sink class for the event"
	WScript.echo "    <Rule> is the rule to use for the event"
	WScript.echo "    <PropertyBag> can be Source or Sink"
	WScript.echo "    <PropertyName> is the name of the property to edit"
	WScript.echo "    <PropertyValue> is the value to assign to the property"
end sub


Dim iInstance
Dim szEvent
Dim szDisplayName
Dim szSinkClass
Dim szRule
Dim szPropertyBag
Dim szPropertyName
Dim szPropertyValue
Dim bCheck

'
' this is the main body of our script.  it reads the command line parameters
' specified and then calls the appropriate function to perform the operation
'
if WScript.Arguments.Count = 0 then
	call DisplayUsage
else 
	Select Case LCase(WScript.Arguments(0))
		Case "/add"
			if not WScript.Arguments.Count = 6 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				szSinkClass = WScript.Arguments(4)
				szRule = WScript.Arguments(5)
				call CheckSink(iInstance, szEvent, szDisplayName, bCheck)
				if bCheck = TRUE then
					call RegisterSink(iInstance, szEvent, szDisplayName, szSinkClass, szRule)
				End if
			end if
		Case "/remove"
			if not WScript.Arguments.Count = 4 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				call UnregisterSink(iInstance, szEvent, szDisplayName)		
			end if	
		Case "/setprop"
			if not WScript.Arguments.Count = 7 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				szPropertyBag = WScript.Arguments(4)
				szPropertyName = WScript.Arguments(5)
				szPropertyValue = WScript.Arguments(6)
				call EditProperty(iInstance, szEvent, szDisplayName, szPropertyBag, "add", szPropertyName, szPropertyValue)
			end if
		Case "/delprop"
			if not WScript.Arguments.Count = 6 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				szPropertyBag = WScript.Arguments(4)
				szPropertyName = WScript.Arguments(5)
				call EditProperty(iInstance, szEvent, szDisplayName, szPropertyBag, "remove", szPropertyName, "")		
			end if
		Case "/enable"
			if not WScript.Arguments.Count = 4 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				call SetSinkEnabled(iInstance, szEvent, szDisplayName, "True")
			end if
		Case "/disable"
			if not WScript.Arguments.Count = 4 then
				call DisplayUsage
			else
				iInstance = WScript.Arguments(1)
				szEvent = WScript.Arguments(2)
				szDisplayName = WScript.Arguments(3)
				call SetSinkEnabled(iInstance, szEvent, szDisplayName, "False")
			end if
		Case "/enum"
			if not WScript.Arguments.Count = 1 then
				call DisplayUsage
			else
				call DisplaySinks
			end if
		Case Else
			call DisplayUsage
	End Select
end if

'' SIG '' Begin signature block
'' SIG '' MIIXLwYJKoZIhvcNAQcCoIIXIDCCFxwCAQExCzAJBgUr
'' SIG '' DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
'' SIG '' gjcCAR4wJAIBAQQQTvApFpkntU2P5azhDxfrqwIBAAIB
'' SIG '' AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFBYGE0UfavVK
'' SIG '' Ib5a4v/38mEUAtiaoIISJDCCBGAwggNMoAMCAQICCi6r
'' SIG '' EdxQ/1ydy8AwCQYFKw4DAh0FADBwMSswKQYDVQQLEyJD
'' SIG '' b3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAu
'' SIG '' MR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
'' SIG '' ITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0
'' SIG '' eTAeFw0wNzA4MjIyMjMxMDJaFw0xMjA4MjUwNzAwMDBa
'' SIG '' MHkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
'' SIG '' dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
'' SIG '' aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1p
'' SIG '' Y3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBMIIBIjANBgkq
'' SIG '' hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3l91l2zRTmo
'' SIG '' NKwx2vklNUl3wPsfnsdFce/RRujUjMNrTFJi9JkCw03Y
'' SIG '' SWwvJD5lv84jtwtIt3913UW9qo8OUMUlK/Kg5w0jH9FB
'' SIG '' JPpimc8ZRaWTSh+ZzbMvIsNKLXxv2RUeO4w5EDndvSn0
'' SIG '' ZjstATL//idIprVsAYec+7qyY3+C+VyggYSFjrDyuJSj
'' SIG '' zzimUIUXJ4dO3TD2AD30xvk9gb6G7Ww5py409rQurwp9
'' SIG '' YpF4ZpyYcw2Gr/LE8yC5TxKNY8ss2TJFGe67SpY7UFMY
'' SIG '' zmZReaqth8hWPp+CUIhuBbE1wXskvVJmPZlOzCt+M26E
'' SIG '' RwbRntBKhgJuhgCkwIffUwIDAQABo4H6MIH3MBMGA1Ud
'' SIG '' JQQMMAoGCCsGAQUFBwMDMIGiBgNVHQEEgZowgZeAEFvQ
'' SIG '' cO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQLEyJDb3B5
'' SIG '' cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAuMR4w
'' SIG '' HAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xITAf
'' SIG '' BgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0eYIP
'' SIG '' AMEAizw8iBHRPvZj7N9AMA8GA1UdEwEB/wQFMAMBAf8w
'' SIG '' HQYDVR0OBBYEFMwdznYAcFuv8drETppRRC6jRGPwMAsG
'' SIG '' A1UdDwQEAwIBhjAJBgUrDgMCHQUAA4IBAQB7q65+Siby
'' SIG '' zrxOdKJYJ3QqdbOG/atMlHgATenK6xjcacUOonzzAkPG
'' SIG '' yofM+FPMwp+9Vm/wY0SpRADulsia1Ry4C58ZDZTX2h6t
'' SIG '' KX3v7aZzrI/eOY49mGq8OG3SiK8j/d/p1mkJkYi9/uEA
'' SIG '' uzTz93z5EBIuBesplpNCayhxtziP4AcNyV1ozb2AQWtm
'' SIG '' qLu3u440yvIDEHx69dLgQt97/uHhrP7239UNs3DWkuNP
'' SIG '' tjiifC3UPds0C2I3Ap+BaiOJ9lxjj7BauznXYIxVhBoz
'' SIG '' 9TuYoIIMol+Lsyy3oaXLq9ogtr8wGYUgFA0qvFL0QeBe
'' SIG '' MOOSKGmHwXDi86erzoBCcnYOMIIEejCCA2KgAwIBAgIK
'' SIG '' YQYngQAAAAAACDANBgkqhkiG9w0BAQUFADB5MQswCQYD
'' SIG '' VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
'' SIG '' A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
'' SIG '' IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQg
'' SIG '' Q29kZSBTaWduaW5nIFBDQTAeFw0wODEwMjIyMTI0NTVa
'' SIG '' Fw0xMDAxMjIyMTM0NTVaMIGDMQswCQYDVQQGEwJVUzET
'' SIG '' MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
'' SIG '' bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0
'' SIG '' aW9uMQ0wCwYDVQQLEwRNT1BSMR4wHAYDVQQDExVNaWNy
'' SIG '' b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEB
'' SIG '' AQUAA4IBDwAwggEKAoIBAQC9crSJ5xyfhcd0uGBcAzY9
'' SIG '' nP2ZepopRiKwp4dT7e5GOsdbBQtXqLfKBczTTHdHcIWz
'' SIG '' 5cvfZ+ej/XQnk2ef14oDRDDG98m6yTodCFZETxcIDfm0
'' SIG '' GWiqJBz7BVeF6cVOByE3p+vOLC+2Qs0hBafW5tMoV8cb
'' SIG '' es4pNgfNnlXMu/Ei66gjpA0pwvvQw1o+Yz3HLEkLe3mF
'' SIG '' 8Ijvcb1DWuOjsw3zVfsl4OIg0+eaXpSlMy0of1cbVWoM
'' SIG '' MkTvZmxv8Dic7wKtmqHdmAcQDjwYaeJ5TkYU4LmM0HVt
'' SIG '' nKwAnC1C9VG4WvR4RYPpLnwru13NGWEorZRDCsVqQv+1
'' SIG '' Mq6kKSLeFujTAgMBAAGjgfgwgfUwEwYDVR0lBAwwCgYI
'' SIG '' KwYBBQUHAwMwHQYDVR0OBBYEFCPRcypMvfvlIfpxHpkV
'' SIG '' 0Rf5xKaKMA4GA1UdDwEB/wQEAwIHgDAfBgNVHSMEGDAW
'' SIG '' gBTMHc52AHBbr/HaxE6aUUQuo0Rj8DBEBgNVHR8EPTA7
'' SIG '' MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
'' SIG '' cGtpL2NybC9wcm9kdWN0cy9DU1BDQS5jcmwwSAYIKwYB
'' SIG '' BQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8vd3d3
'' SIG '' Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL0NTUENBLmNy
'' SIG '' dDANBgkqhkiG9w0BAQUFAAOCAQEAQynPY71s43Ntw5nX
'' SIG '' bQyIO8ZIc3olziziN3udNJ+9I86+39hceRFrE1EgAWO5
'' SIG '' cvcI48Z9USoWKNTR55sqzxgN0hNxkSnsVr351sUNL69l
'' SIG '' LW1NRSlWcoRPP9JqHUFiqXlcjvDHd4rLAiguncecK+W5
'' SIG '' Kgnd7Jfi5XqNXhCIU6HdYE93mHFgqFs5kdOrEh8F6cNF
'' SIG '' qdPCUbmvuNz8BoQA9HSj2//MHaAjBQfkJzXCl5AZqoJg
'' SIG '' J+j7hCse0QTLjs+CDdeoTUNAddLe3XfvilxrD4dkj7S6
'' SIG '' t7qrZ1QhRapKaOdUXosUXGd47JBcAxCRCJ0kIJfo3wAR
'' SIG '' cKn5snJwt67iwp8WAjCCBJ0wggOFoAMCAQICCmFJfO0A
'' SIG '' AAAAAAUwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMC
'' SIG '' VVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcT
'' SIG '' B1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
'' SIG '' b3JhdGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVz
'' SIG '' dGFtcGluZyBQQ0EwHhcNMDYwOTE2MDE1NTIyWhcNMTEw
'' SIG '' OTE2MDIwNTIyWjCBpjELMAkGA1UEBhMCVVMxEzARBgNV
'' SIG '' BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQx
'' SIG '' HjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEn
'' SIG '' MCUGA1UECxMebkNpcGhlciBEU0UgRVNOOjEwRDgtNTg0
'' SIG '' Ny1DQkY4MScwJQYDVQQDEx5NaWNyb3NvZnQgVGltZXN0
'' SIG '' YW1waW5nIFNlcnZpY2UwggEiMA0GCSqGSIb3DQEBAQUA
'' SIG '' A4IBDwAwggEKAoIBAQDqugVjyNl5roREPqWzxO1MniTf
'' SIG '' OXYeCdYySlh40ivZpQeQ7+c9+70mfKP75X1+Ms/ZPYs5
'' SIG '' N/L42Ds0FtSSgvs07GiFchqP4LhM4LiF8zMKAsGidnM1
'' SIG '' TF3xt+FKfR24lHjb/x6FFUJGcc5/J1cS0YNPO8/63vaL
'' SIG '' 7T8A49XeYfkXjUukgTz1aUDq4Ym/B0+6dHvpDOVH6qts
'' SIG '' 8dVngQj4Fsp9E7tz4glM+mL77aA5mjr+6xHIYR5iWNgK
'' SIG '' VIPVO0tL4lW9L2AajpIFQ9pd64IKI5cJoAUxZYuTTh5B
'' SIG '' IaKSkP1FREVvNbFFN61pqWX5NEOxF8I7OeEQjPIah+NU
'' SIG '' UB87nTGtAgMBAAGjgfgwgfUwHQYDVR0OBBYEFH5y8C4/
'' SIG '' VingJfdouAH8S+F+z+M+MB8GA1UdIwQYMBaAFG/oTj+X
'' SIG '' uTSrS4aPvJzqrDtBQ8bQMEQGA1UdHwQ9MDswOaA3oDWG
'' SIG '' M2h0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3Js
'' SIG '' L3Byb2R1Y3RzL3RzcGNhLmNybDBIBggrBgEFBQcBAQQ8
'' SIG '' MDowOAYIKwYBBQUHMAKGLGh0dHA6Ly93d3cubWljcm9z
'' SIG '' b2Z0LmNvbS9wa2kvY2VydHMvdHNwY2EuY3J0MBMGA1Ud
'' SIG '' JQQMMAoGCCsGAQUFBwMIMA4GA1UdDwEB/wQEAwIGwDAN
'' SIG '' BgkqhkiG9w0BAQUFAAOCAQEAaXqCCQwW0d7PRokuv9E0
'' SIG '' eoF/JyhBKvPTIZIOl61fU14p+e3BVEqoffcT0AsU+U3y
'' SIG '' hhUAbuODHShFpyw5Mt1vmjda7iNSj1QDjT+nnGQ49jbI
'' SIG '' FEO2Oj6YyQ3DcYEo82anMeJcXY/5UlLhXOuTkJ1pCUyJ
'' SIG '' 0dF2TDQNauF8RKcrW4NUf0UkGSXEikbFJeMZgGkpFPYX
'' SIG '' xvAiLIFGXiv0+abGdz4jb/mmZIWOomINqS0eqOWQPn//
'' SIG '' sI78l+zx/QSvzUnOWnSs+vMTHxs5zqO01rz0tO7IrfJW
'' SIG '' Hvs88cjWKkS8v5w/fWYYzbIgYwrKQD1lMhl8srg9wSZI
'' SIG '' TiIZmW6MMMHxkTCCBJ0wggOFoAMCAQICEGoLmU/AACWr
'' SIG '' EdtFH1h6Z6IwDQYJKoZIhvcNAQEFBQAwcDErMCkGA1UE
'' SIG '' CxMiQ29weXJpZ2h0IChjKSAxOTk3IE1pY3Jvc29mdCBD
'' SIG '' b3JwLjEeMBwGA1UECxMVTWljcm9zb2Z0IENvcnBvcmF0
'' SIG '' aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgUm9vdCBBdXRo
'' SIG '' b3JpdHkwHhcNMDYwOTE2MDEwNDQ3WhcNMTkwOTE1MDcw
'' SIG '' MDAwWjB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
'' SIG '' aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UE
'' SIG '' ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQD
'' SIG '' ExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQTCCASIw
'' SIG '' DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANw3bvuv
'' SIG '' yEJKcRjIzkg+U8D6qxS6LDK7Ek9SyIPtPjPZSTGSKLaR
'' SIG '' ZOAfUIS6wkvRfwX473W+i8eo1a5pcGZ4J2botrfvhbnN
'' SIG '' 7qr9EqQLWSIpL89A2VYEG3a1bWRtSlTb3fHev5+Dx4Df
'' SIG '' f0wCN5T1wJ4IVh5oR83ZwHZcL322JQS0VltqHGP/gHw8
'' SIG '' 7tUEJU05d3QHXcJc2IY3LHXJDuoeOQl8dv6dbG564Ow+
'' SIG '' j5eecQ5fKk8YYmAyntKDTisiXGhFi94vhBBQsvm1Go1s
'' SIG '' 7iWbE/jLENeFDvSCdnM2xpV6osxgBuwFsIYzt/iUW4RB
'' SIG '' hFiFlG6wHyxIzG+cQ+Bq6H8mjmsCAwEAAaOCASgwggEk
'' SIG '' MBMGA1UdJQQMMAoGCCsGAQUFBwMIMIGiBgNVHQEEgZow
'' SIG '' gZeAEFvQcO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQL
'' SIG '' EyJDb3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENv
'' SIG '' cnAuMR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRp
'' SIG '' b24xITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhv
'' SIG '' cml0eYIPAMEAizw8iBHRPvZj7N9AMBAGCSsGAQQBgjcV
'' SIG '' AQQDAgEAMB0GA1UdDgQWBBRv6E4/l7k0q0uGj7yc6qw7
'' SIG '' QUPG0DAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMAQTAL
'' SIG '' BgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zANBgkq
'' SIG '' hkiG9w0BAQUFAAOCAQEAlE0RMcJ8ULsRjqFhBwEOjHBF
'' SIG '' je9zVL0/CQUt/7hRU4Uc7TmRt6NWC96Mtjsb0fusp8m3
'' SIG '' sVEhG28IaX5rA6IiRu1stG18IrhG04TzjQ++B4o2wet+
'' SIG '' 6XBdRZ+S0szO3Y7A4b8qzXzsya4y1Ye5y2PENtEYIb92
'' SIG '' 3juasxtzniGI2LS0ElSM9JzCZUqaKCacYIoPO8cTZXhI
'' SIG '' u8+tgzpPsGJY3jDp6Tkd44ny2jmB+RMhjGSAYwYElvKa
'' SIG '' AkMve0aIuv8C2WX5St7aA3STswVuDMyd3ChhfEjxF5wR
'' SIG '' ITgCHIesBsWWMrjlQMZTPb2pid7oZjeN9CKWnMywd1RR
'' SIG '' OtZyRLIj9jGCBHcwggRzAgEBMIGHMHkxCzAJBgNVBAYT
'' SIG '' AlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQH
'' SIG '' EwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
'' SIG '' cG9yYXRpb24xIzAhBgNVBAMTGk1pY3Jvc29mdCBDb2Rl
'' SIG '' IFNpZ25pbmcgUENBAgphBieBAAAAAAAIMAkGBSsOAwIa
'' SIG '' BQCggaIwGQYJKoZIhvcNAQkDMQwGCisGAQQBgjcCAQQw
'' SIG '' HAYKKwYBBAGCNwIBCzEOMAwGCisGAQQBgjcCARUwIwYJ
'' SIG '' KoZIhvcNAQkEMRYEFCi8g0QTltiLcVsCZEFSa8o7KZOc
'' SIG '' MEIGCisGAQQBgjcCAQwxNDAyoBiAFgBOAE4AVABQAFIA
'' SIG '' ZQBnAC4AVgBiAHOhFoAUaHR0cDovL21pY3Jvc29mdC5j
'' SIG '' b20wDQYJKoZIhvcNAQEBBQAEggEAt+HLvxPr4JOtlqUu
'' SIG '' sf/bKOpQRECtyyQmoVdiKTSyQ/sYwQK5Gme8s0ETjq+C
'' SIG '' stjZfPr6SkxOPST9NbhUt/mZXS4sAekUtyidko2dArvv
'' SIG '' Yrc92Y8Uzd18R142/VmKKfYm17qzqoZVBWWBEHZG7aBS
'' SIG '' TraStMJNNiYpFRmkgGcSjMsPLrkNKJ9lzRBHMRwZBeXC
'' SIG '' +2aWu0rfhNGOEzu1MTLjlx2vhb5Md5onTGZnpyulV64S
'' SIG '' gmmwNoUqCcc8cPXT1VO6dw8h6PXJbyn9VfXG6DWWMm2P
'' SIG '' UDwrV79CJv5IjRQHJJRi8MIHJVM3iVdvmGVP3jwhR28z
'' SIG '' rclTN/uNQImE9OpjKaGCAh8wggIbBgkqhkiG9w0BCQYx
'' SIG '' ggIMMIICCAIBATCBhzB5MQswCQYDVQQGEwJVUzETMBEG
'' SIG '' A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
'' SIG '' ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
'' SIG '' MSMwIQYDVQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5n
'' SIG '' IFBDQQIKYUl87QAAAAAABTAHBgUrDgMCGqBdMBgGCSqG
'' SIG '' SIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkF
'' SIG '' MQ8XDTA5MTAwNzE5NTQzOVowIwYJKoZIhvcNAQkEMRYE
'' SIG '' FP9Xzgn23I7CiCvlMONO4ucWqaEmMA0GCSqGSIb3DQEB
'' SIG '' BQUABIIBAOXDh4FmV0IBEx7Nt5bPH6us3iKtOkxEOhp7
'' SIG '' EYjiuVUCFh01RboxM3bKyF3fNwf7FdiGgJhYSzYgvg6h
'' SIG '' kS2mtoqXVt8q2NW8xY2InXxZ3JwHQK6Mzy2KEkX4ddhE
'' SIG '' b1/wWukWvddWK38K46jz3ApohpOLZTE9lAI4bx0mJAET
'' SIG '' nkLWjAvKx/I3S8edwN1ZaZ/BdNaj9eNWIUywH1pFC7Vi
'' SIG '' ZFm2FGhXr4K9vx7xbhmr7vTJ2oNkWv4cgYBTL2bLfCsB
'' SIG '' pv3Oyrh1Ru9CzgufCeXwbKV0I9gGYL0ItfjGC8UZFdDj
'' SIG '' cyv21OI27WeAS7YZnKP/xvB15M+e/lZy4GJ9XrSFxv0=
'' SIG '' End signature block
