Установка на видоус
### Для доступа к инфо фото
.\vcpkg install exiv2
.\vcpkg install opencv
.\vcpkg install boost
.\vcpkg install libqrencode

# Настройка neovim для windows
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
   
### Установка LUa
.\vcpkg search LUa
https://github.com/LuaLS/lua-language-server/releases/download/3.13.5/lua-language-server-3.13.5-win32-x64.zip
   
   
### scoop
https://scoop.sh/
Откройте терминал PowerShell (версия 5.1 или более поздняя) и в командной строке

Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
Invoke-RestMethod -Uri https://get.scoop.sh | Invoke-Expression

scoop install lua-language-server

### nsp
https://nodejs.org/en

### clangd
файл конфигурации:
Windows: %LocalAppData%\clangd\config.yaml, как правило C:\Users\Bob\AppData\Local\clangd\config.yaml.
-------------------------------------------------------------


cmake -DCMAKE_TOOLCHAIN_FILE=D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
