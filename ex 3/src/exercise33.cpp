#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QScopedPointer>
#include <QApplication>
#include <QMainWindow>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"
#include "util/unitcube.h"

class Exercise33 : public AbstractExercise
{
public:
    Exercise33();
	~Exercise33() override;

    const QString hints() const override;
    bool initialize() override;
    void render() override;

protected:
    QMatrix4x4 rotateClockwise(int frame);
    QMatrix4x4  m_cubeTransform;
    QScopedPointer<QOpenGLShaderProgram> m_program;
    UnitCube m_cube;
};

Exercise33::Exercise33()
{
    m_cubeTransform.scale(0.5f);
}

Exercise33::~Exercise33()
{
}

const QString Exercise33::hints() const
{
	return QString("Press [SPACE] to pause or resume animation.");
}

bool Exercise33::initialize()
{
    AbstractExercise::initialize();

    m_camera->setEye(QVector3D(0.0f, 3.0f, -6.0f));

    m_program.reset(createBasicShaderProgram("data/cube.vert", "data/cube.frag"));
    m_program->bind();

    m_cube.initialize(*m_program);

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	return true;
}

void Exercise33::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
    m_program->setUniformValue("viewprojection", camera()->viewProjection());

    m_program->setUniformValue("transform", m_cubeTransform);
    m_program->setUniformValue("normalMatrix", (camera()->view() * m_cubeTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));

    m_cube.draw(*this);

    m_program->setUniformValue("transform", rotateClockwise(m_frame) * m_cubeTransform);
    m_program->setUniformValue("normalMatrix", (camera()->view() * rotateClockwise(m_frame) * m_cubeTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1.0f, 1.0f, 0.0f, 1.0f));

    m_cube.draw(*this);
}

QMatrix4x4 Exercise33::rotateClockwise(int frame)
{
	// TODO: Calculate rotation and translation for given frame

	QMatrix4x4 matrix;
	matrix.translate(0.0f, 1.0f, 0.0f);
	return matrix;
}

int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
	GLViewer viewer(new Exercise33);
	viewer.show();

    // Run application
    return app.exec();
}
