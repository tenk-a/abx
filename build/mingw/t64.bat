if "%TDM64_DIR%"=="" (
	set TDM64_DIR=c:\tools\TDM-GCC-64-5.0.2
	call setcc.bat tdm64
)

call mk_mingw.bat 2>err.txt
type err.txt
