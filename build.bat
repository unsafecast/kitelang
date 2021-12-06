@echo off

rem the "<NUL set /p" thingy is like echo without newline

<NUL set /p=:: building kitec
clang -Ofast -Wall -std=gnu2x -Wextra -D_CRT_SECURE_NO_WARNINGS -static -o kitec.exe kitec_main.c kite_string.c kite_tokenize.c kite_pretty.c kite_parse.c kite_io.c
if %errorlevel% neq 0 exit /b %errorlevel%
echo ...done

echo :: all build tasks done