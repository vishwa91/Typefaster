INC = /usr/include/qt3
LIB = -lqt-mt -lpthread
MOC = moc
CXX=  g++
CXXFLAGS=-I$(INC)
LDFLAGS=-Wl,-rpath,/usr/local/qt3/lib -L/usr/lib/qt3

typefaster: BackgroundWidget.o ButtonLine.o KeyBoardWidget.o MyButton.o MyLayout.o MyParser.o TextWidget.o main.o moc_KeyBoardWidget.o moc_TextWidget.o moc_MyMainWindow.o  moc_MyHelpDialog.o MyMainWindow.o MyHelpDialog.o ChartWidget.o moc_ChartWidget.o ClassChart.o moc_ClassChart.o ClassDelChart.o moc_ClassDelChart.o ClassDialog.o moc_ClassDialog.o MyDialog.o moc_MyDialog.o MyLoginDialog.o moc_MyLoginDialog.o MyScoreDialog.o NewUser.o moc_NewUser.o numandtime.o SettingsDialog.o moc_SettingsDialog.o studentstats.o moc_studentstats.o UserDetails.o wordsforgen.o md5.o moc_MyScoreDialog.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIB)

moc_KeyBoardWidget.cpp : KeyBoardWidget.h
	$(MOC) KeyBoardWidget.h > moc_KeyBoardWidget.cpp
moc_TextWidget.cpp : TextWidget.h
	$(MOC) TextWidget.h > moc_TextWidget.cpp
moc_MyMainWindow.cpp : MyMainWindow.h
	$(MOC) MyMainWindow.h > moc_MyMainWindow.cpp
moc_MyDialog.cpp : MyDialog.h
	$(MOC) MyDialog.h > moc_MyDialog.cpp
moc_MyHelpDialog.cpp : MyHelpDialog.h
	$(MOC) MyHelpDialog.h > moc_MyHelpDialog.cpp
moc_ChartWidget.cpp : ChartWidget.h
	$(MOC) ChartWidget.h > moc_ChartWidget.cpp
moc_ClassChart.cpp : ClassChart.h
	$(MOC) ClassChart.h > moc_ClassChart.cpp
moc_ClassDelChart.cpp : ClassDelChart.h
	$(MOC) ClassDelChart.h > moc_ClassDelChart.cpp
moc_ClassDialog.cpp : ClassDialog.h
	$(MOC) ClassDialog.h > moc_ClassDialog.cpp
moc_MyLoginDialog.cpp : MyLoginDialog.h
	$(MOC) MyLoginDialog.h > moc_MyLoginDialog.cpp
moc_NewUser.cpp : NewUser.h
	$(MOC) NewUser.h > moc_NewUser.cpp
moc_SettingsDialog.cpp : SettingsDialog.h
	$(MOC) SettingsDialog.h > moc_SettingsDialog.cpp
moc_studentstats.cpp : studentstats.h
	$(MOC) studentstats.h > moc_studentstats.cpp
moc_MyScoreDialog.cpp: MyScoreDialog.h
	$(MOC) MyScoreDialog.h > moc_MyScoreDialog.cpp

main.o: MyMainWindow.h
MyMainWindow.o: MyMainWindow.h KeyBoardWidget.h TextWidget.h BackgroundWidget.h
BackgroundWidget.o: BackgroundWidget.h
ButtonLine.o: ButtonLine.h
KeyBoardWidget.o: KeyBoardWidget.h
MyButton.o: MyButton.h ButtonLine.h
MyLayout.o: MyLayout.h
MyParser.o: MyParser.h MyButton.h
TextWidget.o: TextWidget.h
MyDialog.o: MyDialog.h
MyHelpDialog.o: MyHelpDialog.h
MyLoginDialog.o: MyLoginDialog.h NewUser.h UserDetails.h
MyScoreDialog.o: MyScoreDialog.h ChartWidget.h
NewUser.o: NewUser.h UserDetails.h
numandtime.o: numandtime.h
SettingsDialog.o: SettingsDialog.h
studentstats.o: studentstats.h
UserDetails.o: UserDetails.h
wordsforgen.o: wordsforgen.h
ClassDialog.o: ClassDialog.h
ClassDelChart.o: ClassDelChart.h UserDetails.h
ClassChart.o: ClassChart.h studentstats.h
ChartWidget.o: ChartWidget.h numandtime.h
moc_KeyBoardWidget.o: KeyBoardWidget.h
moc_TextWidget.o: TextWidget.h
moc_MyMainWindow.o: MyMainWindow.h
moc_MyDialog.o: MyDialog.h
moc_MyHelpDialog.o: MyHelpDialog.h
moc_ChartWidget.o: ChartWidget.h 
moc_ClassChart.o: ClassChart.h 
moc_ClassDelChart.o: ClassDelChart.h 
moc_ClassDialog.o: ClassDialog.h 
moc_MyLoginDialog.o: MyLoginDialog.h 
moc_NewUser.o: NewUser.h 
moc_SettingsDialog.o: SettingsDialog.h 
moc_studentstats.o: studentstats.h
moc_MyScoreDialog.o: MyScoreDialog.h


clean:
	rm -f *.o typefaster moc_KeyBoardWidget.cpp moc_TextWidget.cpp moc_MyMainWindow.cpp moc_MyDialog.cpp moc_MyHelpDialog.cpp moc_ChartWidget.cpp moc_ClassChart.cpp moc_ClassDelChart.cpp moc_ClassDialog.cpp moc_MyLoginDialog.cpp moc_NewUser.cpp moc_SettingsDialog.cpp moc_studentstats.cpp moc_MyScoreDialog.cpp
cleanmoc:
	rm -f moc_KeyBoardWidget.cpp moc_TextWidget.cpp moc_MyMainWindow.cpp moc_MyDialog.cpp moc_MyHelpDialog.cpp moc_ChartWidget.cpp moc_ClassChart.cpp moc_ClassDelChart.cpp moc_ClassDialog.cpp moc_MyLoginDialog.cpp moc_NewUser.cpp moc_SettingsDialog.cpp moc_studentstats.cpp moc_MyScoreDialog.cpp
