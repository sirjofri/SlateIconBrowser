setlocal

md BuildResults
md %temp%\SlateStyleBrowser
for %%a in (4.27 5.0 5.1 5.2 5.3 5.4 5.5) do call :build %%a
rd /s /q %temp%\SlateStyleBrowser
goto :eof

:build
set epic=
for /f "tokens=2* skip=2" %%a in ('reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine\%1" /v "InstalledDirectory"') do set epic=%%b
if "%epic%"=="" goto :eof
md %temp%\SlateStyleBrowser\%1
call "%epic%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CD%\SlateStyleBrowser.uplugin" -Package="%temp%\SlateStyleBrowser\%1" -Rocket >BuildResults\LogBuild-%1.txt
rd /s /q %temp%\SlateStyleBrowser\%1\Binaries
rd /s /q %temp%\SlateStyleBrowser\%1\Intermediate
md %temp%\SlateStyleBrowser\%1\Config
copy Config\FilterPlugin.ini %temp%\SlateStyleBrowser\%1\Config\FilterPlugin.ini
del SlateStyleBrowser-%1.zip
if not exist %temp%\SlateStyleBrowser\%1\SlateStyleBrowser.uplugin goto :eof
tar -a -c -f BuildResults\SlateStyleBrowser-%1.zip -C %temp%\SlateStyleBrowser\%1\ *
goto :eof
