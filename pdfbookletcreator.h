#ifndef PDFBOOKLETCREATOR_H
#define PDFBOOKLETCREATOR_H

#include <QObject>
#include <QString>
#include <QImage>
#include <memory>
#include "pathconfig.h"

// Forward declarations for QPDF classes
namespace PoDoFo {
    class PdfMemDocument;
    class PdfPage;
    class PdfPainter;
}

class QPDFBookletCreator : public QObject
{
    Q_OBJECT
    
public:
    explicit QPDFBookletCreator(QObject *parent = nullptr);
    ~QPDFBookletCreator();
    
    bool createBooklet(const QString &inputPath, const QString &outputPath, bool startFromBeginning = true);
    
signals:
    void progressChanged(int progress);
    void processingComplete(bool success, const QString &message);
    
private:
    // A4 dimensions in points (72 points per inch)
    const double A4_WIDTH = 595.276;
    const double A4_HEIGHT = 841.89;
    
    // A6 dimensions (half of A5, which is half of A4)
    const double A6_WIDTH = A4_WIDTH / 2;
    const double A6_HEIGHT = A4_HEIGHT / 2;
    
    // Helper methods to create a booklet
    bool arrangePages(const QString &inputPath, const QString &outputPath, bool startFromBeginning);
    
    // Extract a page from a PDF to an image
    QImage renderPage(const QString &pdfPath, int pageNum);
    
    // Create a combined page with two source pages side by side
    bool createCombinedPage(const QString &inputPath, const QString &outputPath, 
                          const QList<int> &pageOrder);
    
    // Log progress update
    void logProgress(int current, int total);
};

#endif // PDFBOOKLETCREATOR_H
