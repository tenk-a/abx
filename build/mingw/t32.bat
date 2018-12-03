if "%TDM32_DIR%"=="" (
	set TDM32_DIR=c:\tools\TDM-GCC-32-5.0.3
	call setcc.bat tdm32
)

call mk_mingw.bat
