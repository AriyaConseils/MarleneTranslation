@echo off
REM Vérifie le mode de compilation passé en argument, par défaut en Release
set "BUILDMODE=release"
if /I "%~1" == "debug" (
    set "BUILDMODE=debug"
)

REM Définir les chemins nécessaires avec les chemins complets
set "QTPATH=C:\Qt\6.7.2\mingw_64"
set "COMPILERPATH=C:\Qt\Tools\mingw1310_64\bin"
set "PROJECTPATH=%cd%"
set "BUILDPATH=%PROJECTPATH%\build"
set "INSTALLPATH=%PROJECTPATH%\installMarlene"

REM Supprimer et recréer le dossier installMarlene
if exist "%INSTALLPATH%" (
    rmdir /S /Q "%INSTALLPATH%"
)
mkdir "%INSTALLPATH%"

REM 1. Créer le dossier build s'il n'existe pas
if not exist "%BUILDPATH%" (
    mkdir "%BUILDPATH%"
)

REM 2. Se déplacer dans le dossier build
cd "%BUILDPATH%"

REM 3. Exécuter qmake avec le chemin complet
echo ====== Exécution de qmake en mode %BUILDMODE% ======
"%QTPATH%\bin\qmake.exe" "%PROJECTPATH%\MarleneTranslation.pro" -spec win32-g++ "CONFIG+=%BUILDMODE%"
if errorlevel 1 (
    echo Erreur lors de l'exécution de qmake.
    pause
    exit /b 1
)

REM 4. Compiler le projet avec mingw32-make dans le chemin complet
echo ====== Compilation en cours ======
"%COMPILERPATH%\mingw32-make.exe"
if errorlevel 1 (
    echo Erreur lors de la compilation.
    pause
    exit /b 1
)

REM 5. Copier l'exécutable dans installMarlene si la compilation a réussi
echo ====== Copie de l'exécutable dans le dossier installMarlene ======
copy "%BUILDPATH%\%BUILDMODE%\MarleneTranslation.exe" "%INSTALLPATH%\MarleneTranslation.exe"
if errorlevel 1 (
    echo Erreur lors de la copie de l'exécutable.
    pause
    exit /b 1
)

REM 6. Exécuter windeployqt avec le chemin complet pour ajouter les dépendances
echo ====== Déploiement des dépendances avec windeployqt ======
"%QTPATH%\bin\windeployqt.exe" --dir "%INSTALLPATH%" "%INSTALLPATH%\MarleneTranslation.exe"
if errorlevel 1 (
    echo Erreur lors de l'exécution de windeployqt.
    pause
    exit /b 1
)

REM 7. Exécuter le script Inno Setup pour créer l'installateur avec le chemin complet
echo ====== Création de l'installateur avec Inno Setup ======
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" "%PROJECTPATH%\MarleneInstaller.iss"
if errorlevel 1 (
    echo Erreur lors de la création de l'installateur.
    pause
    exit /b 1
)

echo ====== Processus terminé avec succès ======
pause
exit /b 0
