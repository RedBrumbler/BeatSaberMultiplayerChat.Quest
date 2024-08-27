@REM set /p ndkpath=<ndkpath.txt
@REM %ndkpath%\toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-addr2line.exe -f -C --obj=%1 %2
%ANDROID_NDK_HOME%\toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-addr2line.exe -f -C --obj=%1 %2