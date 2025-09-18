#include "pdfbookletcreator.h"
#include <QDebug>
#include <QProcess>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>

QPDFBookletCreator::QPDFBookletCreator(QObject *parent) : QObject(parent)
{
}

QPDFBookletCreator::~QPDFBookletCreator()
{
}

bool QPDFBookletCreator::createBooklet(const QString &inputPath, const QString &outputPath, bool startFromBeginning)
{
    try {
        return arrangePages(inputPath, outputPath, startFromBeginning);
    } catch (std::exception &e) {
        emit processingComplete(false, QString("Error: %1").arg(e.what()));
        return false;
    } catch (...) {
        emit processingComplete(false, "Unknown error occurred");
        return false;
    }
}

bool QPDFBookletCreator::arrangePages(const QString &inputPath, const QString &outputPath, bool startFromBeginning)
{
    // Instead of using direct QPDF C++ API, we'll use the command-line tool that's already installed
    // This approach avoids potential API compatibility issues
    
    // Create a temporary directory for working files
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        emit processingComplete(false, "Could not create temporary directory");
        return false;
    }
    
    // Get PDF page count using qpdf
    QProcess process;
    process.start(PathConfig::qpdfPath, QStringList() << "--show-npages" << inputPath);
    if (!process.waitForFinished()) {
        emit processingComplete(false, "Failed to get page count: " + process.errorString());
        return false;
    }
    
    bool ok;
    int pageCount = process.readAllStandardOutput().trimmed().toInt(&ok);
    if (!ok || pageCount <= 0) {
        emit processingComplete(false, "Invalid page count");
        return false;
    }
    
    qDebug() << "PDF has" << pageCount << "pages";
    
    // Calculate pages needed for the booklet
    int sheetsNeeded = (pageCount + 3) / 4; // Round up division
    int totalPages = sheetsNeeded * 4;
    
    // If we need blank pages, create a PDF with blank pages
    QString paddedPdfPath = inputPath;
    if (pageCount < totalPages) {
        int blankPagesNeeded = totalPages - pageCount;
        QString blankPdf = tempDir.filePath("blank.pdf");
        
        // Create a blank PDF using qpdf
        QProcess blankProcess;
        blankProcess.start(PathConfig::qpdfPath, QStringList() << "--empty" << "--pages" << "." << QString::number(blankPagesNeeded) << "--" << blankPdf);
        if (!blankProcess.waitForFinished()) {
            emit processingComplete(false, "Failed to create blank pages: " + blankProcess.errorString());
            return false;
        }
        
        // Concatenate original PDF with blank pages
        paddedPdfPath = tempDir.filePath("padded.pdf");
        QProcess catProcess;
        catProcess.start(PathConfig::qpdfPath, QStringList() << "--empty" << "--pages" << inputPath << "1-z" << blankPdf << "1-z" << "--" << paddedPdfPath);
        if (!catProcess.waitForFinished()) {
            emit processingComplete(false, "Failed to add blank pages: " + catProcess.errorString());
            return false;
        }
        
        pageCount = totalPages;
    }
    
    // Calculate page order for booklet
    QList<int> pageOrder;
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
    
    // Create the reordered PDF with booklet page layout
    return createCombinedPage(paddedPdfPath, outputPath, pageOrder);
}

bool QPDFBookletCreator::createCombinedPage(const QString &inputPath, const QString &outputPath, 
                                        const QList<int> &pageOrder)
{
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        emit processingComplete(false, "Could not create temporary directory");
        return false;
    }
    
    // First, extract the pages in the right order
    QString reorderedPdf = tempDir.filePath("reordered.pdf");
    
    QStringList pageArgs;
    pageArgs << "--empty" << "--pages";
    for (int i = 0; i < pageOrder.size(); ++i) {
        pageArgs << inputPath << QString::number(pageOrder.at(i));
    }
    pageArgs << "--" << reorderedPdf;
    
    QProcess reorderProcess;
    reorderProcess.start(PathConfig::qpdfPath, pageArgs);
    if (!reorderProcess.waitForFinished()) {
        emit processingComplete(false, "Failed to reorder pages: " + reorderProcess.errorString());
        return false;
    }
    
    // Now process the reordered PDF into 2-up pages
    QProcess nupProcess;
    QStringList nupArgs;
    nupArgs << reorderedPdf << "--pages-per-sheet" << "2" << "--output" << outputPath;
    
    nupProcess.start(PathConfig::pdfjamPath, nupArgs);
    if (!nupProcess.waitForFinished()) {
        emit processingComplete(false, "Failed to create 2-up layout: " + nupProcess.errorString() + 
                              "\nMake sure pdfjam is installed (brew install texlive-core)");
        return false;
    }
    
    emit processingComplete(true, "Booklet created successfully!");
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
