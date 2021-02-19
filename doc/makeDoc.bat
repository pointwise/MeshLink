

rem Location of the Doxygen program on your machine
set DOXYGEN="C:\Program Files\doxygen\bin\doxygen.exe"

rem Location of the Graphviz program directory on your machine
set GVIZ_DIR="C:\Program Files\Graphviz\bin"

SET PATH=%PATH%;%GVIZ_DIR%

set TODAY=%DATE%

%DOXYGEN% ml_doxygen 
