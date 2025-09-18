default: booklet

./Release/Booklet.app: booklet
	xcodebuild -project Booklet.xcodeproj -scheme Booklet -configuration Release

booklet: A6BookletMaker.pro
	for i in moc_mainwindow.cpp moc_pdfbookletcreator.cpp moc_pdfpreviewwidget.cpp; do /opt/homebrew/Cellar/qt/6.9.0/share/qt/libexec/moc `echo $$i|sed -e 's=^moc_==' -e 's=.cpp=.h='` -o $$i; done
	qmake -spec macx-xcode $<

run: ./Release/Booklet.app
	./Release/Booklet.app/Contents/MacOS/Booklet
