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
	float minX = vertices.at(0).x(), maxX = vertices.at(0).x();
	float minY = vertices.at(0).y(), maxY = vertices.at(0).y();
	float minZ = vertices.at(0).z(), maxZ = vertices.at(0).z();
	for (QVector3D vec : vertices) {
		minX = std::min(vec.x(), minX);
		minY = std::min(vec.y(), minY);
		minZ = std::min(vec.z(), minZ);

		maxX = std::max(vec.x(), maxX);
		maxY = std::max(vec.y(), maxY);
		maxZ = std::max(vec.z(), maxZ);
	}

	std::cout << "Minima: " << minX << " " << minY << " " << minZ << std::endl;
	std::cout << "Maxima: " << maxX << " " << maxY << " " << maxZ << std::endl;

	float scaleX = 1 / (maxX - minX);
	float scaleY = 1 / (maxY - minY);
	float scaleZ = 1 / (maxZ - minZ);

	std::cout << "Scale: " << scaleX << " " << scaleY << " " << scaleZ << std::endl;
	
	float minScale = std::min(std::min(scaleX, scaleY), scaleZ);
	std::cout << minScale << std::endl;

	QMatrix4x4 transform;
	
	transform.translate(-0.5f, -0.5f, -0.5f);
	transform.scale(minScale);
	transform.translate(-minX, -minY, -minZ);

	float *data = transform.data();
	for (int i = 0; i < 16; i++) {
		std::cout << data[i] << " ";
	}
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