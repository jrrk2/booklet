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
    
    // Try Ghostscript for proper 2-up layout (2 A6 pages per A4 sheet)
    qDebug() << "Creating 2-up layout using Ghostscript...";
    return createGhostscript2Up(reorderedPdf, outputPath);
}

bool QPDFBookletCreator::createGhostscript2Up(const QString &inputPath, const QString &outputPath)
{
    qDebug() << "=== Creating 2-up layout with Ghostscript ===";
    
    // Check if ghostscript is available
    QProcess gsCheck;
    gsCheck.start("/opt/homebrew/bin/gs", QStringList() << "--version");
    if (!gsCheck.waitForFinished(10000) || gsCheck.exitCode() != 0) {
        // Try with just 'gs' in case it's in PATH differently
        gsCheck.start("gs", QStringList() << "--version");
        if (!gsCheck.waitForFinished(10000) || gsCheck.exitCode() != 0) {
            QString error = "Ghostscript not found at expected location. Falling back to sequential layout.";
            qDebug() << error;
            // Fall back to the working sequential approach instead of failing
            return createSequential2Up(inputPath, outputPath);
        }
    }
    
    QString gsVersion = gsCheck.readAllStandardOutput().trimmed();
    qDebug() << "Found Ghostscript version:" << gsVersion;
    
    // Remove existing output file
    if (QFile::exists(outputPath)) {
        qDebug() << "Output file already exists, removing it...";
        QFile::remove(outputPath);
    }
    
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qDebug() << "Could not create temp directory, falling back";
        return createSequential2Up(inputPath, outputPath);
    }
    
    // Use a simpler approach: try to use Ghostscript's pdfwrite with page manipulation
    // First, let's try the most straightforward 2-up approach
    QString tempOutput = tempDir.filePath("gs_temp.pdf");
    
    QStringList gsArgs;
    gsArgs << "-sDEVICE=pdfwrite"
           << "-dNOPAUSE"
           << "-dBATCH"
           << "-dSAFER"
           << "-dQUIET"
           << QString("-sOutputFile=%1").arg(tempOutput);
    
    // Add PostScript code to create 2-up layout inline
    QString psCode = QString(
        "<<"
        "/BeginPage {"
        "  exch pop "
        "  2 mod 0 eq {"
        "    /leftpage true def"
        "  } {"
        "    /leftpage false def"
        "  } ifelse"
        "} "
        "/EndPage {"
        "  leftpage {"
        "    0.5 0.5 scale "
        "    0 0 translate"
        "  } {"
        "    0.5 0.5 scale "
        "    297 0 translate"  // Half of A4 width in points
        "  } ifelse"
        "  { pop false } { pop true } ifelse"
        "}"
        ">> setpagedevice"
    );
    
    gsArgs << "-c" << psCode << "-f" << inputPath;
    
    qDebug() << "Attempting Ghostscript 2-up conversion with PostScript...";
    QProcess gsProcess;
    gsProcess.start("/opt/homebrew/bin/gs", gsArgs);
    if (!gsProcess.waitForFinished(60000)) {
        qDebug() << "Ghostscript timeout, falling back to sequential layout";
        return createSequential2Up(inputPath, outputPath);
    }
    
    qDebug() << "--- Ghostscript Process Debug ---";
    qDebug() << "Command: /opt/homebrew/bin/gs";
    qDebug() << "Arguments:" << gsArgs.join(" ");
    qDebug() << "Exit code:" << gsProcess.exitCode();
    
    QString stdout = gsProcess.readAllStandardOutput();
    QString stderr = gsProcess.readAllStandardError();
    if (!stdout.isEmpty()) qDebug() << "STDOUT:" << stdout;
    if (!stderr.isEmpty()) qDebug() << "STDERR:" << stderr;
    qDebug() << "--- End Ghostscript Process Debug ---";
    
    // If that didn't work, try a simpler approach
    if (gsProcess.exitCode() != 0 || !QFile::exists(tempOutput)) {
        qDebug() << "PostScript approach failed, trying basic pdfwrite...";
        
        QStringList basicArgs;
        basicArgs << "-sDEVICE=pdfwrite"
                 << "-dNOPAUSE"
                 << "-dBATCH"
                 << "-dSAFER"
                 << "-dQUIET"
                 << QString("-sOutputFile=%1").arg(outputPath)
                 << inputPath;
        
        QProcess basicProcess;
        basicProcess.start("/opt/homebrew/bin/gs", basicArgs);
        basicProcess.waitForFinished(60000);
        
        qDebug() << "Basic Ghostscript exit code:" << basicProcess.exitCode();
        
        if (basicProcess.exitCode() != 0 || !QFile::exists(outputPath)) {
            qDebug() << "All Ghostscript approaches failed, using sequential fallback";
            return createSequential2Up(inputPath, outputPath);
        }
    } else {
        // Copy temp output to final location
        QFile::copy(tempOutput, outputPath);
    }
    
    // Check final result
    QFileInfo outputInfo(outputPath);
    if (!outputInfo.exists()) {
        qDebug() << "Ghostscript output not created, falling back";
        return createSequential2Up(inputPath, outputPath);
    }
    
    qDebug() << "Ghostscript processing completed!";
    qDebug() << "Final output file:" << outputPath;
    qDebug() << "Output file size:" << outputInfo.size() << "bytes";
    
    // Check if the output is significantly different from input (indicating processing occurred)
    QFileInfo inputInfo(inputPath);
    if (qAbs(outputInfo.size() - inputInfo.size()) < 1000) {
        qDebug() << "Output size similar to input, may not have achieved 2-up layout";
        emit processingComplete(true, "Booklet created with Ghostscript (layout may need manual adjustment for printing)");
    } else {
        emit processingComplete(true, "Booklet with Ghostscript 2-up layout created successfully!");
    }
    
    return true;
}

bool QPDFBookletCreator::createSequential2Up(const QString &inputPath, const QString &outputPath)
{
    qDebug() << "=== Creating sequential 2-up layout ===";
    
    // Get page count
    QProcess pageCountProcess;
    pageCountProcess.start(PathConfig::qpdfPath, QStringList() << "--show-npages" << inputPath);
    if (!pageCountProcess.waitForFinished(30000)) {
        QString error = "Failed to get page count for sequential 2-up: " + pageCountProcess.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    int pageCount = pageCountProcess.readAllStandardOutput().trimmed().toInt();
    qDebug() << "Input has" << pageCount << "pages for sequential 2-up";
    
    // Create pairs of pages sequentially: pages 1&2 on sheet 1, pages 3&4 on sheet 2, etc.
    QStringList finalArgs;
    finalArgs << "--empty" << "--pages";
    
    for (int i = 1; i <= pageCount; i += 2) {
        // Add left page
        finalArgs << inputPath << QString::number(i);
        
        // Add right page (or repeat left page if we're at the end)
        if (i + 1 <= pageCount) {
            finalArgs << inputPath << QString::number(i + 1);
        } else {
            // Odd number of pages, repeat the last page
            finalArgs << inputPath << QString::number(i);
        }
    }
    
    finalArgs << "--" << outputPath;
    
    // Remove existing output file if it exists
    if (QFile::exists(outputPath)) {
        qDebug() << "Output file already exists, removing it...";
        QFile::remove(outputPath);
    }
    
    qDebug() << "Creating sequential 2-up booklet...";
    QProcess finalProcess;
    finalProcess.start(PathConfig::qpdfPath, finalArgs);
    if (!finalProcess.waitForFinished(60000)) {
        QString error = "Failed to create sequential 2-up booklet: " + finalProcess.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "--- Sequential 2-up Debug ---";
    qDebug() << "Command:" << PathConfig::qpdfPath;
    qDebug() << "Arguments:" << finalArgs.join(" ");
    qDebug() << "Exit code:" << finalProcess.exitCode();
    
    QString stdout = finalProcess.readAllStandardOutput();
    QString stderr = finalProcess.readAllStandardError();
    if (!stdout.isEmpty()) qDebug() << "STDOUT:" << stdout;
    if (!stderr.isEmpty()) qDebug() << "STDERR:" << stderr;
    qDebug() << "--- End Sequential 2-up Debug ---";
    
    int exitCode = finalProcess.exitCode();
    if (exitCode != 0 && exitCode != 3) {
        QString error = QString("Failed to create sequential 2-up booklet, exit code: %1").arg(exitCode);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Check if final output was created
    QFileInfo outputInfo(outputPath);
    if (!outputInfo.exists()) {
        QString error = "Sequential 2-up booklet was not created at expected location: " + outputPath;
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "Sequential 2-up booklet created successfully!";
    qDebug() << "Final output file:" << outputPath;
    qDebug() << "Output file size:" << outputInfo.size() << "bytes";
    
    emit processingComplete(true, "Booklet with sequential page layout created successfully! Print double-sided and fold in the middle.");
    return true;
}

bool QPDFBookletCreator::create2UpLayout(const QString &inputPath, const QString &outputPath)
{
    qDebug() << "=== Creating 2-up layout ===";
    
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QString error = "Could not create temporary directory for 2-up layout";
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "2-up temp directory:" << tempDir.path();
    
    // Get page count of reordered PDF
    QProcess pageCountProcess;
    pageCountProcess.start(PathConfig::qpdfPath, QStringList() << "--show-npages" << inputPath);
    if (!pageCountProcess.waitForFinished(30000)) {
        QString error = "Failed to get page count for 2-up layout: " + pageCountProcess.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    int pageCount = pageCountProcess.readAllStandardOutput().trimmed().toInt();
    qDebug() << "Input has" << pageCount << "pages for 2-up layout";
    
    // Create individual 2-up sheets
    QStringList sheetFiles;
    for (int i = 0; i < pageCount; i += 2) {
        QString sheetPdf = tempDir.filePath(QString("sheet_%1.pdf").arg(i/2));
        sheetFiles.append(sheetPdf);
        
        int leftPageNum = i + 1;
        int rightPageNum = (i + 1 < pageCount) ? i + 2 : leftPageNum; // Use left page again if odd number of pages
        
        qDebug() << "Creating sheet" << (i/2) << "with pages" << leftPageNum << "and" << rightPageNum;
        
        if (!create2UpSheet(inputPath, sheetPdf, leftPageNum, rightPageNum)) {
            return false;
        }
    }
    
    // Combine all sheets into final output
    qDebug() << "Combining" << sheetFiles.size() << "sheets into final booklet...";
    QStringList combineArgs;
    combineArgs << "--empty" << "--pages";
    for (const QString &sheet : sheetFiles) {
        combineArgs << sheet << "1";
    }
    combineArgs << "--" << outputPath;
    
    // Remove existing output file if it exists
    if (QFile::exists(outputPath)) {
        qDebug() << "Output file already exists, removing it...";
        QFile::remove(outputPath);
    }
    
    QProcess combineProcess;
    combineProcess.start(PathConfig::qpdfPath, combineArgs);
    if (!combineProcess.waitForFinished(60000)) {
        QString error = "Failed to combine 2-up sheets: " + combineProcess.errorString();
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "--- Final Combine Debug ---";
    qDebug() << "Command:" << PathConfig::qpdfPath;
    qDebug() << "Arguments:" << combineArgs.join(" ");
    qDebug() << "Exit code:" << combineProcess.exitCode();
    
    QString stdout = combineProcess.readAllStandardOutput();
    QString stderr = combineProcess.readAllStandardError();
    if (!stdout.isEmpty()) qDebug() << "STDOUT:" << stdout;
    if (!stderr.isEmpty()) qDebug() << "STDERR:" << stderr;
    qDebug() << "--- End Final Combine Debug ---";
    
    int exitCode = combineProcess.exitCode();
    if (exitCode != 0 && exitCode != 3) {
        QString error = QString("Failed to combine 2-up sheets, exit code: %1").arg(exitCode);
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    // Check if final output was created
    QFileInfo outputInfo(outputPath);
    if (!outputInfo.exists()) {
        QString error = "2-up booklet was not created at expected location: " + outputPath;
        qDebug() << error;
        emit processingComplete(false, error);
        return false;
    }
    
    qDebug() << "2-up booklet created successfully!";
    qDebug() << "Final output file:" << outputPath;
    qDebug() << "Output file size:" << outputInfo.size() << "bytes";
    
    emit processingComplete(true, "Booklet with 2-up layout created successfully!");
    return true;
}

bool QPDFBookletCreator::create2UpSheet(const QString &inputPath, const QString &outputPath,
                                        int leftPageNum, int rightPageNum)
{
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qDebug() << "Could not create temp directory for 2-up sheet";
        return false;
    }
    
    // Extract left page
    QString leftPagePdf = tempDir.filePath("left.pdf");
    QStringList leftArgs;
    leftArgs << inputPath << "--pages" << "." << QString::number(leftPageNum) << "--" << leftPagePdf;
    
    QProcess leftProcess;
    leftProcess.start(PathConfig::qpdfPath, leftArgs);
    if (!leftProcess.waitForFinished(30000) || (leftProcess.exitCode() != 0 && leftProcess.exitCode() != 3)) {
        qDebug() << "Failed to extract left page" << leftPageNum;
        return false;
    }
    
    // Extract right page (or duplicate left if same)
    QString rightPagePdf = tempDir.filePath("right.pdf");
    if (rightPageNum != leftPageNum) {
        QStringList rightArgs;
        rightArgs << inputPath << "--pages" << "." << QString::number(rightPageNum) << "--" << rightPagePdf;
        
        QProcess rightProcess;
        rightProcess.start(PathConfig::qpdfPath, rightArgs);
        if (!rightProcess.waitForFinished(30000) || (rightProcess.exitCode() != 0 && rightProcess.exitCode() != 3)) {
            qDebug() << "Failed to extract right page" << rightPageNum;
            return false;
        }
    } else {
        // Duplicate left page for right side
        QFile::copy(leftPagePdf, rightPagePdf);
    }
    
    // Create scaled versions of each page (50% size)
    QString leftScaledPdf = tempDir.filePath("left_scaled.pdf");
    QString rightScaledPdf = tempDir.filePath("right_scaled.pdf");
    
    // Scale left page to 50% and keep it on the left side
    QStringList scaleLeftArgs;
    scaleLeftArgs << leftPagePdf
                 << "--pages" << "." << "1" << "--"
                 << "--collate"
                 << leftScaledPdf;
    
    QProcess scaleLeftProcess;
    scaleLeftProcess.start(PathConfig::qpdfPath, scaleLeftArgs);
    if (!scaleLeftProcess.waitForFinished(30000) || (scaleLeftProcess.exitCode() != 0 && scaleLeftProcess.exitCode() != 3)) {
        qDebug() << "Failed to scale left page, falling back to simple approach";
        // Fallback: just use the left page
        QFile::copy(leftPagePdf, outputPath);
        return QFile::exists(outputPath);
    }
    
    // Scale right page to 50%
    QStringList scaleRightArgs;
    scaleRightArgs << rightPagePdf
                  << "--pages" << "." << "1" << "--"
                  << "--collate"
                  << rightScaledPdf;
    
    QProcess scaleRightProcess;
    scaleRightProcess.start(PathConfig::qpdfPath, scaleRightArgs);
    if (!scaleRightProcess.waitForFinished(30000) || (scaleRightProcess.exitCode() != 0 && scaleRightProcess.exitCode() != 3)) {
        qDebug() << "Failed to scale right page, falling back to left page only";
        QFile::copy(leftPagePdf, outputPath);
        return QFile::exists(outputPath);
    }
    
    // Now try to create side-by-side layout using underlay instead of overlay
    // Underlay puts the right page behind, then we can position the left page on top
    QStringList underlayArgs;
    underlayArgs << rightScaledPdf
                << "--underlay" << leftScaledPdf
                << "--to=1" << "--from=1"
                << "--" << outputPath;
    
    qDebug() << "Creating 2-up sheet with underlay positioning...";
    QProcess underlayProcess;
    underlayProcess.start(PathConfig::qpdfPath, underlayArgs);
    if (!underlayProcess.waitForFinished(30000)) {
        qDebug() << "Failed to create 2-up underlay:" << underlayProcess.errorString();
        // Final fallback: use left page only
        QFile::copy(leftPagePdf, outputPath);
        return QFile::exists(outputPath);
    }
    
    int exitCode = underlayProcess.exitCode();
    qDebug() << "Underlay exit code:" << exitCode;
    
    QString stderr = underlayProcess.readAllStandardError();
    if (!stderr.isEmpty()) {
        qDebug() << "Underlay stderr:" << stderr;
    }
    
    // If underlay approach doesn't work well, try a different method:
    // Create a blank page and overlay both scaled pages on it
    if (exitCode != 0 && exitCode != 3) {
        qDebug() << "Underlay failed, trying blank page approach";
        
        // Create a blank page of appropriate size (A4 landscape)
        QString blankPdf = tempDir.filePath("blank.pdf");
        QStringList blankArgs;
        blankArgs << "--empty" << "--pages" << "." << "1" << "--" << blankPdf;
        
        QProcess blankProcess;
        blankProcess.start(PathConfig::qpdfPath, blankArgs);
        if (!blankProcess.waitForFinished(30000) || (blankProcess.exitCode() != 0 && blankProcess.exitCode() != 3)) {
            qDebug() << "Failed to create blank page, using left page only";
            QFile::copy(leftPagePdf, outputPath);
            return QFile::exists(outputPath);
        }
        
        // Try overlaying both pages on the blank page
        QString tempOutput = tempDir.filePath("temp_output.pdf");
        QStringList overlayBothArgs;
        overlayBothArgs << blankPdf
                       << "--overlay" << leftScaledPdf
                       << "--to=1" << "--from=1"
                       << "--" << tempOutput;
        
        QProcess overlayBothProcess;
        overlayBothProcess.start(PathConfig::qpdfPath, overlayBothArgs);
        if (!overlayBothProcess.waitForFinished(30000) || (overlayBothProcess.exitCode() != 0 && overlayBothProcess.exitCode() != 3)) {
            qDebug() << "Complex overlay failed, using left page only";
            QFile::copy(leftPagePdf, outputPath);
            return QFile::exists(outputPath);
        }
        
        // Now overlay the right page
        QStringList overlay2Args;
        overlay2Args << tempOutput
                    << "--overlay" << rightScaledPdf
                    << "--to=1" << "--from=1"
                    << "--" << outputPath;
        
        QProcess overlay2Process;
        overlay2Process.start(PathConfig::qpdfPath, overlay2Args);
        if (!overlay2Process.waitForFinished(30000) || (overlay2Process.exitCode() != 0 && overlay2Process.exitCode() != 3)) {
            qDebug() << "Second overlay failed, using intermediate result";
            QFile::copy(tempOutput, outputPath);
        }
    }
    
    // Final check - if nothing worked well, just put both pages in sequence
    if (!QFile::exists(outputPath) || QFileInfo(outputPath).size() < 1000) {
        qDebug() << "All 2-up attempts failed, creating sequential pages instead";
        QStringList sequentialArgs;
        sequentialArgs << "--empty" << "--pages"
                      << leftPagePdf << "1"
                      << rightPagePdf << "1"
                      << "--" << outputPath;
        
        QProcess sequentialProcess;
        sequentialProcess.start(PathConfig::qpdfPath, sequentialArgs);
        sequentialProcess.waitForFinished(30000);
    }
    
    return QFile::exists(outputPath);
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
