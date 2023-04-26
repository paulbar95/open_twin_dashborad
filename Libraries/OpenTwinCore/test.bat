@ECHO OFF

REM This script requires the following environment variables to be set:
REM 1. OPENTWIN_DEV_ROOT
REM 2. OPENTWIN_THIRDPARTY_ROOT
REM 2. DEVENV_ROOT_2022
IF "%OPENTWIN_DEV_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_DEV_ROOT
	goto PAUSE_END
)

IF "%OPENTWIN_THIRDPARTY_ROOT%" == "" (
	ECHO Please specify the following environment variables: OPENTWIN_THIRDPARTY_ROOT
	goto PAUSE_END
)

IF "%DEVENV_ROOT_2022%" == "" (
	ECHO Please specify the following environment variables: DEVENV_ROOT_2022
	goto PAUSE_END
)

REM Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Testing Project : OpenTwinCore

REM Open project

SET RELEASE=1
SET DEBUG=1

IF "%1"=="RELEASE" (
  SET RELEASE=1
  SET DEBUG=0
)

IF "%1"=="DEBUG" (
  SET RELEASE=0
  SET DEBUG=1
)

SET TYPE=/Rebuild
SET TYPE_NAME=REBUILD

IF "%2"=="BUILD" (
	SET TYPE=/Build
	SET TYPE_NAME=BUILD
)



IF %DEBUG%==1 (
	ECHO %TYPE% DEBUGTEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_CORE_ROOT%\OpenTwinCore.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%OT_CORE_ROOT%\x64\Debug\OpenTwinCoreTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Reports\OpenTwinCoreDebugReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Reports\OpenTwinCoreDebugReport.xml" "OpenTwinCore" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\EditReports\OpenTwinCoreDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore\OpenTwinCore.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%OPENTWIN_DEV_ROOT%\Libraries\OpenTwinCore\x64\Release\OpenTwinCoreTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Reports\OpenTwinCoreReleaseReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\Reports\OpenTwinCoreReleaseReport.xml" "OpenTwinCore" "%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest\EditReports\OpenTwinCoreReleaseReport.xml"
) 
  
GOTO END

:PAUSE_END
pause
GOTO END

:END
