#include <qmath.h>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QRectF>
#include <QSettings>
#include <QScopedPointer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QList>
#include <QVector3D>
#include <QTime>
#include <QApplication>
#include <QMainWindow>

#include "util/camera.h"
#include "util/polygonaldrawable.h"
#include "util/objio.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"

#include <iostream>

enum class TranslationMode
{
	ConstantTime,
	ConstantSpeed
};

class Exercise32 : public AbstractExercise
{
public:
    Exercise32();
    ~Exercise32() override;

    void render() override;
    const QString hints() const override;

    bool onKeyPressed(QKeyEvent* keyEvent) override;
    bool initialize() override;

protected:
    int m_animationLength;
    PolygonalDrawable * m_drawable;
    PolygonalDrawable * m_pathDrawable;
    QTime m_time;
    int m_elapsed;
    QVector<QVector3D> m_path;
    qreal m_pathLength;
    TranslationMode m_translationMode;

    QScopedPointer<QOpenGLShaderProgram> m_modelProgram;
    QScopedPointer<QOpenGLShaderProgram> m_pathProgram;

    QMatrix4x4 computeTransformationMatrix(int currentFrame, int maxFrame);
};

Exercise32::Exercise32()
	: m_animationLength(10000)
	, m_drawable(nullptr)
	, m_pathDrawable(nullptr)
	, m_elapsed(0)
	, m_translationMode(TranslationMode::ConstantTime)
	, m_pathLength(0.0)
{
    m_path
        << 5 * QVector3D(-1.0f, 0.0f, -1.0f)
        << 5 * QVector3D(-0.7f, 0.0f, 0.7f)
        << 5 * QVector3D(0.5f, 0.0f, 0.8f)
        << 5 * QVector3D(0.6f, 0.0f, 0.9f)
        << 5 * QVector3D(0.7f, 0.0f, 0.85f)
        << 5 * QVector3D(0.65f, 0.0f, 0.65f)
        << 5 * QVector3D(0.6f, 0.0f, -0.3f)
        ;

    for (auto it = m_path.begin(); it+1 != m_path.end(); ++it)
    {
        m_pathLength += QVector3D(*(it+1) - *it).length();
    }
}

Exercise32::~Exercise32()
{
    delete m_drawable;
}

bool Exercise32::initialize()
{
    AbstractExercise::initialize();

    if (m_drawable)
        return true;

    m_modelProgram.reset(createBasicShaderProgram("data/model.vert", "data/model.frag"));
    m_modelProgram->bind();

    m_drawable = ObjIO::fromObjFile("data/suzanne.obj");

    m_drawable->createVAO(m_modelProgram.data());

    m_pathProgram.reset(createBasicShaderProgram("data/path.vert", "data/path.frag"));
    m_pathProgram->bind();

    m_pathDrawable = new PolygonalDrawable();
    m_pathDrawable->setMode(GL_LINE_STRIP);
    m_pathDrawable->vertices() = m_path;
    m_pathDrawable->createVAO(m_pathProgram.data());

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_time.start();

    return true;
}

void Exercise32::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    static QMatrix4x4 globalScale;
    if(globalScale.isIdentity())
        globalScale.scale(0.6f);

    static QMatrix4x4 modelScale;
    if(modelScale.isIdentity())
        modelScale.scale(0.7f);

    m_modelProgram->bind();
    m_modelProgram->setUniformValue("lightsource", QVector3D(0.0f, 10.0f, 4.0f));
    m_modelProgram->setUniformValue("viewprojection", camera()->viewProjection());

    if (m_active)
    {
        m_elapsed = (m_elapsed + m_time.elapsed()) % m_animationLength;
    }

    m_time.start();

    QMatrix4x4 transform = computeTransformationMatrix(m_elapsed, m_animationLength);

    m_modelProgram->bind();
    m_modelProgram->setUniformValue("transform", globalScale * transform * modelScale);
    m_modelProgram->setUniformValue("normalMatrix", (camera()->view() * globalScale * transform * modelScale).normalMatrix());
    m_modelProgram->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

    m_drawable->draw(*this);

    m_pathProgram->bind();
    m_pathProgram->setUniformValue("viewprojection", camera()->viewProjection());
    m_pathProgram->setUniformValue("transform", globalScale);
    m_pathProgram->setUniformValue("color", QVector4D(0.3f, 0.3f, 0.3f, 1.0f));

    m_pathDrawable->draw(*this);
}

bool Exercise32::onKeyPressed(QKeyEvent* keyEvent)
{
    bool changed = false;

	if (keyEvent->key() == Qt::Key_T)
    {
        m_translationMode = m_translationMode == TranslationMode::ConstantTime ? TranslationMode::ConstantSpeed : TranslationMode::ConstantTime;
        changed = true;
    }

    return changed || AbstractExercise::onKeyPressed(keyEvent);
}

QMatrix4x4 Exercise32::computeTransformationMatrix(int currentFrame, int maxFrame)
{
	// TODO: Calculate manipulation matrix here.
	QMatrix4x4 transform;
	if (m_translationMode == TranslationMode::ConstantTime) {
		// static int currentControlPoint = -1;
		
		float currentFramePercent = float(currentFrame) / maxFrame;
		int routes = m_path.size() - 1;
		
		int currentControlPoint = currentFramePercent * routes;
		/*7
		int nextControlPoint = currentFramePercent * routes;
		if (nextControlPoint != currentControlPoint) {
			currentControlPoint = nextControlPoint;
			std::cout << currentFrame << " " << maxFrame << " " << currentControlPoint << " " << m_path.at(currentControlPoint).x() << " " << m_path.at(currentControlPoint).y() << " " << m_path.at(currentControlPoint).z() << std::endl;
		}
		*/
		QVector3D fromPoint(m_path.at(currentControlPoint));
		QVector3D toPoint(m_path.at(currentControlPoint + 1));
		QVector3D directionToNextControlNode(toPoint - fromPoint);

		int framesPerRoute = maxFrame / routes;
		int frameInCurrentRoute = currentFrame % (framesPerRoute + 1);
		float traveledRoutePercent = float(frameInCurrentRoute) / framesPerRoute;

		QVector3D stepOnRoute = traveledRoutePercent * directionToNextControlNode;
		transform.translate(fromPoint + stepOnRoute);

		QQuaternion rotation = QQuaternion::fromDirection(directionToNextControlNode, QVector3D(0, 1, 0));
		transform.rotate(rotation);
	}
	else if (m_translationMode == TranslationMode::ConstantSpeed) {
		float framesPerLength = float(maxFrame) / m_pathLength;

		float lengthPerFrame = float(m_pathLength) / maxFrame;
		float passedLength = currentFrame * lengthPerFrame;

		float lengthRoutes = 0.0f;
		int currentControlPoint = 0;
		for (; currentControlPoint < m_path.size()-1; currentControlPoint++) {
			float lengthRoute = m_path.at(currentControlPoint).distanceToPoint(m_path.at(currentControlPoint + 1));
			if (lengthRoutes + lengthRoute > passedLength) break;
			lengthRoutes += lengthRoute;
		}

		QVector3D fromPoint(m_path.at(currentControlPoint));
		QVector3D toPoint(m_path.at(currentControlPoint + 1));
		QVector3D directionToNextControlNode(toPoint - fromPoint);

		float lengthPassedOnCurrentRoute = passedLength - lengthRoutes;
		float newlengthPassedOnCurrentRoute = lengthPerFrame + lengthPassedOnCurrentRoute;
		QVector3D stepOnRoute = directionToNextControlNode.normalized() * newlengthPassedOnCurrentRoute;
		transform.translate(fromPoint + stepOnRoute);

		QQuaternion rotation = QQuaternion::fromDirection(directionToNextControlNode, QVector3D(0, 1, 0));
		transform.rotate(rotation);
	}
	return transform;
}

const QString Exercise32::hints() const
{
    return "Press [SPACE] to pause/resume animation. Use key T to modify the type of rotation and translation interpolation.";
}

int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
    GLViewer viewer(new Exercise32);
	viewer.show();

    // Run application
    return app.exec();
}
