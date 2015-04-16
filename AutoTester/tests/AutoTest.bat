@echo off

rem Change runMode to Debug or Release of your choice
set runMode=Release
set src=cs3202\contains\src.txt
set qry=cs3202\contains\qry.txt
set out=out.xml

set /p params="Enter AutoTester Additional Args: " %=%


:: No additional arguments, run normally
if "%params%" == "" (
  echo Running AutoTester with
  call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out%
)

:: Have additional arguments
for /f "tokens=1" %%G IN ("%params%") DO set p1=%%G
for /f "tokens=2" %%G IN ("%params%") DO set p2=%%G
:: Skip query params
if "%p1%" == "-f" (
  if "%p2%" NEQ "" (
    echo Running AutoTester with
    call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out% %p1% %p2%
  )
)

for /f "tokens=3" %%G IN ("%params%") DO set p3=%%G
for /f "tokens=4" %%G IN ("%params%") DO set p4=%%G
:: Timelimit params
if "%p1%" == "-n" (
  if "%p3%" == "-t" (
    echo Running AutoTester with
    call "..\..\%runMode%\AutoTester.exe" %src% %qry% %out% %p1% %p2% %p3% %p4%
  )
)

