SETLOCAL
IF NOT EXIST log MKDIR log

SET testproc="%VSINSTALLDIR%Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"

%testproc% x64\Release\unit_test.dll /Diag:log\ut_x64_Release.log /Logger:"trx;LogFileName=..\log\ut_x64_Release.trx" /Platform:x64
%testproc% x86\Release\unit_test.dll /Diag:log\ut_x86_Release.log /Logger:"trx;LogFileName=..\log\ut_x86_Release.trx" /Platform:x86
%testproc% x64\Debug\unit_test.dll   /Diag:log\ut_x64_Debug.log   /Logger:"trx;LogFileName=..\log\ut_x64_Debug.trx"   /Platform:x64
%testproc% x86\Debug\unit_test.dll   /Diag:log\ut_x86_Debug.log   /Logger:"trx;LogFileName=..\log\ut_x86_Debug.trx"   /Platform:x86

ENDLOCAL
