@Echo Off
echo Removing old fonts...
del /F ..\..\..\fonts\*.dds
del /F ..\..\..\fonts\*.fnt
echo Setting default fonts to "Latin_Only"...
copy *.fnt ..\..\..\fonts /Y 
copy *.dds ..\..\..\fonts /Y
echo Done.