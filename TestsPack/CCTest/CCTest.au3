; global
Global $CCPath       = ""
Global $CCWindowName = "iZ3D Control Center"
Global $IgnoreTests  = False
Global $PythonPath   = "../../lib/python27/python.exe"

Global $BaseProfilePath = EnvGet("ProgramData") & "\iZ3D Driver\BaseProfile.xml"
Global $UserProfilePath = EnvGet("APPDATA") & "\iZ3D Driver\UserProfile.xml"

Opt("MouseClickDelay", 200)
Opt("WinWaitDelay", 1000)
Opt("MouseCoordMode", 0)
	
; team-city
Func TestIgnore($name)
    ConsoleWrite("##teamcity[testIgnored name='" & $name & "']" & @CRLF)
EndFunc

Func TestStart($name)
    ConsoleWrite("##teamcity[testStarted name='" & $name & "']" & @CRLF)
EndFunc
 
Func TestEnd($name)
    ConsoleWrite("##teamcity[testFinished name='" & $name & "']" & @CRLF)
EndFunc
 
Func TestFail($name, $message, $details = "")
    ConsoleWrite("##teamcity[testFailed name='" & $name & "' message='" & $message & "' details='" & $details & "']" & @CRLF)
EndFunc

Func TestPythonCheck($name, $script = "")
	If $script = "" Then
		$script = $name & ".py"
	Endif
	$res = RunWait($PythonPath & " " & $script)
	If @error <> 0 Then
		TestFail($name, "Can't run python check script: " & $script)
		return False
	EndIf
	If $res <> 0 Then
		TestFail($name, "Python check script " & $script & " failed")
		return False
	Endif
	return True
EndFunc

; test cases
Func CheckInstall()
	Local $TestName = "Install"
	
	If $IgnoreTests Then
		TestIgnore($TestName)
		Return
	Else
		TestStart($TestName)
	Endif
	Local $UninstallPath = RegRead("HKLM\SOFTWARE\iZ3D\iZ3D Driver", "UninstallExeName")
	If @error <> 0 Then
		TestFail($TestName, "Can't get installation registry value", "HKLM\SOFTWARE\iZ3D\iZ3D Driver\UninstallExeName")
		$IgnoreTests = True
		Return
	Endif
	$CCPath = StringRegExp($UninstallPath, "(.*)\\.*\.exe", 1)
	If @error <> 0 Then
		TestFail($TestName, "Can't retreive control center path", "Uninstall path: " & $UninstallPath)
		$IgnoreTests = True
		Return
	Endif
	$CCPath = $CCPath[0] & "\ControlCenter.exe"
	TestEnd($TestName)
EndFunc

Func CopyProfiles($TestName)
    If Not FileCopy("BaseProfile.xml", $BaseProfilePath, 1) Then
        TestFail($TestName, "Can't copy profiles", "Can't copy base profile to " & $BaseProfilePath)
        Return False
    Endif
    
    If Not FileCopy("UserProfile.xml", $UserProfilePath, 1) Then
        TestFail($TestName, "Can't copy profiles", "Can't copy user profile to " & $UserProfilePath)
        Return False
    Endif
    
    Return True
EndFunc

Func StartCC($TestName) 
    If Not WinExists($CCWindowName) Then
        Run($CCPath)
        If WinWaitActive($CCWindowName, "", 20) = 0 Then
            TestFail($TestName, "Can't start control center", "Timeout reached when starting control center")
            Return False
        Endif
    Endif
    
    Return True
EndFunc

Func CloseCC($TestName)
    If WinExists($CCWindowName) Then
        If WinClose($CCWindowName) = 0 Then
            TestFail($TestName, "Can't close control center", "Can't find control center window")
            Return False
        Endif
        If WinExists($CCWindowName) Then
            TestFail($TestName, "Can't close control center", "Control center not closed after one second")
            Return False
        Endif
    Endif
    
    Return True
EndFunc

Func CheckHotkeys()
	Local $TestName = "Hotkeys"
	
	If $IgnoreTests Then
		TestIgnore($TestName)
		Return
	Else
		TestStart($TestName)
	Endif
    
    If Not CopyProfiles($TestName) Then
        Return
    Endif

    If Not StartCC($TestName) Then
        Return
    Endif
    
    MouseClick("left",170,160,1)
    MouseClick("left",105,240,1)
    MouseClick("left",403,227,1)
    
    ; select DTest
    MouseClick("left",700,139,1)
    MouseMove(697,176)
    MouseDown("left")
    MouseMove(702,222)
    MouseUp("left")
    MouseClick("left",637,236,1)

    MouseClick("left",434,263,1)
    Send("q")
    MouseClick("left",434,285,1)
    Send("w")
    MouseClick("left",431,319,1)
    Send("e")
    MouseClick("left",427,342,1)
    Send("r")
    MouseClick("left",420,371,1)
    Send("t")
    MouseClick("left",420,397,1)
    Send("y")
    MouseClick("left",640,267,1)
    Send("a")
    MouseClick("left",640,282,1)
    MouseClick("left",640,288,1)
    Send("s")
    MouseClick("left",638,319,1)
    Send("d")
    MouseClick("left",644,343,1)
    Send("f")
    MouseClick("left",642,366,1)
    Send("g")
    MouseClick("left",643,392,1)
    Send("h")
    MouseClick("left",655,533,1)

    If Not CloseCC($TestName) Then
        Return
    Endif

	TestPythonCheck($TestName)
	TestEnd($TestName)
EndFunc

Func CheckInGameSettings()
	Local $TestName = "InGameSettings"
	
	If $IgnoreTests Then
		TestIgnore($TestName)
		Return
	Else
		TestStart($TestName)
	Endif
        
    If Not CopyProfiles($TestName) Then
        Return
    Endif
    
    If Not StartCC($TestName) Then
        Return
    Endif
    
    MouseClick("left",170,160,1)
    MouseClick("left",146,239,1)
    
    ; select DTest
    MouseClick("left",700,139,1)
    MouseMove(697,176)
    MouseDown("left")
    MouseMove(702,222)
    MouseUp("left")
    MouseClick("left",637,236,1)
    
    MouseClick("left",411,255,1)
    MouseClick("left",280,266,1)
    MouseClick("left",279,292,1)
    MouseClick("left",281,324,1)
    MouseClick("left",283,354,1)
    MouseClick("left",470,264,1)
    MouseClick("left",471,294,1)
    MouseClick("left",471,326,1)
    MouseClick("left",471,355,1)
    MouseClick("left",443,426,2)
    MouseClick("left",443,426,2)
    MouseClick("left",683,417,2)
    MouseClick("left",683,417,2)
    MouseClick("left",683,417,2)
    MouseClick("left",683,417,2)
    MouseClick("left",683,417,1)
    MouseClick("left",683,417,1)
    MouseClick("left",439,487,1)
    MouseClick("left",418,502,1)
    MouseClick("left",679,483,1)
    MouseClick("left",643,538,1)
    MouseClick("left",643,538,1)

    If Not CloseCC($TestName) Then
        Return
    Endif
    
	TestPythonCheck($TestName)
	TestEnd($TestName)
EndFunc

Func CheckAutofocus()
	Local $TestName = "Autofocus"
	
	If $IgnoreTests Then
		TestIgnore($TestName)
		Return
	Else
		TestStart($TestName)
	Endif
        
    If Not CopyProfiles($TestName) Then
        Return
    Endif
    
    If Not StartCC($TestName) Then
        Return
    Endif
    
    MouseClick("left",182,161,1)
    MouseClick("left",140,243,1)
    MouseClick("left",355,228,1)
    MouseClick("left",694,145,1)
    MouseMove(697,179)
    MouseDown("left")
    MouseMove(695,224)
    MouseUp("left")
    MouseClick("left",371,273,1)
    MouseMove(665,345)
    MouseDown("left")
    MouseMove(666,278)
    MouseUp("left")
    MouseClick("left",443,304,2)
    MouseClick("left",443,304,2)
    MouseClick("left",443,304,2)
    MouseClick("left",443,304,2)
    MouseClick("left",443,304,2)
    MouseClick("left",443,304,2)
    MouseClick("left",446,335,2)
    MouseClick("left",446,335,2)
    MouseClick("left",438,350,2)
    MouseClick("left",438,350,2)
    MouseClick("left",438,350,1)
    MouseClick("left",442,360,2)
    MouseClick("left",442,360,2)
    MouseClick("left",443,391,1)
    MouseClick("left",443,391,2)
    MouseClick("left",443,391,2)
    MouseClick("left",443,391,2)
    MouseClick("left",443,391,2)
    MouseClick("left",443,391,1)
    MouseClick("left",630,530,1)

    If Not CloseCC($TestName) Then
        Return
    Endif
    
	TestPythonCheck($TestName)
	TestEnd($TestName)
EndFunc

; main
ConsoleWrite("##teamcity[testSuiteStarted name='suite.ControlCenter']" & @CRLF)
CheckInstall()
CheckHotkeys()
CheckInGameSettings()
CheckAutofocus()
ConsoleWrite("##teamcity[testSuiteFinished name='suite.ControlCenter']" & @CRLF)