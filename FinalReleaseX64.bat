echo Start Time %TIME%
SET ORIGPATH="%CD%"
call "%VS90COMNTOOLS%vsvars32.bat"
cd %ORIGPATH%
vcbuild /M%NUMBER_OF_PROCESSORS% S3DDriver.sln /rebuild "Final Release|x64"
echo End Time %TIME%