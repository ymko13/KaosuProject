@Echo Off
echo Removing old fonts...
del /F ..\..\..\fonts\*.dds
del /F ..\..\..\fonts\*.fnt
echo Setting default fonts to "Hangul_Hiragana_Katakana_Latin"...
copy *.fnt ..\..\..\fonts /Y 
copy *.dds ..\..\..\fonts /Y
echo Done.