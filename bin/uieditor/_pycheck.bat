
@echo off
echo python syntax check...

set pythonhome=D:\Python33\
set checker=%pythonhome%Scripts\pyflakes
%pythonhome%python %checker% res\script

pause
