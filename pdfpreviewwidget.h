#ifndef PDFPREVIEWWIDGET_H
#define PDFPREVIEWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QImage>

class PDFPreviewWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PDFPreviewWidget(QWidget *parent = nullptr);
    ~PDFPreviewWidget();
    
    // Load a PDF file for preview
    bool loadPDF(const QString &filePath);
    
    // Clear the current preview
    void clearPreview();
    
    // Set the current page index (0-based)
    void setCurrentPage(int pageIndex);
    
    // Get the total number of pages
    int pageCount() const;
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private:
    // Render the current page
    void renderPage();
    
    QString m_pdfPath;
    int m_currentPage;
    int m_pageCount;
    QImage m_pageImage;
    
    // Target page size in points
    const double A4_WIDTH = 595.276;
    const double A4_HEIGHT = 841.89;
    
    // Rendering parameters
    double m_scale;
    int m_dpi;
};

#endif // PDFPREVIEWWIDGET_H
