
@echo off

call "C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild" /t:Clean,Build /p:Configuration=Release C:\Users\Wei\Documents\CS3202\Code\Group5.sln

pause