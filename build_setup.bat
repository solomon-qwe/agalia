SETLOCAL
IF NOT EXIST log MKDIR log

SET devenv="%VSINSTALLDIR%Common7\IDE\devenv.com"

%devenv% agalia.sln /Build "Release|x64" /Out log\build_x64_Release.log /project setup.x64
%devenv% agalia.sln /Build "Release|x86" /Out log\build_x86_Release.log /project setup.x86

ENDLOCAL
