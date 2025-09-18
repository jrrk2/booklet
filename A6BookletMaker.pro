# A6BookletMaker.pro

QT       += core gui widgets

TARGET = A6BookletMaker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Silence SDK version warnings
CONFIG += sdk_no_version_check

# Include paths for macOS
macx {
    # QPDF include paths - adjust according to your installation
    INCLUDEPATH += /opt/homebrew/Cellar/qpdf/12.1.0/include
    
    # Add library paths
    LIBS += -L/opt/homebrew/lib -lqpdf
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    pdfbookletcreator.cpp \
    pdfpreviewwidget.cpp

HEADERS += \
    mainwindow.h \
    pdfbookletcreator.h \
    pdfpreviewwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Resources - commented out until icons are available
# RESOURCES += resources.qrc
