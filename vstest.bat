setlocal
if not exist log mkdir log

set testproc="%VS140COMNTOOLS%..\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe"

%testproc% x64\Release_static\unit_test.dll /Diag:log\ut_x64rel_sta.log /Platform:x64
%testproc% x86\Release_static\unit_test.dll /Diag:log\ut_x86rel_sta.log /Platform:x86
%testproc% x64\Debug_static\unit_test.dll /Diag:log\ut_x64dbg_sta.log /Platform:x64
%testproc% x86\Debug_static\unit_test.dll /Diag:log\ut_x86dbg_sta.log /Platform:x86
%testproc% x64\Release\unit_test.dll /Diag:log\ut_x64rel_dyn.log /Platform:x64
%testproc% x86\Release\unit_test.dll /Diag:log\ut_x86rel_dyn.log /Platform:x86
%testproc% x64\Debug\unit_test.dll /Diag:log\ut_x64dbg_dyn.log /Platform:x64
%testproc% x86\Debug\unit_test.dll /Diag:log\ut_x86dbg_dyn.log /Platform:x86

endlocal
