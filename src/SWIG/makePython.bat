REM
REM (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
REM
REM This sample source code is not supported by Cadence Design Systems, Inc.
REM It is provided freely for demonstration purposes only.
REM SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
REM

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

REM
REM This file is licensed under the Cadence Public License Version 1.0 (the
REM "License"), a copy of which is found in the included file named "LICENSE",
REM and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
REM LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
REM ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
REM Please see the License for the full text of applicable terms.
REM
