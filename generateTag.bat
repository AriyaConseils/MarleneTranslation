@echo off
setlocal

:: Récupère le dernier tag
for /f "tokens=*" %%i in ('git describe --tags --abbrev=0 2^>nul') do set "LAST_TAG=%%i"

:: Vérifie si un tag existe déjà
if "%LAST_TAG%"=="" (
    set "LAST_TAG=v0.0.0"
)

:: Découpe la version en MAJOR, MINOR et PATCH
for /f "tokens=1,2,3 delims=." %%a in ("%LAST_TAG:v=%") do (
    set "MAJOR=%%a"
    set "MINOR=%%b"
    set "PATCH=%%c"
)

:: Incrémente le numéro de version PATCH
set /a PATCH+=1
set "NEW_TAG=v%MAJOR%.%MINOR%.%PATCH%"

:: Affiche la nouvelle version pour confirmation
echo Last tag: %LAST_TAG%
echo New tag: %NEW_TAG%

:: Créé un nouveau tag et le pousse
git tag -a %NEW_TAG% -m "Release version %NEW_TAG%"
git push origin %NEW_TAG%

endlocal
