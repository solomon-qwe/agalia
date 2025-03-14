if (Test-Path docs\agalia_X.X.X_x64.zip) {
	Remove-Item -Path docs\agalia_X.X.X_x64.zip
}

if (Test-Path docs\agalia_X.X.X_x86.zip) {
	Remove-Item -Path docs\agalia_X.X.X_x86.zip
}

Compress-Archive `
	-Path `
		README.md, LICENSE.txt, `
		src\agaliarept\dcm_private.xml, `
		src\agaliarept\dcm_standard.xml, `
		src\agaliarept\tif_standard.xml, `
		bin\x64\Release\agalia.com, `
		bin\x64\Release\agalia.exe, `
		bin\x64\Release\agaliarept.dll, `
		bin\x64\Release\agaliaUtil.dll, `
		bin\x64\Release\decode.dll, `
		bin\x64\Release\decode_MF.dll, `
		bin\x64\Release\decode_openjpeg.dll, `
		bin\x64\Release\decode_WIC.dll, `
		bin\x64\Release\openjp2.dll `
	-DestinationPath `
		docs\agalia_X.X.X_x64.zip `
	-CompressionLevel `
		Optimal

Compress-Archive `
	-Path `
		README.md, LICENSE.txt, `
		src\agaliarept\dcm_private.xml, `
		src\agaliarept\dcm_standard.xml, `
		src\agaliarept\tif_standard.xml, `
		bin\x86\Release\agalia.com, `
		bin\x86\Release\agalia.exe, `
		bin\x86\Release\agaliarept.dll, `
		bin\x86\Release\agaliaUtil.dll, `
		bin\x86\Release\decode.dll, `
		bin\x86\Release\decode_MF.dll, `
		bin\x86\Release\decode_openjpeg.dll, `
		bin\x86\Release\decode_WIC.dll, `
		bin\x86\Release\openjp2.dll `
	-DestinationPath `
		docs\agalia_X.X.X_x86.zip `
	-CompressionLevel `
		Optimal

if (Test-Path setup\setup.x64\Release\agalia_X.X.X_x64.msi) {
	Copy-Item `
		-Path `
			setup\setup.x64\Release\agalia_X.X.X_x64.msi `
		-Destination `
			docs\
}

if (Test-Path setup\setup.x86\Release\agalia_X.X.X_x86.msi) {
	Copy-Item `
		-Path `
			setup\setup.x86\Release\agalia_X.X.X_x86.msi `
		-Destination `
			docs\
}
