'THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT 
'WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
'INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
'OF MERCHANTABILITY AND/OR FITNESS FOR A  PARTICULAR 
'PURPOSE

'------------------------------------------------------------------------------
'FILE DESCRIPTION: Script for registering for SMTP Protocol sinks. 
'
'File Name: smtpreg.vbs
' 
'
' Copyright (c) Microsoft Corporation 1993-1999. All rights reserved.
'------------------------------------------------------------------------------
Option Explicit

'
'
' the OnArrival event GUID
Const GUIDComCatOnArrival = "{ff3caa23-00b9-11d2-9dfb-00C04FA322BA}"
' the SMTP source type
Const GUIDSourceType = "{FB65C4DC-E468-11D1-AA67-00C04FA345F6}"
' 
Const GUIDCat = "{871736c0-fd85-11d0-869a-00c04fd65616}"
Const GUIDSources = "{DBC71A31-1F4B-11d0-869D-80C04FD65616}"

' the SMTP service display name.  This is used to key which service to
' edit
Const szService = "smtpsvc"

' the event manager object.  This is used to communicate with the 
' event binding database.
Dim EventManager
Set EventManager = WScript.CreateObject("Event.Manager")

'
' register a new sink with event manager
'
' iInstance - the instance to work against
' szEvent - OnArrival
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
		case "onarrival"
			GUIDComCat = GUIDComCatOnArrival
		case else
			WScript.echo "invalid event: " & szEvent
			exit sub
	end select
	' enumerate through each of the registered instances for the SMTP source
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
' check for a previously registered sink with the passed in name
'
' iInstance - the instance to work against
' szEvent - OnArrival
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
		case "onarrival"
			GUIDComCat = GUIDComCatOnArrival
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
' unregister a previously registered sink
'
' iInstance - the instance to work against
' szEvent - OnArrival
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
		case "onarrival"
			GUIDComCat = GUIDComCatOnArrival
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
' iInstance - the SMTP instance to edit
' szEvent - the event type (OnArrival)
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
		case "onarrival"
			GUIDComCat = GUIDComCatOnArrival
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
		for each Propval in Binding.SinkProperties
			WScript.echo "        " & propval & " = " & Binding.SinkProperties.Item(Propval)
		next
		WScript.echo "      }"
		WScript.echo "    }"
	next
end sub

'
' dumps all of the information in the binding database related to SMTP
'
public sub DisplaySinks
	Dim SourceType
	Dim Source

	' look for each of the sources registered for the SMTP source type
	set SourceType = EventManager.SourceTypes(GUIDSourceType)
	for each Source in SourceType.Sources
		' display the source properties
		WScript.echo "Source " & Source.ID & " {"
		WScript.echo "  DisplayName = " & Source.DisplayName
		' display all of the sinks registered for the OnArrival event
		WScript.echo "  OnArrival Sinks {"
		call DisplaySinksHelper(Source, GUIDComCatOnArrival)
		WScript.echo "  }"
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
		case "onarrival"
			GUIDComCat = GUIDComCatOnArrival
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
	WScript.echo "usage: cscript smtpreg.vbs <command> <arguments>"
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
	WScript.echo "    <Instance> is the SMTP instance to work against"
	WScript.echo "    <Event> can be OnArrival"
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
dim bCheck

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
'' SIG '' AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFI3Vwo/zZkp/
'' SIG '' vqByjbmIq6ZOYpW3oIISJDCCBGAwggNMoAMCAQICCi6r
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
'' SIG '' cKn5snJwt67iwp8WAjCCBJ0wggOFoAMCAQICCmFHUroA
'' SIG '' AAAAAAQwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMC
'' SIG '' VVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcT
'' SIG '' B1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
'' SIG '' b3JhdGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVz
'' SIG '' dGFtcGluZyBQQ0EwHhcNMDYwOTE2MDE1MzAwWhcNMTEw
'' SIG '' OTE2MDIwMzAwWjCBpjELMAkGA1UEBhMCVVMxEzARBgNV
'' SIG '' BAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQx
'' SIG '' HjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEn
'' SIG '' MCUGA1UECxMebkNpcGhlciBEU0UgRVNOOkQ4QTktQ0ZD
'' SIG '' Qy01NzlDMScwJQYDVQQDEx5NaWNyb3NvZnQgVGltZXN0
'' SIG '' YW1waW5nIFNlcnZpY2UwggEiMA0GCSqGSIb3DQEBAQUA
'' SIG '' A4IBDwAwggEKAoIBAQCbbdyGUegyOzc6liWyz2/uYbVB
'' SIG '' 0hg7Wp14Z7r4H9kIVZKIfuNBU/rsKFT+tdr+cDuVJ0h+
'' SIG '' Q6AyLyaBSvICdnfIyan4oiFYfg29Adokxv5EEQU1OgGo
'' SIG '' 6lQKMyyH0n5Bs+gJ2bC+45klprwl7dfTjtv0t20bSQvm
'' SIG '' 08OHbu5GyX/zbevngx6oU0Y/yiR+5nzJLPt5FChFwE82
'' SIG '' a1Map4az5/zhwZ9RCdu8pbv+yocJ9rcyGb7hSlG8vHys
'' SIG '' LJVql3PqclehnIuG2Ju9S/wnM8FtMqzgaBjYbjouIkPR
'' SIG '' +Y/t8QABDWTAyaPdD/HI6VTKEf/ceCk+HaxYwNvfqtyu
'' SIG '' ZRvTnbxnAgMBAAGjgfgwgfUwHQYDVR0OBBYEFE8YiYrS
'' SIG '' ygB4xuxZDQ/9fMTBIoDeMB8GA1UdIwQYMBaAFG/oTj+X
'' SIG '' uTSrS4aPvJzqrDtBQ8bQMEQGA1UdHwQ9MDswOaA3oDWG
'' SIG '' M2h0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3Js
'' SIG '' L3Byb2R1Y3RzL3RzcGNhLmNybDBIBggrBgEFBQcBAQQ8
'' SIG '' MDowOAYIKwYBBQUHMAKGLGh0dHA6Ly93d3cubWljcm9z
'' SIG '' b2Z0LmNvbS9wa2kvY2VydHMvdHNwY2EuY3J0MBMGA1Ud
'' SIG '' JQQMMAoGCCsGAQUFBwMIMA4GA1UdDwEB/wQEAwIGwDAN
'' SIG '' BgkqhkiG9w0BAQUFAAOCAQEANyce9YxA4PZlJj5kxJC8
'' SIG '' PuNXhd1DDUCEZ76HqCra3LQ2IJiOM3wuX+BQe2Ex8xoT
'' SIG '' 3oS96mkcWHyzG5PhCCeBRbbUcMoUt1+6V+nUXtA7Q6q3
'' SIG '' P7baYYtxz9R91Xtuv7TKWjCR39oKDqM1nyVhTsAydCt6
'' SIG '' BpRyAKwYnUvlnivFOlSspGDYp/ebf9mpbe1Ea7rc4BL6
'' SIG '' 8K2HDJVjCjIeiU7MzH6nN6X+X9hn+kZL0W0dp33SvgL/
'' SIG '' 826C84d0xGnluXDMS2WjBzWpRJ6EfTlu/hQFvRpQIbU+
'' SIG '' n/N3HI/Cmp1X4Wl9aeiDzwJvKiK7NzM6cvrWMB2RrfZQ
'' SIG '' GusT3jrFt1zNszCCBJ0wggOFoAMCAQICEGoLmU/AACWr
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
'' SIG '' KoZIhvcNAQkEMRYEFH/8/6cpzCa/KIvvGNkJxBOn8qZU
'' SIG '' MEIGCisGAQQBgjcCAQwxNDAyoBiAFgBTAE0AVABQAFIA
'' SIG '' ZQBnAC4AVgBiAHOhFoAUaHR0cDovL21pY3Jvc29mdC5j
'' SIG '' b20wDQYJKoZIhvcNAQEBBQAEggEAPFsD2bh8cdqXN0gC
'' SIG '' q3pfidO+B78LsyHq/iCNwcfDMpcohuIwcglI7i+yvJzV
'' SIG '' gsUVr/N1J9v1/5q5/aUDuK06Cq61YtL32OP48CZD3ojF
'' SIG '' vrdnCAqKLCNPqggakgaVsPvvuf51SYpsoLZIz0mnZPrb
'' SIG '' 20pl10+Z+m7DgdfV+wB5BOaTTzsd7lt9I1TQaZGSLrvj
'' SIG '' 80d9KOnEfyiZgem0ewiMSu9nxy7ahg26oVPd1WCXepeY
'' SIG '' o8SljLlmPBRHCuOGZAxlBOjxSxpnjwVjwpb5DQhaizaU
'' SIG '' NaH1Yv5wu7vXAl92F/CdnnBkLdVxhnF2tBjdBRcMQN8/
'' SIG '' 12gNnG0mu8kxSIRkmaGCAh8wggIbBgkqhkiG9w0BCQYx
'' SIG '' ggIMMIICCAIBATCBhzB5MQswCQYDVQQGEwJVUzETMBEG
'' SIG '' A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
'' SIG '' ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
'' SIG '' MSMwIQYDVQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5n
'' SIG '' IFBDQQIKYUdSugAAAAAABDAHBgUrDgMCGqBdMBgGCSqG
'' SIG '' SIb3DQEJAzELBgkqhkiG9w0BBwEwHAYJKoZIhvcNAQkF
'' SIG '' MQ8XDTA5MTAwNzE5NTQzOVowIwYJKoZIhvcNAQkEMRYE
'' SIG '' FC0IEtmPVbQtW81dofRkwuHrf62nMA0GCSqGSIb3DQEB
'' SIG '' BQUABIIBAGgwgxvsnoeFewwXRK4+NxRveDdrFjg452s1
'' SIG '' RqzCsmjDCwG7tEZlKxW1E2tWjFiG2pVxdfr3zVcO6Cna
'' SIG '' 68jVnxwTiNNzdVCnwb3GERlQsN6AvbqcJuFXOPj/f4h6
'' SIG '' 26ITzubJ2MZ8eALqRhFiuVLVBjGiibqyzoPtwN40MlTn
'' SIG '' LbTJD0Jh3UArlQMjNLksKR4DQLL21PdENzJ2ghe+7ySX
'' SIG '' /8DuOOMkn09SjDSeO3G8ZvcbizwlHwzhemg5r1SyjXQB
'' SIG '' kneq1M2bo4+VSQycHtUIfk6CPIG8kjXhatKfs7Cr49XW
'' SIG '' ieAO6og439826kyH2FAYM9QHg7Dc6IzG64e2XYHtYL0=
'' SIG '' End signature block
