setlocal
if not exist log mkdir log

set devenv="%VS140COMNTOOLS%..\IDE\devenv.com"

%devenv% agalia.sln /Rebuild "Release_static|x64" /Out log\build_x64_Release_static.log
%devenv% agalia.sln /Rebuild "Release_static|x86" /Out log\build_x86_Release_static.log
%devenv% agalia.sln /Rebuild "Debug_static|x64" /Out log\build_x64_Debug_static.log
%devenv% agalia.sln /Rebuild "Debug_static|x86" /Out log\build_x86_Debug_static.log
%devenv% agalia.sln /Rebuild "Release|x64" /Out log\build_x64_Release.log
%devenv% agalia.sln /Rebuild "Release|x86" /Out log\build_x86_Release.log
%devenv% agalia.sln /Rebuild "Debug|x64" /Out log\build_x64_Debug.log
%devenv% agalia.sln /Rebuild "Debug|x86" /Out log\build_x86_Debug.log

endlocal
