; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define source_path "R:/Documents/Programming/C++/SDBVS/" 

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{ADE00E96-A29D-4A20-A091-6AEE977FD138}
AppName=GeodeDB
AppVersion=1.0
;AppVerName=GeodeDB 1.0
AppPublisher=Rowan Preston
AppPublisherURL=https://github.com/r-preston/geodedb
AppSupportURL=https://github.com/r-preston/geodedb
AppUpdatesURL=https://github.com/r-preston/geodedb
DefaultDirName={userdocs}/GeodeDB
DisableProgramGroupPage=yes
OutputBaseFilename=geodedb_install
Compression=lzma
UsePreviousAppDir=no
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"   

[Tasks]
Name: "portable"; Description: "Create a portable installation (recommended). The program will store the database and all configuration files in its install directory instead of AppData. Do not use this if the install location is protected, for example C:/Program Files";
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
; portable lock file
Source: "{#source_path}portable"; DestDir: "{app}"; Flags: ignoreversion; Tasks: portable    
; main components           
Source: "{#source_path}geodedb.exe"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}awesomium_process.exe"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}avcodec-53.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}avformat-53.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}avutil-51.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}awesomium.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}icudt.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}libGLESv2.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}sqlite3.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}xinput9_1_0.dll"; DestDir: "{app}"; Flags: ignoreversion;
; UI components
Source: "{#source_path}ui/ui.html"; DestDir: "{app}/ui"; Flags: ignoreversion;   
Source: "{#source_path}ui/ui_src.html"; DestDir: "{app}/ui"; Flags: ignoreversion; 
Source: "{#source_path}ui/Generate ui.html from component files.exe"; DestDir: "{app}/ui"; Flags: ignoreversion;

Source: "{#source_path}ui/style/fileadding.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/inputs.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/itemedit.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/itemview.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/main.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/result-list.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 
Source: "{#source_path}ui/style/search-panel.css"; DestDir: "{app}/ui/style"; Flags: ignoreversion; 

Source: "{#source_path}ui/scripts/edit-panel.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/global.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/helpers.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/inputs.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/item-panel.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/list-sorting.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/objects.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/resize.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/results-panel.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/search-panel.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/setup.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 
Source: "{#source_path}ui/scripts/syncing.js"; DestDir: "{app}/ui/scripts"; Flags: ignoreversion; 

Source: "{#source_path}ui/resource/icon.ico"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_down.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_down_centered.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_dual.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_left.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_left_null.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_right.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_right_null.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_up.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/arrow_up_centered.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/placeholder.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/plus.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/x.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
Source: "{#source_path}ui/resource/x_over.png"; DestDir: "{app}/ui/resource"; Flags: ignoreversion; 
; misc components   
Source: "{#source_path}Minerals.xlsx"; DestDir: "{app}"; Flags: ignoreversion;
Source: "{#source_path}Collection.xlsx"; DestDir: "{app}"; Flags: ignoreversion; 
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\GeodeDB"; Filename: "{app}\geodedb.exe"
Name: "{commondesktop}\GeodeDB"; Filename: "{app}\geodedb.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\geodedb.exe"; Description: "{cm:LaunchProgram,GeodeDB}"; Flags: nowait postinstall skipifsilent

