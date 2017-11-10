#pragma once

#include <QMainWindow>


class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class Ui_ImageViewer;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();
	~ImageViewer();
    bool loadFile(const QString &);

protected:
    void open();
    void save();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

    // Image operations
    void reset();
    void blur();
    void hypotheticalContrast();
    void sharpness();
    void edgeDetection();
	void colorBasedSharpness();

private:
    void updateZoomActions();
	void updateFilterActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QImage m_originalImage;
    double m_scaleFactor;

	Ui_ImageViewer* m_ui;
	//MandelbrotRenderThread thread;
};
