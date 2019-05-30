@echo off

REM OpenLogger.bat "image" "suffix" "bootloader"

del /Q ..\OpenLogger*.%2

for /f "usebackq delims=" %%x in (`date /t`)do set dat=%%x
for /f "usebackq delims=" %%x in (`time /t`)do set tim=%%x

set dat=%dat:/=-%
set "dat=%dat: =-%"
set "tim=%tim: =-%"
set "tim=%tim::=-%"

REM Set this to the path to OpenLogger.h
set "f=..\OpenLogger\Version.c"

for /F "delims=" %%v IN ('powershell -Command "& {$file= Get-Content %f%;foreach($line in $file.length..0) {if($file[$line] -match \"VER_MAJOR\"){$ver=$file[$line]}};$vernum=$ver -replace '\D+(\d+)','$1';$num=[INT]$vernum;return $num;}"') DO set CURRENT_MAJOR_VER=%%v

for /F "delims=" %%v IN ('powershell -Command "& {$file= Get-Content %f%;foreach($line in $file.length..0) {if($file[$line] -match \"VER_MINOR\"){$ver=$file[$line]}};$vernum=$ver -replace '\D+(\d+)','$1';$num=[INT]$vernum;return $num;}"') DO set CURRENT_MINOR_VER=%%v

for /F "delims=" %%v IN ('powershell -Command "& {$file= Get-Content %f%;foreach($line in $file.length..0) {if($file[$line] -match \"VER_PATCH\"){$ver=$file[$line]}};$vernum=$ver -replace '\D+(\d+)','$1';$num=[INT]$vernum;return $num;}"') DO set CURRENT_PATCH_VER=%%v



set ufname=OpenLoggerMZ-unified-%CURRENT_MAJOR_VER%.%CURRENT_MINOR_VER%.%CURRENT_PATCH_VER%.%2
set fname=OpenLoggerMZ-%CURRENT_MAJOR_VER%.%CURRENT_MINOR_VER%.%CURRENT_PATCH_VER%.%2

hexmate %1 %3 -O..\%ufname%

copy %1 ..\%fname%
