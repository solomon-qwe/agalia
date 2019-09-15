if (Test-Path docs\agalia_X.X.X_x64.zip) {
	Remove-Item -Path docs\agalia_X.X.X_x64.zip
}
if (Test-Path docs\agalia_X.X.X_x86.zip) {
	Remove-Item -Path docs\agalia_X.X.X_x86.zip
}

Compress-Archive -Path README.md, LICENSE.txt, src\agaliarept\dcm_private.xml, src\agaliarept\dcm_standard.xml, src\agaliarept\tif_standard.xml, x64\Release\agalia.com, x64\Release\agalia.exe, x64\Release\agaliarept.dll -DestinationPath docs\agalia_X.X.X_x64.zip -CompressionLevel Optimal
Compress-Archive -Path README.md, LICENSE.txt, src\agaliarept\dcm_private.xml, src\agaliarept\dcm_standard.xml, src\agaliarept\tif_standard.xml, x86\Release\agalia.com, x86\Release\agalia.exe, x86\Release\agaliarept.dll -DestinationPath docs\agalia_X.X.X_x86.zip -CompressionLevel Optimal

if (Test-Path setup\setup.x64\Release\agalia_setup_X.X.X_x64.msi) {
	Copy-Item -Path setup\setup.x64\Release\agalia_setup_X.X.X_x64.msi -Destination docs\
}
if (Test-Path setup\setup.x86\Release\agalia_setup_X.X.X_x86.msi) {
	Copy-Item -Path setup\setup.x86\Release\agalia_setup_X.X.X_x86.msi -Destination docs\
}
