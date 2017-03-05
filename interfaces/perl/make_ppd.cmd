cd blib\arch\auto\RASCH
mt.exe -outputresource:RASCH.dll;#2 -manifest RASCH.dll.manifest
cd ..\..\..\..
tar cvf RASCH.tar blib
gzip --best RASCH.tar
nmake ppd
del RASCH.ppd.orig
rename RASCH.ppd RASCH.ppd.orig
perl correct_ppd.pl RASCH.ppd.orig > RASCH.ppd
