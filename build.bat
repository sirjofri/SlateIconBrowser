setlocal

md %temp%\SlateIconBrowser
for %%a in (5.0 5.1 5.2 5.3) do call :build %%a
rd /s /q %temp%\SlateIconBrowser
goto :eof

:build
md %temp%\SlateIconBrowser\%1
for /f "tokens=2* skip=2" %%a in ('reg query "HKLM\SOFTWARE\EpicGames\Unreal Engine\%1" /v "InstalledDirectory"') do set epic=%%b
call "%epic%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CD%\SlateIconBrowser.uplugin" -Package="%temp%\SlateIconBrowser\%1" -Rocket
rd /s /q %temp%\SlateIconBrowser\%1\Binaries
rd /s /q %temp%\SlateIconBrowser\%1\Intermediate
md %temp%\SlateIconBrowser\%1\Config
copy Config\FilterPlugin.ini %temp%\SlateIconBrowser\%1\Config\FilterPlugin.ini
del SlateIconBrowser-%1.zip
tar -a -c -f SlateIconBrowser-%1.zip -C %temp%\SlateIconBrowser\%1\ *
goto :eof
