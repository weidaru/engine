SET LUA_PATH=./script/?.lua
..\vendor\lua-5.2.3\bin\lua.exe .\script\generate_type_info.lua -s %cd%\src -d %cd%\src\generate\generated_type_info.cpp