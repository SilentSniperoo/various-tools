@ECHO off
SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

REM This batch file will rename all files so spaces are replaced with underscores.
REM It chooses files from the folder the batch file is in (to change set `src`).
REM It places the renamed files in a folder named NO_SPACES (to change set `dst`).

REM Why? For consistency and because various programs require no spaces.

SET me=%~n0
SET path=%~dp0
FOR %%d IN ("%~dp0.") DO SET dir=%%~nxd
FOR %%d IN ("%~dp0..") DO SET parent=%%~nxd

SET src=%path%
SET dst=%path%../%dir%_NO_SPACES/

ECHO %me%: ----- Renaming Parameters -----
ECHO %me%:      Source "%src%"
ECHO %me%: Destination "%dst%"

ECHO %me%: ----- Copying Source -----

CALL :RECURSIVE_COPY "%src%" "%dst%"

ECHO %me%: ----- Renaming Copies -----

CALL :RECURSIVE_REMOVE_SPACES "%dst%"

ECHO %me%: ----- Renaming Finished -----
PAUSE
EXIT /B

REM Call using a variable name as both input and output.
REM Example:
REM		SET test="This is a test"
REM		ECHO %test%
REM		CALL :REMOVE_SPACES "test"
REM		ECHO %test%
REM		PAUSE
:REMOVE_SPACES
SET "%~1=!%~1: =_!"
EXIT /B

REM Call using a path as both source and destination.
REM Example:
REM		SET test="./This is a/test/"
REM		CALL :RECURSIVE_REMOVE_SPACES "%test%"
REM		PAUSE
:RECURSIVE_REMOVE_SPACES
SET r=""
FOR %%f IN ("%~1/*") DO (
	SET r=%%~nxf
	SET r=!r: =_!
	SET r=!r:,=!
	ECHO %me%: Src: "%~1%%~nxf"
	ECHO %me%: Dst: "%~1!r!"
	MOVE "%~1%%~nxf" "%~1!r!"
	PAUSE
)
FOR /D %%f IN ("%~1/*") DO (
	SET r=%%~nxf
	SET r=!r: =_!
	SET r=!r:,=!
	ECHO %me%: Src: "%~1%%~nxf"
	ECHO %me%: Dst: "%~1!r!"
	MOVE "%~1%%~nxf" "%~1!r!"
	CALL :RECURSIVE_REMOVE_SPACES "%~1!r!/"
	PAUSE
)
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
	COPY "%~1%%~nxf" "%~2%%~nxf"
)
FOR /D %%f IN ("%~1/*") DO (
	CALL :RECURSIVE_COPY "%~1%%~nxf/" "%~2%%~nxf/"
)
EXIT /B