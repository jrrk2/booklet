/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.16
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
//#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionSaveAs;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *controlsLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLabel *descriptionLabel;
    QGridLayout *gridLayout;
    QLabel *inputLabel;
    QLineEdit *inputPathEdit;
    QPushButton *browseButton;
    QPushButton *previewButton;
    QLabel *outputLabel;
    QLineEdit *outputPathEdit;
    QPushButton *outputBrowseButton;
    QCheckBox *startFromBeginningCheckBox;
    QLabel *instructionsLabel;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *createButtonLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *createBookletButton;
    QSpacerItem *horizontalSpacer_2;
    QFrame *previewFrame;
    QVBoxLayout *previewLayout;
    QLabel *previewTitle;
    QLabel *previewLabel;
    QLabel *previewImage;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(640, 480);
        MainWindow->setMinimumSize(QSize(640, 480));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionSaveAs = new QAction(MainWindow);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        controlsLayout = new QVBoxLayout(layoutWidget);
        controlsLayout->setSpacing(6);
        controlsLayout->setContentsMargins(11, 11, 11, 11);
        controlsLayout->setObjectName(QString::fromUtf8("controlsLayout"));
        controlsLayout->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        descriptionLabel = new QLabel(groupBox);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));
        descriptionLabel->setWordWrap(true);

        verticalLayout_2->addWidget(descriptionLabel);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        inputLabel = new QLabel(groupBox);
        inputLabel->setObjectName(QString::fromUtf8("inputLabel"));

        gridLayout->addWidget(inputLabel, 0, 0, 1, 1);

        inputPathEdit = new QLineEdit(groupBox);
        inputPathEdit->setObjectName(QString::fromUtf8("inputPathEdit"));
        inputPathEdit->setReadOnly(true);

        gridLayout->addWidget(inputPathEdit, 0, 1, 1, 1);

        browseButton = new QPushButton(groupBox);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        gridLayout->addWidget(browseButton, 0, 2, 1, 1);

        previewButton = new QPushButton(groupBox);
        previewButton->setObjectName(QString::fromUtf8("previewButton"));
        previewButton->setEnabled(false);

        gridLayout->addWidget(previewButton, 0, 3, 1, 1);

        outputLabel = new QLabel(groupBox);
        outputLabel->setObjectName(QString::fromUtf8("outputLabel"));

        gridLayout->addWidget(outputLabel, 1, 0, 1, 1);

        outputPathEdit = new QLineEdit(groupBox);
        outputPathEdit->setObjectName(QString::fromUtf8("outputPathEdit"));

        gridLayout->addWidget(outputPathEdit, 1, 1, 1, 1);

        outputBrowseButton = new QPushButton(groupBox);
        outputBrowseButton->setObjectName(QString::fromUtf8("outputBrowseButton"));

        gridLayout->addWidget(outputBrowseButton, 1, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        startFromBeginningCheckBox = new QCheckBox(groupBox);
        startFromBeginningCheckBox->setObjectName(QString::fromUtf8("startFromBeginningCheckBox"));
        startFromBeginningCheckBox->setChecked(true);

        verticalLayout_2->addWidget(startFromBeginningCheckBox);

        instructionsLabel = new QLabel(groupBox);
        instructionsLabel->setObjectName(QString::fromUtf8("instructionsLabel"));
        instructionsLabel->setWordWrap(true);

        verticalLayout_2->addWidget(instructionsLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        createButtonLayout = new QHBoxLayout();
        createButtonLayout->setSpacing(6);
        createButtonLayout->setObjectName(QString::fromUtf8("createButtonLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        createButtonLayout->addItem(horizontalSpacer);

        createBookletButton = new QPushButton(groupBox);
        createBookletButton->setObjectName(QString::fromUtf8("createBookletButton"));
        createBookletButton->setEnabled(false);
        createBookletButton->setMinimumSize(QSize(150, 40));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(QFont::Normal);
        createBookletButton->setFont(font);

        createButtonLayout->addWidget(createBookletButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        createButtonLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(createButtonLayout);


        controlsLayout->addWidget(groupBox);

        splitter->addWidget(layoutWidget);
        previewFrame = new QFrame(splitter);
        previewFrame->setObjectName(QString::fromUtf8("previewFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(previewFrame->sizePolicy().hasHeightForWidth());
        previewFrame->setSizePolicy(sizePolicy);
        previewFrame->setFrameShape(QFrame::StyledPanel);
        previewFrame->setFrameShadow(QFrame::Raised);
        previewLayout = new QVBoxLayout(previewFrame);
        previewLayout->setSpacing(6);
        previewLayout->setContentsMargins(11, 11, 11, 11);
        previewLayout->setObjectName(QString::fromUtf8("previewLayout"));
        previewTitle = new QLabel(previewFrame);
        previewTitle->setObjectName(QString::fromUtf8("previewTitle"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(QFont::Normal);
        previewTitle->setFont(font1);
        previewTitle->setAlignment(Qt::AlignCenter);

        previewLayout->addWidget(previewTitle);

        previewLabel = new QLabel(previewFrame);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        previewLabel->setAlignment(Qt::AlignCenter);

        previewLayout->addWidget(previewLabel);

        previewImage = new QLabel(previewFrame);
        previewImage->setObjectName(QString::fromUtf8("previewImage"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(previewImage->sizePolicy().hasHeightForWidth());
        previewImage->setSizePolicy(sizePolicy1);
        previewImage->setMinimumSize(QSize(200, 300));
        previewImage->setAlignment(Qt::AlignCenter);

        previewLayout->addWidget(previewImage);

        splitter->addWidget(previewFrame);

        horizontalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 640, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSaveAs);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "A6 to A4 Booklet Maker", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open PDF...", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveAs->setText(QCoreApplication::translate("MainWindow", "Save Booklet As...", nullptr));
#if QT_CONFIG(shortcut)
        actionSaveAs->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "A6 to A4 Booklet Maker", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("MainWindow", "This tool arranges 4 A6 pages into a booklet format with two copies on a double-sided A4 sheet.", nullptr));
        inputLabel->setText(QCoreApplication::translate("MainWindow", "Input PDF:", nullptr));
        inputPathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Select input PDF file...", nullptr));
        browseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        previewButton->setText(QCoreApplication::translate("MainWindow", "Preview", nullptr));
        outputLabel->setText(QCoreApplication::translate("MainWindow", "Output PDF:", nullptr));
        outputPathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Select output location...", nullptr));
        outputBrowseButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        startFromBeginningCheckBox->setText(QCoreApplication::translate("MainWindow", "Start from beginning (first page as cover)", nullptr));
        instructionsLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-weight:600;\">Instructions:</span></p><ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select your PDF with A6-sized pages</li><li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Choose your output location</li><li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Click &quot;Create Booklet&quot;</li><li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Print the output PDF double-sided (flip on short edge)</li><li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Fold each sheet to create your booklet</li></ol></body></"
                        "html>", nullptr));
        createBookletButton->setText(QCoreApplication::translate("MainWindow", "Create Booklet", nullptr));
        previewTitle->setText(QCoreApplication::translate("MainWindow", "Preview", nullptr));
        previewLabel->setText(QCoreApplication::translate("MainWindow", "The preview will show how your pages will be arranged.", nullptr));
        previewImage->setText(QCoreApplication::translate("MainWindow", "A6 Booklet Preview", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
