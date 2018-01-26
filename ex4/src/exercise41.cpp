#include "exercise41.h"

#include <cassert>

#include <QGuiApplication>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QRectF>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/polygonaldrawable.h"
#include "util/objio.h"

#include <QTextStream>
QTextStream out(stdout);

Exercise41::Exercise41()
	: m_startAngle(-150.0f, 10.0f, -160.0f)
	, m_endAngle(0.0f, 0.0f, 0.0f)
	, m_drawable(nullptr)
	, m_frame(0)
{
}

Exercise41::~Exercise41()
{
	delete m_drawable;
}

bool Exercise41::initialize()
{
	AbstractExercise::initialize();

	if (m_drawable)
		return true;

	m_program.reset(createBasicShaderProgram("data/model.vert", "data/model.frag"));
	m_program->bind();

	const std::string fileName = "data/suzanne.obj";
	m_drawable = ObjIO::fromObjFile(fileName);

	m_drawable->createVAO(m_program.data());

	m_cubeProgram.reset(createBasicShaderProgram("data/cube14.vert", "data/cube14.frag"));
	m_cubeProgram->bind();

	m_cube.initialize(*m_cubeProgram);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Exercise41::drawEnvironment(float x0, float y0, float x1, float y1, const QMatrix4x4 & globalTransform, const Camera &camera)
{
	const float dy = (y1 - y0) * 0.5;

	QMatrix4x4 horizontalTranslation;
	horizontalTranslation.translate(x0, y0, 0.f);

	QMatrix4x4 verticalTranslation;
	for (int i = 0; i < 3; ++i)
	{
		QMatrix4x4 rotation;

		rotation.rotate(m_startAngle[0], 1.0f, 0.0f, 0.0f);
		rotation.rotate(m_startAngle[1], 0.0f, 1.0f, 0.0f);
		rotation.rotate(m_startAngle[2], 0.0f, 0.0f, 1.0f);

		drawUnitBase(globalTransform * verticalTranslation * horizontalTranslation * rotation);

		verticalTranslation.translate(0.0f, dy, 0.0f);
	}

	verticalTranslation.setToIdentity();
	horizontalTranslation.setToIdentity();

	horizontalTranslation.translate(x1, y0, 0.0f);

	for (int i = 0; i < 3; ++i)
	{
		QMatrix4x4 rotation;

		rotation.rotate(m_endAngle[0], 1.0f, 0.0f, 0.0f);
		rotation.rotate(m_endAngle[1], 0.0f, 1.0f, 0.0f);
		rotation.rotate(m_endAngle[2], 0.0f, 0.0f, 1.0f);

		drawUnitBase(globalTransform * verticalTranslation * horizontalTranslation * rotation);

		verticalTranslation.translate(0.0f, dy, 0.0f);
	}
}

void Exercise41::drawUnitBase(const QMatrix4x4 &globalTransform)
{
	static const GLfloat s(0.01f);
	static const GLfloat l(0.5f);

	m_cubeProgram->bind();
	m_cubeProgram->setUniformValue("lightsource", QVector3D(0.0, 10.0, 4.0));
	m_cubeProgram->setUniformValue("viewprojection", m_camera->viewProjection());

	QMatrix4x4 transform;

	// x
	transform.setToIdentity();
	transform.scale(l, s, s);
	transform.translate(1.0, 0.0, 0.0);
	m_cubeProgram->setUniformValue("transform", globalTransform * transform);
	m_cubeProgram->setUniformValue("normalMatrix", (m_camera->view() * globalTransform * transform).normalMatrix());
	m_cubeProgram->setUniformValue("color", QVector4D(1.0, 0.0, 0.0, 1.0));
	m_cube.draw(*this);

	// y
	transform.setToIdentity();
	transform.scale(s, l, s);
	transform.translate(0.0, 1.0, 0.0);
	m_cubeProgram->setUniformValue("transform", globalTransform * transform);
	m_cubeProgram->setUniformValue("normalMatrix", (m_camera->view() * globalTransform * transform).normalMatrix());
	m_cubeProgram->setUniformValue("color", QVector4D(0.0, 1.0, 0.0, 1.0));
	m_cube.draw(*this);

	// z
	transform.setToIdentity();
	transform.scale(s, s, l);
	transform.translate(0.0, 0.0, 1.0);
	m_cubeProgram->setUniformValue("transform", globalTransform * transform);
	m_cubeProgram->setUniformValue("normalMatrix", (m_camera->view() * globalTransform * transform).normalMatrix());
	m_cubeProgram->setUniformValue("color", QVector4D(0.0, 0.0, 1.0, 1.0));
	m_cube.draw(*this);
}

void Exercise41::update()
{
	m_frame = ++m_frame % 360;
}

void Exercise41::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	static QMatrix4x4 globalScale;
	if (globalScale.isIdentity())
		globalScale.scale(0.6f);

	m_program->bind();
	m_program->setUniformValue("lightsource", QVector3D(0.0, 10.0, 4.0));
	m_program->setUniformValue("viewprojection", m_camera->viewProjection());

	static const float tx[2] = { -4.0f, +4.0f };
	static const float ty[2] = { +2.0f, -2.0f };
	static const float dy = (ty[1] - ty[0]) * 0.5f;

	drawEnvironment(tx[0], ty[0], tx[1], ty[1], globalScale, *m_camera);

	float t = static_cast<float>(m_frame) / 360.f;
	float x = (tx[1] - tx[0]) * t + tx[0];

	QMatrix4x4 translation;
	for (int i = 0; i < 3; ++i)
	{
		translation.setToIdentity();

		float y = ty[0] + dy * i;
		translation.translate(x, y, 0.0f);

		QMatrix4x4 rotation;
		switch (i)
		{
		case 0:
			rotation = interpolateMatrix(t);
			break;
		case 1:
			rotation = interpolateEuler(t);
			break;
		case 2:
			rotation = interpolateQuaternion(t);
			break;
		default:
			break;
		}

		QMatrix4x4 transform = globalScale * translation * rotation;

		QMatrix4x4 modelScale;
		modelScale.scale(0.7f);

		m_program->bind();
		m_program->setUniformValue("transform", transform * modelScale);
		m_program->setUniformValue("normalMatrix", (m_camera->view() * transform * modelScale).normalMatrix());
		m_program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

		m_drawable->draw(*this);

		glEnable(GL_DEPTH_TEST);
		drawUnitBase(transform);
	}
}

QMatrix4x4 Exercise41::interpolateEuler(float t)
{
	// TODO: Interpolate rotations by interpolating between the euler angles (by using function lerp)
	QMatrix4x4 matrix;

	float a = lerp(m_startAngle.x(), m_endAngle.x(), t);
	float b = lerp(m_startAngle.y(), m_endAngle.y(), t);
	float c = lerp(m_startAngle.z(), m_endAngle.z(), t);

	QQuaternion rotation = QQuaternion::fromEulerAngles(a, b, c);
	matrix.rotate(rotation);
	return matrix;
}

QMatrix4x4 Exercise41::interpolateQuaternion(float t)
{

	// TODO: Implement a spherical interpolation based on quaternions (use function slerp here)
	QVector4D start, end;
	start = QQuaternion::fromEulerAngles(m_startAngle).toVector4D();
	end = QQuaternion::fromEulerAngles(m_endAngle).toVector4D();

	QQuaternion rotation;

	rotation = static_cast<QQuaternion> (slerp(start, end, t));
	QMatrix3x3 rotate = rotation.toRotationMatrix();

	return QMatrix4x4(rotate);
}

QMatrix4x4 Exercise41::interpolateMatrix(const float t)
{
	// TODO: Implement a simple interpolation between start and end angle

	QMatrix3x3 start, end, rotate;
	start = QQuaternion::fromEulerAngles(m_startAngle).toRotationMatrix();
	end = QQuaternion::fromEulerAngles(m_endAngle).toRotationMatrix();

	//rotate = start*(1-t) - end*t;
	rotate = start + t* (end - start);

	QMatrix4x4 matrix(rotate);
	//matrix.rotate(rotate);

	return matrix;
}

QVector4D Exercise41::slerp(const QVector4D &a, const QVector4D &b, float t)
{
	// TODO: Implement a spherical linear interpolation between a and b
	QVector4D start = a;
	QVector4D end = b;
	start.normalize();
	end.normalize();

	double dot = QVector4D::dotProduct(start, end);

	if (dot < 0.0f) {
		start = -start;
		dot = -dot;
	}

	const double DOT_THRESHOLD = 0.9995;
	if (dot > DOT_THRESHOLD) {
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.

		QVector4D result = start + t*(end - start);
		result.normalize();
		return result;
	}

	if (dot < -1) dot = -1;
	else if (dot > 1) dot = 1;

	double theta_0 = acos(dot);  // theta_0 = angle between input vectors
	double theta = theta_0*t;    // theta = angle between v0 and result 

	QVector4D temp = end - start*dot;
	temp.normalize();

	return start*cos(theta) + temp*sin(theta);
}

float Exercise41::lerp(float a, float b, float t)
{
	assert(0 <= t && t <= 1);

	// TODO: Implement a simple linear interpolation between a and b
	return a + t * (b - a);

}

bool Exercise41::onKeyPressed(QKeyEvent* keyEvent)
{
	int i = 0;

	bool changed = false;

	switch (keyEvent->key())
	{
	case Qt::Key_Z:
		++i;
	case Qt::Key_Y:
		++i;
	case Qt::Key_X:
		m_startAngle[i] += (keyEvent->modifiers() & Qt::SHIFT) ? 1.f : -1.f;
		update();

		changed = true;

		break;
	case Qt::Key_R:
		m_startAngle[0] = 0.0f;
		m_startAngle[1] = 0.0f;
		m_startAngle[2] = 0.0f;
		update();

		changed = true;

		break;
	default:
		break;
	}

	return changed || AbstractExercise::onKeyPressed(keyEvent);
}

const QString Exercise41::hints() const
{
	return "Press [SPACE] to pause/resume animation. Use X, Y, and Z keys to modify orientation at start.";
}

int main(int argc, char *argv[])
{
	// Create application
	QGuiApplication app(argc, argv);

	// Create main window
	GLViewer viewer(new Exercise41);
	viewer.show();

	// Run application
	return app.exec();
}
