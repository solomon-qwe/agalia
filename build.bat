setlocal
if not exist log mkdir log

for /f "tokens=1,2,*" %%A in ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') do if "%%A"=="15.0" set vs_path=%%C

set devenv="%vs_path%Common7\IDE\devenv.com"

%devenv% agalia.sln /Rebuild "Release|x64" /Out log\build_x64_Release.log
%devenv% agalia.sln /Rebuild "Release|x86" /Out log\build_x86_Release.log
%devenv% agalia.sln /Rebuild "Debug|x64" /Out log\build_x64_Debug.log
%devenv% agalia.sln /Rebuild "Debug|x86" /Out log\build_x86_Debug.log

endlocal
