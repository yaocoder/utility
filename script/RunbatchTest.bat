@echo off
 set b=0
 :a
 set /a b=b+1
 start UnitTest.exe
SET SLEEP=ping 127.0.0.1 -n
����%SLEEP% 30 > nul
 if %b%==200 exit
 goto:a
 
 rem 30  间隔秒数 200 运行实例个数 