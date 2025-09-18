#include "pdfpreviewwidget.h"
#include <QDebug>
#include <QPainter>
#include <QFile>

// Use PDFium for PDF rendering
// Note: This is a simplified implementation - in a real app, you'd want to use the PDFium C API properly
// For demonstration purposes, we're using a simple QImage-based approach
// You would need to properly implement the PDFium integration in a real application

PDFPreviewWidget::PDFPreviewWidget(QWidget *parent)
    : QWidget(parent), m_currentPage(0), m_pageCount(0), m_scale(1.0), m_dpi(72)
{
    setMinimumSize(200, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

PDFPreviewWidget::~PDFPreviewWidget()
{
    clearPreview();
}

bool PDFPreviewWidget::loadPDF(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        qWarning() << "PDF file does not exist:" << filePath;
        return false;
    }
    
    m_pdfPath = filePath;
    m_currentPage = 0;
    
    // In a real implementation, you would:
    // 1. Use PDFium to open the PDF
    // 2. Get the page count
    // 3. Set up the document for rendering
    
    // For this simplified version:
    m_pageCount = 1; // Placeholder
    
    // Render the first page
    renderPage();
    
    update();
    return true;
}

void PDFPreviewWidget::clearPreview()
{
    m_pdfPath.clear();
    m_pageImage = QImage();
    m_currentPage = 0;
    m_pageCount = 0;
    update();
}

void PDFPreviewWidget::setCurrentPage(int pageIndex)
{
    if (pageIndex >= 0 && pageIndex < m_pageCount && pageIndex != m_currentPage) {
        m_currentPage = pageIndex;
        renderPage();
        update();
    }
}

int PDFPreviewWidget::pageCount() const
{
    return m_pageCount;
}

void PDFPreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::lightGray);
    
    if (m_pageImage.isNull()) {
        // Draw placeholder or "No PDF loaded" message
        painter.setPen(Qt::black);
        painter.drawText(rect(), Qt::AlignCenter, "No PDF loaded");
        return;
    }
    
    // Calculate the position to center the page
    QRect targetRect;
    double aspect = (double)m_pageImage.width() / m_pageImage.height();
    
    if (width() / aspect <= height()) {
        // Fit to width
        int targetHeight = static_cast<int>(width() / aspect);
        targetRect = QRect(0, (height() - targetHeight) / 2, width(), targetHeight);
    } else {
        // Fit to height
        int targetWidth = static_cast<int>(height() * aspect);
        targetRect = QRect((width() - targetWidth) / 2, 0, targetWidth, height());
    }
    
    // Draw the page
    painter.drawImage(targetRect, m_pageImage);
    
    // Draw a border
    painter.setPen(Qt::black);
    painter.drawRect(targetRect);
}

void PDFPreviewWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Re-render the page at the new size
    if (!m_pdfPath.isEmpty()) {
        renderPage();
    }
}

void PDFPreviewWidget::renderPage()
{
    if (m_pdfPath.isEmpty() || m_currentPage >= m_pageCount) {
        m_pageImage = QImage();
        return;
    }
    
    // This is where you would use PDFium to render the page
    // For this simplified implementation, we'll create a placeholder image
    
    // Calculate DPI based on widget size to get a good quality preview
    m_dpi = std::max(72, std::min(300, width() / (int)(A4_WIDTH / 72)));
    
    // Create a blank white image with A4 proportions
    int imgWidth = static_cast<int>(A4_WIDTH * m_dpi / 72);
    int imgHeight = static_cast<int>(A4_HEIGHT * m_dpi / 72);
    
    m_pageImage = QImage(imgWidth, imgHeight, QImage::Format_RGB32);
    m_pageImage.fill(Qt::white);
    
    // Draw a placeholder - in a real app, you'd render the actual PDF page
    QPainter painter(&m_pageImage);
    painter.setPen(Qt::gray);
    painter.drawRect(0, 0, imgWidth-1, imgHeight-1);
    
    // Draw some text
    QFont font = painter.font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.setPen(Qt::black);
    painter.drawText(QRect(0, 0, imgWidth, imgHeight), Qt::AlignCenter, 
                     "PDF Preview\nPage " + QString::number(m_currentPage + 1) + " of " + QString::number(m_pageCount));
    
    // Draw A6 guides
    painter.setPen(QPen(Qt::lightGray, 2, Qt::DashLine));
    painter.drawLine(imgWidth/2, 0, imgWidth/2, imgHeight);
    
    // In a real implementation:
    // 1. Use PDFium to render the specific page to a bitmap
    // 2. Convert that bitmap to a QImage
    // 3. Store the result in m_pageImage
}
