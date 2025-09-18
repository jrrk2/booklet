#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include "pdfbookletcreator.h"
#include "pdfpreviewwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseButton_clicked();
    void on_createBookletButton_clicked();
    void on_outputBrowseButton_clicked();
    void on_previewButton_clicked();
    void on_actionOpen_triggered();
    void on_actionSaveAs_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QPDFBookletCreator *bookletCreator;
    PDFPreviewWidget *previewWidget;
    QString inputFilePath;
    QString outputFilePath;
    
    void updateUI();
    void showError(const QString &message);
    bool checkDependencies();
};

#endif // MAINWINDOW_H
