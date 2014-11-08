@echo off

set LUA_PATH=./script/?.lua
echo ===================Run unit test===================
set LOG_FILE=%cd%\log\script_unit_test.log
..\vendor\lua-5.2.3\bin\lua.exe .\script\unit_test.lua -d %cd%\script\test > %LOG_FILE%
echo Dump log to %LOG_FILE%
echo.
echo.
if "%errorlevel%"=="0" (
	echo ===================Run generation script===================
	set LOG_FILE=%cd%\log\generate_type_info.log
	..\vendor\lua-5.2.3\bin\lua.exe .\script\generate_type_info.lua -s %cd%\src -d %cd%\src\generate\generated_type_info.cpp > %LOG_FILE%
	echo Dump log to %LOG_FILE%
	type %LOG_FILE%
) else (
	echo Unit test fails.
	type %LOG_FILE%
)





