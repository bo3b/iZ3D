@echo off
>ftpscript.txt echo !Title Connecting...
>>ftpscript.txt echo open ftp.neurok.ru
>>ftpscript.txt echo developers
>>ftpscript.txt echo NayWotep
>>ftpscript.txt echo !Title Preparing...
>>ftpscript.txt echo cd pub
>>ftpscript.txt echo cd IX
>>ftpscript.txt echo cd iZ3DDriverVersions
>>ftpscript.txt echo mkdir %1
>>ftpscript.txt echo cd %1
IF NOT "%3"=="" (
>>ftpscript.txt echo mkdir %3
>>ftpscript.txt echo cd %3
)
>>ftpscript.txt echo binary
>>ftpscript.txt echo !Title Uploading...
>>ftpscript.txt echo put %2
>>ftpscript.txt echo !Title Disconnecting...
>>ftpscript.txt echo disconnect
>>ftpscript.txt echo quit

@start /w %windir%\system32\ftp.exe -v -s:ftpscript.txt
@del ftpscript.txt
