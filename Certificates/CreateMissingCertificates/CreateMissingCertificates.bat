@ECHO OFF

REM Check whether all certificate files are present

SET REQUIRE_CERTIFICATE_GENERATION=0
 
IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca-key.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server-key.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

IF NOT EXIST "%OPENTWIN_DEV_ROOT%\Certificates\Generated\certificateKeyFile.pem" (
	SET REQUIRE_CERTIFICATE_GENERATION=1
)

REM Create the certificates if needed

IF %REQUIRE_CERTIFICATE_GENERATION%==1 (

	DEL "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca.pem"
	DEL "%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca-key.pem"
	DEL "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server.pem"
	DEL "%OPENTWIN_DEV_ROOT%\Certificates\Generated\server-key.pem"
	DEL "%OPENTWIN_DEV_ROOT%\Certificates\Generated\certificateKeyFile.pem"

	"%CERT_CREATE_TOOLS%\cfssl.exe" gencert -initca ca-csr.json | "%CERT_CREATE_TOOLS%\cfssljson.exe" -bare ca

	"%CERT_CREATE_TOOLS%\cfssl.exe" gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=server server-csr.json | "%CERT_CREATE_TOOLS%\cfssljson.exe" -bare server
	
	TYPE server.pem server-key.pem > certificateKeyFile.pem

	COPY /Y ca.pem                 "%OPENTWIN_DEV_ROOT%\Certificates\Generated\"
	COPY /Y ca-key.pem             "%OPENTWIN_DEV_ROOT%\Certificates\Generated\"
	COPY /Y server.pem             "%OPENTWIN_DEV_ROOT%\Certificates\Generated\"
	COPY /Y server-key.pem         "%OPENTWIN_DEV_ROOT%\Certificates\Generated\"
	COPY /Y certificateKeyFile.pem "%OPENTWIN_DEV_ROOT%\Certificates\Generated\"
	
	DEL ca.pem
	DEL ca-key.pem
	DEL server.pem
	DEL server-key.pem
	DEL ca.csr
	DEL server.csr
	DEL certificateKeyFile.pem

	ECHO SUCCESS: The certificates have been created successfully.
)


