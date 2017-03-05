echo off
rem Distutils for Python2.5 supports only Visual Studio 2003, using it with
rem Visual Studio 2005 will exit with an error. I made some changes to 
rem distutils file 'msvccompiler.py' to allow building the libRASCH extension.
rem Up to now I have found no problems using the libRASCH for Python interface.

rem If you want to build the extension and the installation file, copy the
rem changed 'msvccompiler.py' into your Python installation (of course after
rem you made a backup of the original file). The program building the installation
rem file is only available for Visual studio 2003 or below but it looks like that
rem the program for the 2003 version will work for Visual Studio 2005 also. 
rem Therefore duplicate the file 'wininst-7.1.exe' in the command directory in the 
rem distutils directory and rename it to 'wininst-8.0.exe'. For building the
rem extension, please run this batch-file (or the command below) in the 
rem 'Visual Studio 2005 Command Prompt', so the environment settings are correct.

rem Please let me know if it is working for you or not. Thanks.
rem Raphael Schneider <librasch@gmail.com>
echo on

python setup.py build

rem now we have to add the manifest for correct usage of the C-lib
mt /manifest build\lib.win32-2.5\RASCH_if.pyd.manifest /outputresource:build\lib.win32-2.5\RASCH_if.pyd;#2

rem and now create the installation file
python setup.py bdist --format=wininst