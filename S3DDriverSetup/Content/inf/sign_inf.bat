set WINDDKDIR=C:\WinDDK\7100.0.0
%WINDDKDIR%\bin\SelfSign\inf2cat.exe /driver:. /os:XP_X86,XP_X64,Vista_x86,Vista_X64,7_X86,7_X64,Server2003_X86,Server2003_X64,Server2008_X86,Server2008_X64,Server2008R2_X64
rem Private certificate
rem %WINDDKDIR%\bin\SelfSign\SignTool sign /s PrivateCertStore iz3d.cat
rem Public cetrificate
%WINDDKDIR%\bin\x86\Signtool sign /v /ac ..\lib\sign\MSCV-VSClass3.cer /s My /n iZ3D /t http://timestamp.verisign.com/scripts/timstamp.dll iz3d.cat