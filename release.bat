@echo off
if exist "release" goto purge

:begin
echo [94mCopying files for release... [0m
call robocopy .\ release\eISCP "eISCP.cpp" "eISCP.h" "keywords.txt" "README.md" "LICENSE" /njh /nfl /ndl /njs > nul
if not %ERRORLEVEL% == 1 goto nocopied
call robocopy .\examples release\eISCP\examples /mir /njh /nfl /ndl /njs > nul
if %ERRORLEVEL% == 1 goto copied
:nocopied
echo [93mNo files copied![0m
goto end

:copied
echo [94mCompressing... [0m
cd release
call 7z a eISCP.zip eISCP > nul || goto nocompressed
if exist "eISCP.zip" goto compressed

:nocompressed
echo [93mCompression failed! [0m
goto end

:compressed
echo [94mRemoving files... [0m
timeout /t 1 > nul
call rd /s /q "eISCP" || goto nodelete
forfiles /M "eISCP.zip" /C "cmd /c echo [7;92m Released @ @fdate @ftime [0m"
goto end

:nodelete
echo [93mThere was en error! [0m
goto end

:purge
echo [94mPurging existing files... [0m
timeout /t 1 > nul
call rd /s /q "release" || goto nodelete
goto begin

:end

