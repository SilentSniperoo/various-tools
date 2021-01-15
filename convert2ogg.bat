@ECHO off
SETLOCAL ENABLEEXTENSIONS

REM THIS BATCH FILE REQUIRES THAT VLC BE INSTALLED
REM To ensure that VLC is found, set the variable named `vlc` below to the VLC path.

REM Many game engines accept both ".wav" and ".ogg" audio file formats
REM A ".wav" file is very large and unnecessarily so.
REM Therefore, it is preferable to use the ".ogg" file in almost all cases.

REM This batch file will convert all files of certain extensions to the .ogg format.
REM It chooses extensions by the comma delineated string variable named `ext` below.
REM It chooses files from the folder the batch file is in (to change set `src`).
REM It places the converted files in a folder named OGG (to change set `dst`).

SET me=%~n0
SET parent=%~dp0

SET vlc=C:\Program Files (x86)\VideoLAN\VLC\vlc.exe
SET src=%parent%
SET dst=%parent%OGG
SET ext=aif, caf, mp3, wav

ECHO %me%: ----- Conversion Parameters -----
ECHO %me%:     Program "%vlc%"
ECHO %me%:      Source "%src%"
ECHO %me%: Destination "%dst%"
ECHO %me%:  Extensions "%ext%"

ECHO %me%: ----- Checking Destination -----
IF EXIST "%dst%" ( ECHO %me%: "%dst%" exists ) ELSE ( MKDIR "%dst%" && ECHO %me%: "%dst%" creating )
IF NOT EXIST "%dst%" ( ECHO %me%: "%dst%" could be created )

ECHO %me%: ----- Converting -----
FOR %%e IN (%ext%) DO (
    ECHO %me%: "%%e" to ".ogg"
    FOR %%f IN ("%src%*.%%e") DO (
        ECHO %me%: Convert: "%src%/%%~nf.%%e"
        ECHO %me%: To ogg:  "%dst%/%%~nf.ogg"
        CALL "%vlc%" "%src%/%%~nf.%%e" --sout="#transcode{vcodec=none,acodec=vorb,ab=128,channels=2,samplerate=44100}:file{dst="%dst%/%%~nf.ogg"}"  vlc://quit
    )
)

ECHO %me%: ----- Conversion Finished -----
PAUSE