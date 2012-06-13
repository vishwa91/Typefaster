Project web site: http://typefaster.sourceforge.net

The program has been developed with C++ and QT.  

The program has been mainly developed on Windows with the 
use of Visual C++ 6, with "QtWin230-NonCommercial.exe" 
installed.  Although this QT install file no longer seems to be 
available on "http://www.trolltech.com", you may still be able to 
get it from their ftp site: "ftp.trolltech.com", probably in "qt/non-commercial" 
If you want to develop on Windows, just open TypeFaster.dsw 
in the source folder.

If you are interested in the Linux port, see README-Linux.txt

If you look at main.cpp, you will see that there is not much there.  
The program really starts to get going in MyMainWindow.cpp 
which then gets KeyBoardWidget.cpp and TextWidget.cpp going.

Another really useful way for you to help would be to contribute 
more keyboard layout xml files.  Take a look at the examples 
in the layouts directory.  You can also help by sending in lesson 
files.  Please make sure that you send the files as text files saved 
with Unicode encoding but preferably zip or gzip them also.
(Info-ZIP's Wiz is a free zip program).

If you are artistic you could design some 
better icons, or even make the keyboard buttons look better, they 
are drawn in the drawButton() method of MyButton.cpp.  

Send patches, layouts, lessons, artwork, ideas ... to "mataav@yahoo.co.uk"

If you are unhappy with the requirement that the teacher-class version needs to be installed into a writable, shared folder then let me know (mataav@yahoo.co.uk) and I will consider writing a network server process (which also means that it could work with many clients installed in different places). Or you could even do it yourself.

P.S. I would like the game to support more languages, so perhaps a font
renderer like: gltext.sourceforge.net or oglft.sourceforge.net 
could be used (this would also mean that glut would no longer be needed)
P.P.S Another game would be nice or just improve the current one.
