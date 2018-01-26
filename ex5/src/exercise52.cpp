#include "exercise52.h"

#include <cassert>

#include <QGuiApplication>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/polygonaldrawable.h"
#include "util/objio.h"

Exercise52::Exercise52()
    : AbstractExercise()
	, m_drawable(nullptr)
	, m_animationFrame(0)
{
}

Exercise52::~Exercise52()
{
	delete m_drawable;
}

bool Exercise52::hasAnimation()
{
	return true;
}

const QString Exercise52::hints() const
{
	return "Keys:\n\t[SPACE] : Pause/Resume animation.";
}

bool Exercise52::initialize(Camera &camera)
{
    AbstractExercise::initialize(camera);

	m_drawable = ObjIO::fromObjFile("data/bunny_sparse.obj");

	m_program.reset(createBasicShaderProgram("data/model.vert", "data/model.geom", "data/model.frag"));
    m_program->bind();

    m_drawable->createVAO(m_program.data());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	camera.setCenter(QVector3D(0.0f, 1.0f, 0.0f));
	camera.setEye(QVector3D(0.0f, 0.0f, 8.0f));

	return true;
}

void Exercise52::update()
{
	m_animationFrame++;
	if (m_animationFrame > 150)
	{
		m_animationFrame = 0;
	}
}

void Exercise52::render(const Camera &camera)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->bind();
	m_program->setUniformValue("lightsource", QVector3D(0.0, 10.0, 4.0));
	m_program->setUniformValue("color", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
	if (m_animationFrame > 25)
	{
		m_program->setUniformValue("animationFrame", (m_animationFrame - 25) / 125.0f);
	}
	else
	{
		m_program->setUniformValue("animationFrame", 0.0f);
	}

	m_program->setUniformValue("viewprojection", camera.viewProjection());
	m_program->setUniformValue("transform", QMatrix4x4());
	m_program->setUniformValue("normalMatrix", camera.view().normalMatrix());

    m_drawable->draw(*this);
}

int main(int argc, char *argv[])
{
	// Create application
	QGuiApplication app(argc, argv);

	// Create main window
	GLViewer viewer(new Exercise52);
	viewer.show();

	// Run application
	return app.exec();
}
