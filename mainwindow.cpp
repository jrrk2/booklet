#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QStandardPaths>
#include <QDialog>
#include <QVBoxLayout>
#include <QProcess>

QString PathConfig::qpdfPath("/opt/homebrew/bin/qpdf");
QString PathConfig::pdfjamPath("/opt/homebrew/bin/pdfjam");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bookletCreator(new QPDFBookletCreator(this)),
    previewWidget(nullptr)
{
    ui->setupUi(this);
    
    // Set window title
    setWindowTitle("A6 to A4 Booklet Maker");
    
    // Check if required dependencies are installed
    if (!checkDependencies()) {
        QMessageBox::warning(this, "Missing Dependencies",
                            "Some required dependencies are missing. The application may not work correctly.");
    }
    
    // Initialize UI
    updateUI();
    
    // Connect signals/slots for the booklet creator
    connect(bookletCreator, &QPDFBookletCreator::progressChanged, 
            [this](int progress) {
                // Update progress dialog if needed
            });
    
    connect(bookletCreator, &QPDFBookletCreator::processingComplete, 
            [this](bool success, const QString &message) {
                if (success) {
                    QMessageBox::information(this, "Success", 
                                           "Booklet created successfully!\n\n" + message);
                } else {
                    showError("Failed to create booklet: " + message);
                }
            });
    
    // Default output directory is Desktop
    QString defaultOutputDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    ui->outputPathEdit->setText(defaultOutputDir + "/booklet.pdf");
    outputFilePath = ui->outputPathEdit->text();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::checkDependencies() 
{
    bool allDepsPresent = true;
    QString missingDeps;
    
    allDepsPresent = PathConfig::checkDependencies(missingDeps);
    
    if (!allDepsPresent) {
        QMessageBox::warning(this, "Missing Dependencies",
                            "The following dependencies are missing:\n" + missingDeps + 
                            "\nPlease install them using Homebrew:\n" +
                            "brew install qpdf\n" +
                            "brew install texlive-core");
    }
    
    return allDepsPresent;
}

void MainWindow::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
                                                   "Select PDF with A6 pages", 
                                                   QDir::homePath(),
                                                   "PDF Files (*.pdf)");
    
    if (!filePath.isEmpty()) {
        inputFilePath = filePath;
        ui->inputPathEdit->setText(filePath);
        
        // Auto-generate output path
        if (outputFilePath.isEmpty()) {
            QFileInfo fileInfo(filePath);
            QString outPath = fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_booklet.pdf";
            ui->outputPathEdit->setText(outPath);
            outputFilePath = outPath;
        }
        
        updateUI();
    }
}

void MainWindow::on_outputBrowseButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, 
                                                  "Save Booklet PDF", 
                                                  QDir::homePath(),
                                                  "PDF Files (*.pdf)");
    
    if (!filePath.isEmpty()) {
        if (!filePath.toLower().endsWith(".pdf")) {
            filePath += ".pdf";
        }
        outputFilePath = filePath;
        ui->outputPathEdit->setText(filePath);
        updateUI();
    }
}

void MainWindow::on_createBookletButton_clicked()
{
    if (inputFilePath.isEmpty() || outputFilePath.isEmpty()) {
        showError("Please select input and output file paths.");
        return;
    }
    
    // Create progress dialog
    QProgressDialog progressDialog("Creating booklet...", "Cancel", 0, 100, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();
    
    // Connect progress updates
    connect(bookletCreator, &QPDFBookletCreator::progressChanged,
            &progressDialog, &QProgressDialog::setValue);
    
    // Create the booklet
    bool startFromBeginning = ui->startFromBeginningCheckBox->isChecked();
    bookletCreator->createBooklet(inputFilePath, outputFilePath, startFromBeginning);
}

void MainWindow::on_previewButton_clicked()
{
    if (inputFilePath.isEmpty()) {
        showError("Please select a PDF file first.");
        return;
    }
    
    // Create a dialog for the preview
    QDialog previewDialog(this);
    previewDialog.setWindowTitle("PDF Preview");
    previewDialog.resize(800, 600);
    
    // Create a layout for the dialog
    QVBoxLayout *layout = new QVBoxLayout(&previewDialog);
    
    // Create the preview widget if it doesn't exist
    if (!previewWidget) {
        previewWidget = new PDFPreviewWidget(&previewDialog);
    }
    
    // Add the preview widget to the layout
    layout->addWidget(previewWidget);
    
    // Load the PDF
    if (!previewWidget->loadPDF(inputFilePath)) {
        showError("Failed to load PDF for preview.");
        return;
    }
    
    // Show the dialog
    previewDialog.exec();
}

void MainWindow::on_actionOpen_triggered()
{
    on_browseButton_clicked();
}

void MainWindow::on_actionSaveAs_triggered()
{
    on_outputBrowseButton_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About A6 to A4 Booklet Maker",
                      "A6 to A4 Booklet Maker v1.0\n\n"
                      "A simple tool for creating booklets from A6 pages.\n\n"
                      "This application arranges A6 pages into a booklet format "
                      "that can be printed double-sided on A4 paper and folded "
                      "to create a booklet.");
}

void MainWindow::updateUI()
{
    bool hasInputFile = !inputFilePath.isEmpty();
    bool hasOutputFile = !outputFilePath.isEmpty();
    
    ui->createBookletButton->setEnabled(hasInputFile && hasOutputFile);
    ui->previewButton->setEnabled(hasInputFile);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Error", message);
}
