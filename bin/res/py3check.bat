@echo off

set PYTHON_EXE=d:\python33\python
set PYFLAKES=D:\Python33\Scripts\pyflakes

:start
cls
echo python syntax check start...

%PYTHON_EXE% %PYFLAKES% script/

choice /M "check finish continue\exit"
if errorlevel 2 goto end
if errorlevel 1 goto start

:end
