@echo off

rem Directory of script
set SCRIPT_DIR=%~dp0

rem Location of the SWIG program on your machine
set SWIG_EXE=%SCRIPT_DIR%\swigwin-3.0.12\swig.exe

rem Location of the PYTHON on your machine
set PYTHON_DIR="C:\Python37"
set PYTHON_EXE=%PYTHON_DIR%\python.exe

echo Using SWIG:
%SWIG_EXE% -help


echo Using PYTHON:
%PYTHON_EXE% -V


set SWIG_OPTS=-python   -v -o meshlink_python\MeshLink_wrap.cxx

%SWIG_EXE% %SWIG_OPTS% MeshLink.i