#include "glviewer.h"

#include "abstractexercise.h"
#include "camera.h"

#include <QtMath>

GLViewer::GLViewer(AbstractExercise *painter)
    : QOpenGLWindow()
    , m_painter(painter)
    , m_painterInitialized(false)
{
	QSurfaceFormat format;
	format.setVersion(4, 0);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapInterval(1);
    format.setDepthBufferSize(16); // Required to allow both 16 and 24 bit depth buffers (fix for OS X and Intel HD)
	setFormat(format);

	resize(800, 600);

	m_camera.setFovy(40.0);
	m_camera.setCenter(QVector3D(0.0f, 0.0f, 0.0f));
	m_camera.setEye(QVector3D(0.0f, 0.0f, 6.0f));
	m_camera.setUp(QVector3D(0.f, 1.0f, 0.0f));
}

GLViewer::~GLViewer()
{
    delete m_painter;
}

void GLViewer::initializeGL()
{
    if (!m_painterInitialized)
    {
        m_painterInitialized = true;
        m_painter->initialize(m_camera);

        if (!m_initialSize.isEmpty())
        {
            m_painter->resize(m_initialSize.width(), m_initialSize.height());
            m_camera.setViewport(m_initialSize);
        }
    }
}

void GLViewer::resizeGL(int w, int h)
{
    if (!m_painterInitialized)
    {
        m_initialSize = QSize(w, h);

        return;
    }

	m_painter->resize(w, h);
	m_camera.setViewport(QSize(w, h));
}

void GLViewer::paintGL()
{	
    if (renderLoopRequired())
	{
		m_painter->update();

		update(); //request next frame
	}
	m_painter->render(m_camera);
}

void GLViewer::keyPressEvent(QKeyEvent *event)
{
	if (m_painter->onKeyPressed(event, m_camera))
	{
		event->accept();
		restartRenderLoopIfNecessary();
		return;
	}

	event->ignore();
}

void GLViewer::exposeEvent(QExposeEvent *ev)
{
	restartRenderLoopIfNecessary();
}

bool GLViewer::renderLoopRequired()
{
	return isExposed() && m_painter->redrawRequested();
}

void GLViewer::restartRenderLoopIfNecessary()
{
	if (renderLoopRequired())
	{
		//start render loop again
		update();
	}
}
