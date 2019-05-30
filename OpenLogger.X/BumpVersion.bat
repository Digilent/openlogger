@echo off

REM Set this to the path to Openscope.h
set "f=..\OpenLogger\Version.c"

powershell -Command "& {$file= Get-Content %f%;foreach($line in $file.length..0) {if($file[$line] -match \"VER_MINOR\"){$ver=$file[$line]}};$vernum=$ver -replace '\D+(\d+)','$1';$num=[INT]$vernum;$num++;$file=$file -replace \"$ver\",\"#define VER_MINOR $num\";Set-Content -Path %f% -Value $file;}"

