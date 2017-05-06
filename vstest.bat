setlocal
if not exist log mkdir log

for /f "tokens=1,2,*" %%A in ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') do if "%%A"=="15.0" set vs_path=%%C

set testproc="%vs_path%Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"

%testproc% x64\Release\unit_test.dll /Diag:log\ut_x64_Release.log /Platform:x64
%testproc% x86\Release\unit_test.dll /Diag:log\ut_x86_Release.log /Platform:x86
%testproc% x64\Debug\unit_test.dll /Diag:log\ut_x64_Debug.log /Platform:x64
%testproc% x86\Debug\unit_test.dll /Diag:log\ut_x86_Debug.log /Platform:x86

endlocal
