START "Cppcheck" /D..\ /B /WAIT cppcheck\cppcheck.exe ^
-ilib -iBuildSetup -iMetrics -ibin -iOutputMethods\ColorCodeOutput\LUTs ^
-IOutputMethods\OutputLib -IS3DAPI -IS3DInjector -IS3DWrapper9 -IS3DWrapper10 ^
-IShaderAnalysis -IShared  --enable=all --xml . 2> cppcheck_log.xml
START "Report" /B /WAIT ..\lib\python27\python.exe cppcheck\cppcheck-htmlreport --source-dir=..\ --report-dir=cppcheck_report --file=cppcheck_log.xml