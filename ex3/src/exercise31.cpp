#include <QApplication>

#include <cfloat>

#include "util/abstractexercise.h"
#include "util/camera.h"
#include "util/glviewer.h"
#include "util/objio.h"
#include "util/polygonaldrawable.h"
#include "util/unitcube.h"

#include <iostream>

class Exercise31 : public AbstractExercise
{
public:
	Exercise31();
	~Exercise31() override;

	const QString hints() const override;

	bool initialize() override;
	void render() override;

protected:
	UnitCube m_cube;
	PolygonalDrawable * m_drawable;

	QScopedPointer<QOpenGLShaderProgram> m_cubeProgram;
	QScopedPointer<QOpenGLShaderProgram> m_modelProgram;

	QMatrix4x4 m_modelTransform;

	static QMatrix4x4 calculateModelTransform(const PolygonalDrawable * const drawable);
};

QMatrix4x4 Exercise31::calculateModelTransform(const PolygonalDrawable * const drawable)
{
	// TODO: Calculate manipulation matrix here.
	
	QVector<QVector3D> vertices = drawable->vertices();

	QVector3D minima(vertices.at(0).x(), vertices.at(0).y(), vertices.at(0).z());
	QVector3D maxima(minima);
	for (QVector3D vec : vertices) {
		minima.setX(std::min(vec.x(), minima.x()));
		minima.setY(std::min(vec.y(), minima.y()));
		minima.setZ(std::min(vec.z(), minima.z()));

		maxima.setX(std::max(vec.x(), maxima.x()));
		maxima.setY(std::max(vec.y(), maxima.y()));
		maxima.setZ(std::max(vec.z(), maxima.z()));
	}
	qDebug() << "min: " << minima;
	qDebug() << "max: " << maxima;

	QVector3D delta = maxima - minima;
	float maxDelta = std::max(std::max(delta.x(), delta.y()), delta.z());
	qDebug() << "Delta: " << delta << " max Delta: " << maxDelta;

	QMatrix4x4 transform;
	float uniformScale = 1 / maxDelta;
	transform.scale(uniformScale);

	QVector3D center = (maxima + minima) / 2;
	transform.translate(-center);
	
	qDebug() << transform;
	return transform;
}

Exercise31::Exercise31()
	: m_drawable(nullptr)
{

}

Exercise31::~Exercise31()
{
	delete m_drawable;
}

const QString Exercise31::hints() const
{
	return "";
}

bool Exercise31::initialize()
{
	AbstractExercise::initialize();

	m_modelProgram.reset(createBasicShaderProgram("data/model.vert", "data/model.frag"));
	m_modelProgram->bind();

	m_drawable = ObjIO::fromObjFile("data/suzanne2.obj");
	m_drawable->createVAO(m_modelProgram.data());

	m_cubeProgram.reset(createBasicShaderProgram("data/cube.vert", "data/cube.frag"));

	m_cube.initialize(*m_cubeProgram);
	camera()->setEye(QVector3D(1.0f, 1.0f, 4.0f));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	m_modelTransform = calculateModelTransform(m_drawable);

	return true;
}

void Exercise31::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//draw model
	m_modelProgram->bind();
	m_modelProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 4.0f));
	m_modelProgram->setUniformValue("viewprojection", camera()->viewProjection());

	m_modelProgram->bind();
	m_modelProgram->setUniformValue("transform", m_modelTransform);
	m_modelProgram->setUniformValue("normalMatrix", (camera()->view() * m_modelTransform).normalMatrix());
	m_modelProgram->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

	m_drawable->draw(*this);

	//draw cube
	m_cubeProgram->bind();
	m_cubeProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 0.0f));
	m_cubeProgram->setUniformValue("viewprojection", camera()->viewProjection());
	m_cubeProgram->setUniformValue("color", QVector4D(0.8f, 0.2f, 0.2f, 0.2f));

	QMatrix4x4 transform;
	transform.scale(0.5f, 0.5f, 0.5f);

	m_cubeProgram->setUniformValue("transform", transform);
	m_cubeProgram->setUniformValue("normalMatrix", (camera()->view() * transform).normalMatrix());

	m_cube.draw(*this);
}

int main(int argc, char *argv[])
{
	// Create application
	QApplication app(argc, argv);

	// Create main window
	GLViewer viewer(new Exercise31);
	viewer.show();

	// Run application
	return app.exec();
}