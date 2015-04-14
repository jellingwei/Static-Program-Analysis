@echo off

set runMode=Debug

echo ------------- CS3201 Test Case -------------

set src=cs3201\case0\src.txt
set qry=cs3201\case0\qry.txt
set out=result\case0.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3201\case1while\src.txt
set qry=cs3201\case1while\qry.txt
set out=result\case1while.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3201\case2pattern\src.txt
set qry=cs3201\case2pattern\qry.txt
set out=result\case2pattern.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3201\case3longvar\src.txt
set qry=cs3201\case3longvar\qry.txt
set out=result\case3longvar.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3201\case4if\src.txt
set qry=cs3201\case4if\qry.txt
set out=result\case4if.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

echo ------------- CS3202 Test Case -------------

set src=cs3202\calls\src.txt
set qry=cs3202\calls\qry.txt
set out=result\calls.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\with\src.txt
set qry=cs3202\with\qry.txt
set out=result\with.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\modify\src.txt
set qry=cs3202\modify\qry.txt
set out=result\modify.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\uses\src.txt
set qry=cs3202\uses\qry.txt
set out=result\uses.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%


set src=cs3202\pattern\src.txt
set qry=cs3202\pattern\qry.txt
set out=result\pattern.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\next\src.txt
set qry=cs3202\next\qry.txt
set out=result\next.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\mix\src.txt
set qry=cs3202\mix\qry.txt
set out=result\mix.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\IVLE\src.txt
set qry=cs3202\IVLE\qry.txt
set out=result\IVLE.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\cs3201assign1\src.txt
set qry=cs3202\cs3201assign1\qry.txt
set out=result\assign1.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\follows\src.txt
set qry=cs3202\follows\qry.txt
set out=result\follows.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

set src=cs3202\parent\src.txt
set qry=cs3202\parent\qry.txt
set out=result\parent.xml

echo Running AutoTester with
call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%

echo ------------- End of TestSuite -------------

pause



