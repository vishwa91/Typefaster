The code should compile on Linux with the Makefile provided (assuming you have Qt3 installed).

There are several problems with the Linux port:

1. No game (but should be possible since it is just OpenGL)

2. The buttons on the keyboard stay down once pressed. This seems to be a bug in  Linux Qt3 in that the QKeyEvent in the keyReleaseEvent does not contain the QString of what key it was.

3. The bounding box of text as reported by Qt seems to differ from Windows so small things like "," look very bad on the drawn keyboard (if they can be seen at all).

4. The text does not fit in the button sizes and the buttons do not always fit on the dialogs. Can be fixed if some effort is put in.

If you improve the Linux port, send me the new source-code or just a patch. My email address is mataav@yahoo.co.uk