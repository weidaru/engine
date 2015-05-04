@echo off

mkdir %cd%\log
set LUA_PATH=./script/?.lua
echo ===================Run unit test===================
set unitest_log=%cd%\log\script_unit_test.log
..\vendor\lua-5.2.3\bin\lua.exe .\script\unit_test.lua -d %cd%\script\test > %unitest_log% 2>&1
echo Dump log to %unitest_log%
set generation_log=%cd%\log\generate_type_info.log
if "%errorlevel%"=="0" (
	echo ===================Unit test succeed===================
	echo.
	echo.
	echo.
	echo.
	echo ===================Run generation script===================
	..\vendor\lua-5.2.3\bin\lua.exe .\script\generate_type_info.lua -s %cd%\src -d %cd%\src\generate\generated_type_info.cpp > %generation_log% 2>&1
	echo Dump log to %generation_log%
	type %generation_log%
) else (
	echo ===================Unit test failed!===================
	type %unitest_log%
)





