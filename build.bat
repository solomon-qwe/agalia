SETLOCAL
IF NOT EXIST log MKDIR log

SET devenv="%VSINSTALLDIR%Common7\IDE\devenv.com"

%devenv% agalia.sln /Clean "Release|x64"
%devenv% agalia.sln /Clean "Release|x86"
%devenv% agalia.sln /Clean "Debug|x64"
%devenv% agalia.sln /Clean "Debug|x86"

DEL /Q log\*

%devenv% agalia.sln /Build "Release|x64" /Out log\build_x64_Release.log
%devenv% agalia.sln /Build "Release|x86" /Out log\build_x86_Release.log
%devenv% agalia.sln /Build "Debug|x64" /Out log\build_x64_Debug.log
%devenv% agalia.sln /Build "Debug|x86" /Out log\build_x86_Debug.log

%devenv% agalia.sln /Build "Release|x64" /Out log\build_x64_Release.log /project setup.x64
%devenv% agalia.sln /Build "Release|x86" /Out log\build_x86_Release.log /project setup.x86
%devenv% agalia.sln /Build "Debug|x64" /Out log\build_x64_Debug.log /project setup.x64
%devenv% agalia.sln /Build "Debug|x86" /Out log\build_x86_Debug.log /project setup.x86

ENDLOCAL
