REM
REM (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
REM
REM This sample source code is not supported by Cadence Design Systems, Inc.
REM It is provided freely for demonstration purposes only.
REM SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
REM

rem Location of the Doxygen program on your machine
set DOXYGEN="C:\Program Files\doxygen\bin\doxygen.exe"

rem Location of the Graphviz program directory on your machine
set GVIZ_DIR="C:\Program Files\Graphviz\bin"

SET PATH=%PATH%;%GVIZ_DIR%

set TODAY=%DATE%

%DOXYGEN% ml_doxygen 

REM
REM This file is licensed under the Cadence Public License Version 1.0 (the
REM "License"), a copy of which is found in the included file named "LICENSE",
REM and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
REM LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
REM ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
REM Please see the License for the full text of applicable terms.
REM
