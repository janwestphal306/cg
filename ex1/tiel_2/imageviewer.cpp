#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QtWidgets>

#include "imageeffects.h"

ImageViewer::ImageViewer()
	: m_ui(new Ui_ImageViewer())
{
	m_ui->setupUi(this);

	m_ui->scrollArea->setWidget(m_ui->imageLabel);
	m_ui->scrollArea->setAlignment(Qt::AlignCenter);

	connect(m_ui->actionOpen, &QAction::triggered, this, &ImageViewer::open);
	connect(m_ui->actionSave, &QAction::triggered, this, &ImageViewer::save);
	connect(m_ui->actionExit, &QAction::triggered, this, &ImageViewer::close);
	connect(m_ui->actionZoomIn, &QAction::triggered, this, &ImageViewer::zoomIn);
	connect(m_ui->actionZoomOut, &QAction::triggered, this, &ImageViewer::zoomOut);
	connect(m_ui->actionNormalSize, &QAction::triggered, this, &ImageViewer::normalSize);
	connect(m_ui->actionFitToWindow, &QAction::triggered, this, &ImageViewer::fitToWindow);
	connect(m_ui->actionReset, &QAction::triggered, this, &ImageViewer::reset);
	connect(m_ui->actionBlur, &QAction::triggered, this, &ImageViewer::blur);
	connect(m_ui->actionHypothecialContrast, &QAction::triggered, this, &ImageViewer::hypotheticalContrast);
	connect(m_ui->actionSharpness, &QAction::triggered, this, &ImageViewer::sharpness);
	connect(m_ui->actionEdgeDetection, &QAction::triggered, this, &ImageViewer::edgeDetection);
	connect(m_ui->actionColorBasedSharpness, &QAction::triggered, this, &ImageViewer::colorBasedSharpness);
	connect(m_ui->actionAbout, &QAction::triggered, this, &ImageViewer::about);

	//connect(&thread, SIGNAL(renderedImage(QImage)), this, SLOT(renderedMandelbrot(QImage)));

	resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

ImageViewer::~ImageViewer()
{
	delete m_ui;
}

bool ImageViewer::loadFile(const QString &fileName)
{
	QImageReader reader(fileName);

	//reader.setAutoTransform(true);

	const QImage image = reader.read();
	if (image.isNull())
	{
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(), tr("Cannot load %1: %2").arg(QDir::toNativeSeparators(fileName)).arg(reader.errorString()));
		setWindowFilePath(QString());
		m_ui->imageLabel->setPixmap(QPixmap());
		m_ui->imageLabel->adjustSize();

		m_originalImage = QImage();

		updateFilterActions();

		return false;
	}

	m_originalImage = image;
	m_ui->imageLabel->setPixmap(QPixmap::fromImage(image));
	m_scaleFactor = 1.0;
	updateFilterActions();

	m_ui->actionFitToWindow->setEnabled(true);
	updateZoomActions();

	if (!m_ui->actionFitToWindow->isChecked())
	{
		m_ui->imageLabel->adjustSize();
	}

	setWindowFilePath(fileName);
	return true;
}

void ImageViewer::open()
	{
	QStringList mimeTypeFilters;
	foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
	{
		mimeTypeFilters.append(mimeTypeName);
	}

	mimeTypeFilters.sort();
	const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	QFileDialog dialog(this, tr("Open File"),
					   picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setMimeTypeFilters(mimeTypeFilters);

	QStringList list = dialog.nameFilters();
	list.prepend("All Files (*.*)");
	dialog.setNameFilters(list);

	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::save()
{
	QString filename = QFileDialog::getSaveFileName(this);

	if (filename.isEmpty())
	{
		return;
	}

	m_ui->imageLabel->pixmap()->toImage().save(filename);
}

void ImageViewer::zoomIn()
{
	scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
	scaleImage(0.8);
}

void ImageViewer::normalSize()
{
	m_ui->imageLabel->adjustSize();
	m_scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
	bool fitToWindow = m_ui->actionFitToWindow->isChecked();
	m_ui->scrollArea->setWidgetResizable(fitToWindow);
	if (!fitToWindow)
	{
		normalSize();
	}
	updateZoomActions();
}

void ImageViewer::about()
{
	QMessageBox::about(this, tr("About Image Viewer"),
		tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
			"and QScrollArea to display an image. QLabel is typically used "
			"for displaying a text, but it can also display an image. "
			"QScrollArea provides a scrolling view around another widget. "
			"If the child widget exceeds the size of the frame, QScrollArea "
			"automatically provides scroll bars. </p><p>The example "
			"demonstrates how QLabel's ability to scale its contents "
			"(QLabel::scaledContents), and QScrollArea's ability to "
			"automatically resize its contents "
			"(QScrollArea::widgetResizable), can be used to implement "
			"zooming and scaling features. </p><p>In addition the example "
			"shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::updateZoomActions()
{
	m_ui->actionZoomIn->setEnabled(!m_ui->actionFitToWindow->isChecked());
	m_ui->actionZoomOut->setEnabled(!m_ui->actionFitToWindow->isChecked());
	m_ui->actionNormalSize->setEnabled(!m_ui->actionFitToWindow->isChecked());
}

void ImageViewer::updateFilterActions()
{
	bool enabled = !m_originalImage.isNull();

	m_ui->actionReset->setEnabled(enabled);
	m_ui->actionBlur->setEnabled(enabled);
	m_ui->actionHypothecialContrast->setEnabled(enabled);
	m_ui->actionSharpness->setEnabled(enabled);
	m_ui->actionEdgeDetection->setEnabled(enabled);
	m_ui->actionColorBasedSharpness->setEnabled(enabled);
	m_ui->actionFlowField->setEnabled(enabled);
	m_ui->actionFlowFieldFilter->setEnabled(enabled);
}

void ImageViewer::scaleImage(double factor)
{
	Q_ASSERT(m_ui->imageLabel->pixmap());
	m_scaleFactor *= factor;
	m_ui->imageLabel->resize(m_scaleFactor * m_ui->imageLabel->pixmap()->size());

	adjustScrollBar(m_ui->scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(m_ui->scrollArea->verticalScrollBar(), factor);

	m_ui->actionZoomIn->setEnabled(m_scaleFactor < 4.0);
	m_ui->actionZoomOut->setEnabled(m_scaleFactor > 0.25);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

// Image operations
void ImageViewer::reset()
{
    m_ui->imageLabel->setPixmap(QPixmap::fromImage(m_originalImage));
}

void ImageViewer::blur()
{
	if (m_originalImage.isNull())
	{
		return;
	}

    m_ui->imageLabel->setPixmap(QPixmap::fromImage(ImageEffects::blur(m_originalImage)));
}

void ImageViewer::hypotheticalContrast()
{
	if (m_originalImage.isNull())
	{
		return;
	}

    m_ui->imageLabel->setPixmap(QPixmap::fromImage(ImageEffects::hypotheticalContrast(m_originalImage)));
}

void ImageViewer::sharpness()
{
	if (m_originalImage.isNull())
	{
		return;
	}

    m_ui->imageLabel->setPixmap(QPixmap::fromImage(ImageEffects::sharpness(m_originalImage)));
}

void ImageViewer::edgeDetection()
{
	if (m_originalImage.isNull())
	{
		return;
	}

    m_ui->imageLabel->setPixmap(QPixmap::fromImage(ImageEffects::edgeDetection(m_originalImage)));
}

void ImageViewer::colorBasedSharpness()
{
	if (m_originalImage.isNull())
	{
		return;
	}

	m_ui->imageLabel->setPixmap(QPixmap::fromImage(ImageEffects::colorBasedSharpness(m_originalImage)));
}

