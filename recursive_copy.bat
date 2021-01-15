@ECHO off
SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

SET me=%~n0
SET path=%~dp0
FOR %%d IN ("%~dp0.") DO SET dir=%%~nxd

SET src=%path%
SET dst=%path%../%dir%_NO_SPACES/

ECHO %me%: ----- Copy Parameters -----
ECHO %me%:      Source "%src%"
ECHO %me%: Destination "%dst%"

ECHO %me%: ----- Copying -----

CALL :RECURSIVE_COPY "%src%" "%dst%"

ECHO %me%: ----- Finished -----
PAUSE
EXIT /B

REM Call using a destination path.
REM Example:
REM		SET test="./This is a/test"
REM		CALL :GUARANTEE_DIRECTORY "%test%"
REM		PAUSE
:GUARANTEE_DIRECTORY
IF EXIST "%~1" ( ECHO %me%: "%~1" exists ) ELSE ( MKDIR "%~1" && ECHO %me%: "%~1" creating )
IF NOT EXIST "%~1" ( ECHO %me%: "%~1" could be created )
EXIT /B

REM Copies a directory in depth-first recursion.
REM Call using a source path and a destination path.
REM Example:
REM		SET test="./This is a/test"
REM		SET test2="./This is a/test2"
REM		CALL :RECURSIVE_COPY "%test%" "%test2%"
REM		PAUSE
:RECURSIVE_COPY
CALL :GUARANTEE_DIRECTORY "%~2"
FOR %%f IN ("%~1/*") DO (
	ECHO %me%: Src: "%~1%%~nxf"
	ECHO %me%: Dst: "%~2%%~nxf"
	COPY "%~1%%~nxf" "%~2%%~nxf"
	PAUSE
)
FOR /D %%f IN ("%~1/*") DO (
REM	ECHO %me%: Src: "%~1%%~nxf"
REM	ECHO %me%: Dst: "%~2%%~nxf"
	CALL :RECURSIVE_COPY "%~1%%~nxf/" "%~2%%~nxf/"
)
EXIT /B