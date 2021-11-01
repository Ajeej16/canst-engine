@echo off

set application_name=canst
set build_options=
set compiler_flags= -O2 -nologo -Zi -FC -wd4533 -I ..\canst\src
set linker_flags=-opt:ref -incremental:no -Debug:fastlink
set platform_linker_flags=user32.lib winmm.lib gdi32.lib %linker_flags%

if not exist ..\build mkdir ..\build
pushd ..\build

cl %build_options% %compiler_flags% ..\canst\src\win32\win32_canst.cpp /link %platform_linker_flags% /out:%application_name%.exe

REM start /b /wait "" "cl.exe" %build_options% %comiler_flags% ..\..\src\app.c /LC /link %common_linker_flags% /out:%application_name%.dll

popd