#define MyAppName "test"
#define MyAppVersion "1.0.3"
#define MyAppPublisher "Ali YILDIRIM"
#define MyAppURL "https://github.com/aliyldm/BarkodTakip"
#define MyAppExeName "barcode_tracker.exe"
#define MyAppSourcePath "build\Desktop_Qt_6_8_1_MinGW_64_bit-Release"

[Setup]
AppId={{B2A5F2D3-8C09-4E85-9D65-8C1B7D44A3F2}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=LICENSE
OutputDir=installer
OutputBaseFilename=BarkodTakip_Setup_v{#MyAppVersion}
SetupIconFile=resources\icons\app.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Ana uygulama
Source: "{#MyAppSourcePath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; DLL'ler ve diğer bağımlılıklar
Source: "{#MyAppSourcePath}\windeployqt\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppSourcePath}\windeployqt\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs
Source: "{#MyAppSourcePath}\windeployqt\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs
Source: "{#MyAppSourcePath}\windeployqt\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs

; Uygulama kaynakları
Source: "{#MyAppSourcePath}\windeployqt\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

; İkon dosyası
Source: "resources\icons\app.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\app.ico"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\app.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent 