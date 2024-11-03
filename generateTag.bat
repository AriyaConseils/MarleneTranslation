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

:: Boucle pour trouver un tag unique
:find_unique_tag
set /a PATCH+=1
set "NEW_TAG=v%MAJOR%.%MINOR%.%PATCH%"

:: Vérifie si le tag existe déjà
git rev-parse %NEW_TAG% >nul 2>&1
if not errorlevel 1 (
    goto find_unique_tag
)

:: Affiche la dernière version et la nouvelle version pour confirmation
echo New tag: %NEW_TAG%

:: Crée un nouveau tag et le pousse
git tag -a %NEW_TAG% -m "Release version %NEW_TAG%"
git push origin %NEW_TAG%

endlocal
pause
