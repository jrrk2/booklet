#include "pdfbookletcreator.h"
#include <QDebug>
#include <QProcess>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QStandardPaths>

QPDFBookletCreator::QPDFBookletCreator(QObject *parent) : QObject(parent)
{
}

QPDFBookletCreator::~QPDFBookletCreator()
{
}

bool QPDFBookletCreator::createBooklet(const QString &inputPath, const QString &outputPath, bool startFromBeginning)
{
    qDebug() << "=== Starting booklet creation ===";
    qDebug() << "Input path:" << inputPath;
    qDebug() << "Output path:" << outputPath;
    qDebug() << "Start from beginning:" << startFromBeginning;
    
    // Check if input file exists
    QFileInfo inputInfo(inputPath);
    if (!inputInfo.exists()) {
        QString error = QString("Input file does not exist: %1").arg(inputPath);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Check if output directory exists and is writable
    QFileInfo outputInfo(outputPath);
    QDir outputDir = outputInfo.absoluteDir();
    if (!outputDir.exists()) {
        qDebug() << "Output directory does not exist:" << outputDir.absolutePath();
        if (!outputDir.mkpath(".")) {
            QString error = QString("Cannot create output directory: %1").arg(outputDir.absolutePath());
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        qDebug() << "Created output directory:" << outputDir.absolutePath();
    }
    
    try {
        return arrangePages(inputPath, outputPath, startFromBeginning);
    } catch (std::exception &e) {
        QString error = QString("Exception: %1").arg(e.what());
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    } catch (...) {
        qDebug() << "Unknown exception occurred";
        emit processingComplete(false, "Unknown error occurred");
        return false;
    }
}

void QPDFBookletCreator::debugProcess(QProcess &process, const QString &command, const QStringList &args)
{
    qDebug() << "--- Process Debug Info ---";
    qDebug() << "Command:" << command;
    qDebug() << "Arguments:" << args.join(" ");
    qDebug() << "Working directory:" << process.workingDirectory();
    qDebug() << "Process state:" << process.state();
    qDebug() << "Exit code:" << process.exitCode();
    qDebug() << "Exit status:" << process.exitStatus();
    
    QString stdout = process.readAllStandardOutput();
    QString stderr = process.readAllStandardError();
    
    if (!stdout.isEmpty()) {
        qDebug() << "STDOUT:" << stdout;
    }
    if (!stderr.isEmpty()) {
        qDebug() << "STDERR:" << stderr;
    }
    qDebug() << "--- End Process Debug ---";
}

bool QPDFBookletCreator::arrangePages(const QString &inputPath, const QString &outputPath, bool startFromBeginning)
{
    qDebug() << "=== Arranging pages ===";
    
    // Check if required tools exist
    qDebug() << "Checking for required tools:";
    qDebug() << "qpdf path:" << PathConfig::qpdfPath;
    qDebug() << "pdfjam path:" << PathConfig::pdfjamPath;
    
    // Test if qpdf exists and is executable
    QFileInfo qpdfInfo(PathConfig::qpdfPath);
    if (!qpdfInfo.exists() || !qpdfInfo.isExecutable()) {
        QString error = QString("qpdf not found or not executable at: %1").arg(PathConfig::qpdfPath);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Test if pdfjam exists and is executable
    QFileInfo pdfjamInfo(PathConfig::pdfjamPath);
    if (!pdfjamInfo.exists() || !pdfjamInfo.isExecutable()) {
        QString error = QString("pdfjam not found or not executable at: %1").arg(PathConfig::pdfjamPath);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Create a temporary directory for working files
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QString error = "Could not create temporary directory";
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "Temporary directory:" << tempDir.path();
    
    // Get PDF page count using qpdf
    qDebug() << "Getting page count...";
    QProcess process;
    QStringList pageCountArgs;
    pageCountArgs << "--show-npages" << inputPath;
    
    qDebug() << "Running qpdf to get page count...";
    process.start(PathConfig::qpdfPath, pageCountArgs);
    
    if (!process.waitForFinished(30000)) { // 30 second timeout
        debugProcess(process, PathConfig::qpdfPath, pageCountArgs);
        QString error = "Failed to get page count (timeout or process error): " + process.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    if (process.exitCode() != 0) {
        debugProcess(process, PathConfig::qpdfPath, pageCountArgs);
        QString error = QString("qpdf failed with exit code %1").arg(process.exitCode());
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    QString pageCountOutput = process.readAllStandardOutput().trimmed();
    qDebug() << "Page count output:" << pageCountOutput;
    
    // Debug the process after we've captured the output
    qDebug() << "--- Process Debug Info ---";
    qDebug() << "Command:" << PathConfig::qpdfPath;
    qDebug() << "Arguments:" << pageCountArgs.join(" ");
    qDebug() << "Exit code:" << process.exitCode();
    qDebug() << "Exit status:" << process.exitStatus();
    qDebug() << "--- End Process Debug ---";
    
    bool ok;
    int pageCount = pageCountOutput.toInt(&ok);
    if (!ok || pageCount <= 0) {
        QString error = QString("Invalid page count: '%1'").arg(pageCountOutput);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "PDF has" << pageCount << "pages";
    
    // Calculate pages needed for the booklet
    int sheetsNeeded = (pageCount + 3) / 4; // Round up division
    int totalPages = sheetsNeeded * 4;
    
    qDebug() << "Sheets needed:" << sheetsNeeded;
    qDebug() << "Total pages needed:" << totalPages;
    
    // If we need blank pages, create a PDF with blank pages
    QString paddedPdfPath = inputPath;
    if (pageCount < totalPages) {
        int blankPagesNeeded = totalPages - pageCount;
        qDebug() << "Need to add" << blankPagesNeeded << "blank pages";
        
        QString blankPdf = tempDir.filePath("blank.pdf");
        qDebug() << "Creating blank PDF at:" << blankPdf;
        
        // Create a blank PDF using qpdf
        QProcess blankProcess;
        QStringList blankArgs;
        blankArgs << "--empty" << "--pages" << "." << QString::number(blankPagesNeeded) << "--" << blankPdf;
        
        qDebug() << "Creating blank pages...";
        blankProcess.start(PathConfig::qpdfPath, blankArgs);
        if (!blankProcess.waitForFinished(30000)) {
            debugProcess(blankProcess, PathConfig::qpdfPath, blankArgs);
            QString error = "Failed to create blank pages: " + blankProcess.errorString();
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        
        debugProcess(blankProcess, PathConfig::qpdfPath, blankArgs);
        
        if (blankProcess.exitCode() != 0) {
            QString error = QString("Failed to create blank pages, exit code: %1").arg(blankProcess.exitCode());
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        
        // Check if blank PDF was created
        QFileInfo blankInfo(blankPdf);
        if (!blankInfo.exists()) {
            QString error = "Blank PDF was not created at expected location: " + blankPdf;
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        qDebug() << "Blank PDF created successfully, size:" << blankInfo.size() << "bytes";
        
        // Concatenate original PDF with blank pages
        paddedPdfPath = tempDir.filePath("padded.pdf");
        qDebug() << "Creating padded PDF at:" << paddedPdfPath;
        
        QProcess catProcess;
        QStringList catArgs;
        catArgs << "--empty" << "--pages" << inputPath << "1-z" << blankPdf << "1-z" << "--" << paddedPdfPath;
        
        qDebug() << "Concatenating PDFs...";
        catProcess.start(PathConfig::qpdfPath, catArgs);
        if (!catProcess.waitForFinished(30000)) {
            debugProcess(catProcess, PathConfig::qpdfPath, catArgs);
            QString error = "Failed to add blank pages: " + catProcess.errorString();
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        
        debugProcess(catProcess, PathConfig::qpdfPath, catArgs);
        
        if (catProcess.exitCode() != 0) {
            QString error = QString("Failed to concatenate PDFs, exit code: %1").arg(catProcess.exitCode());
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        
        // Check if padded PDF was created
        QFileInfo paddedInfo(paddedPdfPath);
        if (!paddedInfo.exists()) {
            QString error = "Padded PDF was not created at expected location: " + paddedPdfPath;
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
        qDebug() << "Padded PDF created successfully, size:" << paddedInfo.size() << "bytes";
        
        pageCount = totalPages;
    }
    
    // Calculate page order for booklet
    QList<int> pageOrder;
    qDebug() << "Calculating page order...";
    
    for (int sheet = 0; sheet < sheetsNeeded; sheet++) {
        if (startFromBeginning) {
            // Standard booklet ordering (first page is cover)
            pageOrder.append(totalPages - sheet * 2);
            pageOrder.append(sheet * 2 + 1);
            pageOrder.append(sheet * 2 + 2);
            pageOrder.append(totalPages - sheet * 2 - 1);
        } else {
            // Reverse ordering (last page is cover)
            pageOrder.append(sheet * 2 + 1);
            pageOrder.append(totalPages - sheet * 2);
            pageOrder.append(totalPages - sheet * 2 - 1);
            pageOrder.append(sheet * 2 + 2);
        }
    }
    
    qDebug() << "Page order:" << pageOrder;
    
    // Create the reordered PDF with booklet page layout
    return createCombinedPage(paddedPdfPath, outputPath, pageOrder);
}

bool QPDFBookletCreator::createCombinedPage(const QString &inputPath, const QString &outputPath,
                                        const QList<int> &pageOrder)
{
    qDebug() << "=== Creating combined pages ===";
    qDebug() << "Input path:" << inputPath;
    qDebug() << "Output path:" << outputPath;
    
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QString error = "Could not create temporary directory for combined pages";
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "Temp directory for combined pages:" << tempDir.path();
    
    // First, extract the pages in the right order into a single reordered PDF
    QString reorderedPdf = tempDir.filePath("reordered.pdf");
    qDebug() << "Reordered PDF path:" << reorderedPdf;
    
    QStringList pageArgs;
    pageArgs << "--empty" << "--pages";
    for (int i = 0; i < pageOrder.size(); ++i) {
        pageArgs << inputPath << QString::number(pageOrder.at(i));
    }
    pageArgs << "--" << reorderedPdf;
    
    qDebug() << "Reordering pages...";
    QProcess reorderProcess;
    reorderProcess.start(PathConfig::qpdfPath, pageArgs);
    if (!reorderProcess.waitForFinished(60000)) {
        QString error = "Failed to reorder pages: " + reorderProcess.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "--- Reorder Process Debug Info ---";
    qDebug() << "Command:" << PathConfig::qpdfPath;
    qDebug() << "Arguments:" << pageArgs.join(" ");
    qDebug() << "Exit code:" << reorderProcess.exitCode();
    
    QString stdout = reorderProcess.readAllStandardOutput();
    QString stderr = reorderProcess.readAllStandardError();
    if (!stdout.isEmpty()) qDebug() << "STDOUT:" << stdout;
    if (!stderr.isEmpty()) qDebug() << "STDERR:" << stderr;
    qDebug() << "--- End Reorder Process Debug ---";
    
    // qpdf exit codes: 0 = success, 3 = success with warnings, 2+ = error
    int exitCode = reorderProcess.exitCode();
    if (exitCode != 0 && exitCode != 3) {
        QString error = QString("Failed to reorder pages, exit code: %1").arg(exitCode);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    if (exitCode == 3) {
        qDebug() << "qpdf completed with warnings (exit code 3) - this is usually okay";
    }
    
    // Check if reordered PDF was created
    QFileInfo reorderedInfo(reorderedPdf);
    if (!reorderedInfo.exists()) {
        QString error = "Reordered PDF was not created at expected location: " + reorderedPdf;
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    qDebug() << "Reordered PDF created successfully, size:" << reorderedInfo.size() << "bytes";
    
    // For now, let's just copy the reordered PDF as the output
    // This will give us the correct page order, but not the 2-up layout
    // We can work on the 2-up layout separately once we confirm the ordering works
    qDebug() << "Copying reordered PDF to output (without 2-up layout for now)...";
    
    // Remove existing output file if it exists
    if (QFile::exists(outputPath)) {
        qDebug() << "Output file already exists, removing it...";
        if (!QFile::remove(outputPath)) {
            QString error = "Failed to remove existing output file: " + outputPath;
            qDebug() << error;
            emit processingComplete(false, error);
            return false;
        }
    }
    
    if (!QFile::copy(reorderedPdf, outputPath)) {
        QFileInfo reorderedFileInfo(reorderedPdf);
        QFileInfo outputFileInfo(outputPath);
        
        QString error = QString("Failed to copy reordered PDF to output location\n"
                               "Source: %1 (exists: %2, readable: %3, size: %4)\n"
                               "Dest: %5 (dir exists: %6, dir writable: %7)")
                               .arg(reorderedPdf)
                               .arg(reorderedFileInfo.exists())
                               .arg(reorderedFileInfo.isReadable())
                               .arg(reorderedFileInfo.size())
                               .arg(outputPath)
                               .arg(outputFileInfo.dir().exists())
                               .arg(outputFileInfo.dir().isReadable());
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Check if final output was created
    QFileInfo outputInfo(outputPath);
    if (!outputInfo.exists()) {
        QString error = "Output PDF was not created at expected location: " + outputPath;
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "Output PDF created successfully!";
    qDebug() << "Final output file:" << outputPath;
    qDebug() << "Output file size:" << outputInfo.size() << "bytes";
    qDebug() << "Output file permissions:" << outputInfo.permissions();
    
    emit processingComplete(true, "Booklet pages reordered successfully! (2-up layout not yet implemented)");
    return true;
}

QImage QPDFBookletCreator::renderPage(const QString &pdfPath, int pageNum)
{
    // This is a placeholder. In a real implementation, you would:
    // 1. Use a PDF rendering library like Poppler or MuPDF to render the page to an image
    // 2. Return that image for display
    
    // For now, just return a blank image of A6 size
    return QImage(static_cast<int>(A6_WIDTH), static_cast<int>(A6_HEIGHT), QImage::Format_RGB32);
}

void QPDFBookletCreator::logProgress(int current, int total)
{
    int progress = (current * 100) / total;
    emit progressChanged(progress);
}
