cd /d "C:\Users\andre\Projects\ktl\ktl-test" &msbuild "ktl-test.vcxproj" /t:sdvViewer /p:configuration="Debug" /p:platform=x64
exit %errorlevel% 