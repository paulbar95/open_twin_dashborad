@ECHO OFF

ECHO Setting up environment

rem Setup eviroment
CALL "%OPENTWIN_DEV_ROOT%\Scripts\SetupEnvironment.bat"

ECHO Testing Project : uiService

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
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_UI_SERVICE_ROOT%\uiService.vcxproj" %TYPE% "DebugTest|x64"  
	ECHO %TYPE% DEBUG
	"%OT_UI_SERVICE_ROOT%\%OT_DLLD%\uiServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\uiServiceDebugReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\uiServiceDebugReport.xml" "uiService" "%OPENTWIN_DEV_ROOT%\Scripts\EditReports\uiServiceDebugReport.xml"
)

IF %RELEASE%==1 (
	ECHO %TYPE% RELEASETEST
	"%DEVENV_ROOT_2022%\devenv.exe" "%OT_UI_SERVICE_ROOT%\uiService.vcxproj" %TYPE% "ReleaseTest|x64"
	ECHO %TYPE% RELEASE
	"%OT_UI_SERVICE_ROOT%\%OT_DLLR%\uiServiceTest.exe" /Out --gtest_output="xml:%OPENTWIN_DEV_ROOT%\Scripts\Reports\uiServiceReleaseReport.xml"
	CALL "%OPENTWIN_THIRDPARTY_ROOT%\Python\set_paths_dev.bat"
	python "%OPENTWIN_DEV_ROOT%\Scripts\modifyXML.py" "%OPENTWIN_DEV_ROOT%\Scripts\Reports\uiServiceReleaseReport.xml" "uiService" "%OPENTWIN_DEV_ROOT%\Scripts\EditReports\uiServiceReleaseReport.xml"
) 
  
:END








