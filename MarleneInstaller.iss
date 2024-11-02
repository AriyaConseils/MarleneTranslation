[Setup]
AppName=MarleneTranslation
AppVersion=1.0
SetupIconFile=ressources\MarleneTranslation.ico
DefaultDirName={userappdata}\MarleneTranslation
OutputDir=.
OutputBaseFilename=MarleneTranslationInstaller
DefaultGroupName=MarleneTranslation
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
UninstallDisplayIcon={app}\MarleneTranslation.exe

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Files]
Source: "installMarlene\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{userstartmenu}\MarleneTranslation"; Filename: "{app}\MarleneTranslation.exe"; IconFilename: "{app}\MarleneTranslation.ico"
Name: "{userdesktop}\MarleneTranslation"; Filename: "{app}\MarleneTranslation.exe"; IconFilename: "{app}\MarleneTranslation.ico"

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; \
    ValueData: "{app};{olddata}"

[Run]
Filename: "{app}\MarleneTranslation.exe"; Description: "Lancer MarleneTranslation"; Flags: nowait postinstall skipifsilent
; Force le rafraîchissement des variables d'environnement sans redémarrage
Filename: "powershell"; Parameters: "-Command [System.Environment]::SetEnvironmentVariable('Path', [System.Environment]::GetEnvironmentVariable('Path','User'),'User')"; Flags: runhidden
