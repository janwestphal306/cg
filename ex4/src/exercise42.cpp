//Note: Remember that velocity = position.vector + direction.vector

//Core system includes
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
#include <QTime>
#include <qmath.h>

//Framework includes
#include "util/camera.h"
#include "util/glviewer.h"
#include "util/abstractexercise.h"
#include "util/movingunitsphere.h"
#include "util/unitcube.h"

class Exercise52 : public AbstractExercise
{
public:
    Exercise52();
    ~Exercise52() override;

    const QString hints() const override;
    bool initialize() override;
    void render() override;

protected:
    QScopedPointer<QOpenGLShaderProgram> m_program;
    std::vector<MovingUnitSphere*> m_sphereList;
    UnitCube m_plane;
    QRectF m_boundingBox;

private:
    void createSpheres(const int count);
    int randInt(int low, int high);
    void computeBoundingBox();
    void updateCamera();

    const float PLANEBOUNDS = 50.0;
};

Exercise52::Exercise52()
{

}

Exercise52::~Exercise52()
{
    for (const auto &cube : m_sphereList)
    {
        delete cube;
    }

    m_sphereList.clear();
}

const QString Exercise52::hints() const
{
    return QString("Press [SPACE] to pause or resume animation. Press [C] for switching the camera.");
}

bool Exercise52::initialize()
{
    AbstractExercise::initialize();

    if (m_fromTop)
    {
        m_camera->setEye(QVector3D(0.0f, 90.0f, 0.0f));
        m_camera->setUp(QVector3D(0.0f, 0.0f, -1.0f));
    }
    else
    {
        m_camera->setEye(QVector3D(80.0f, 80.0f, 80.0f));
        m_camera->setUp(QVector3D(0.0f, 1.0f, 0.0f));
    }

    m_camera->setCenter(QVector3D(0.0f, 1.0f, 0.0f));
    m_camera->setFovy(60.0);

    m_program.reset(createBasicShaderProgram("data/sphere.vert", "data/sphere.frag"));
    m_program->bind();

    //Create GroundPlane
    m_plane.initialize(*m_program);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    createSpheres(5);

    for (const auto &cube : m_sphereList)
    {
        cube->initialize(*m_program);
    }

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return true;
}

int Exercise52::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void Exercise52::createSpheres(const int count)
{
    for (int i = 0; i < count; i++)
    {
        float x = randInt(-PLANEBOUNDS + 5, PLANEBOUNDS - 5);
        float y = randInt(-PLANEBOUNDS + 5, PLANEBOUNDS - 5);
        QVector2D pos(x, y);

        float dirX = randInt(-180, 180) / 180.0;
        float dirY = randInt(-180, 180) / 180.0;
        QVector2D dir(dirX, dirY);
        dir.normalize();

        float r = randInt(0, 164) / 255.0f;
        float g = randInt(0, 164) / 255.0f;
        float b = randInt(0, 164) / 255.0f;
        QVector4D color(r, g, b, 1.0);

        float speed = randInt(1, 20.0) / 20.0;

        MovingUnitSphere* sphere = new MovingUnitSphere(pos,dir, color, speed, i);

        m_sphereList.push_back(sphere);
    }
}

void Exercise52::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->setUniformValue("lightsource", QVector3D(0.0f, 60.0f, 0.0f));
    m_program->setUniformValue("viewprojection", camera()->viewProjection());

    QMatrix4x4 planeTransform;
    planeTransform.translate(0.0, 0.0, 0.0);
    planeTransform.scale(PLANEBOUNDS, 0.001, PLANEBOUNDS);

    m_program->setUniformValue("transform", planeTransform);
    m_program->setUniformValue("normalMatrix", (camera()->view() * planeTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1,1,1,1.0));

    m_plane.draw(*this);

    if (m_fromTop)
    {
        m_camera->setEye(QVector3D(0.0f, 90.0f, 0.0f));
        m_camera->setUp(QVector3D(0.0f, 0.0f, -1.0f));
    }
    else
    {
        m_camera->setEye(QVector3D(80.0f, 80.0f, 80.0f));
        m_camera->setUp(QVector3D(0.0f, 1.0f, 0.0f));
    }

    computeBoundingBox();
    updateCamera();

    for (const auto &sphere : m_sphereList)
    {
        QVector2D pos = sphere->getPos() + sphere->GetSpeed() * sphere->GetDirection();
        QMatrix4x4  sphereTransform;

        sphereTransform.translate(pos.x(), 1.0, pos.y());
        sphere->UpdatePositions(pos.x(), pos.y());

        if (sphere->GetPositionX() >= PLANEBOUNDS - 1)
        {
            sphere->SetPosition(QVector2D(PLANEBOUNDS - 1, sphere->GetPositionY()));
            sphere->SetDirectionX(-1);
        }
        if (sphere->GetPositionX() <= -PLANEBOUNDS + 1)
        {
            sphere->SetPosition(QVector2D(-PLANEBOUNDS + 1, sphere->GetPositionY()));
            sphere->SetDirectionX(-1);
        }

        if (sphere->GetPositionY() >= PLANEBOUNDS - 1)
        {
            sphere->SetPosition(QVector2D(sphere->GetPositionX(), PLANEBOUNDS - 1));
            sphere->SetDirectionY(-1);
        }
        if (sphere->GetPositionY() <= -PLANEBOUNDS + 1)
        {
            sphere->SetPosition(QVector2D(sphere->GetPositionX(), -PLANEBOUNDS + 1));
            sphere->SetDirectionY(-1);
        }

        m_program->setUniformValue("transform", sphereTransform);
        m_program->setUniformValue("normalMatrix", (camera()->view() * sphereTransform).normalMatrix());
        m_program->setUniformValue("color", sphere->getColor());

        sphere->draw(*this);
    }
}

void Exercise52::computeBoundingBox()
{
    //TODO: Compute the Axis-Aligned Bounding Box for the moving elements and replace the QRectF values below
	float minX = PLANEBOUNDS, minY = PLANEBOUNDS, maxX = -PLANEBOUNDS, maxY = -PLANEBOUNDS;
	for (const auto &sphere : m_sphereList) {
		if (sphere->GetPositionX() <= minX) {
			minX = sphere->GetPositionX();
		}
		if (sphere->GetPositionX() >= maxX) {
			maxX = sphere->GetPositionX();
		}
		if (sphere->GetPositionY() <= minY) {
			minY = sphere->GetPositionY();
		}
		if (sphere->GetPositionY() >= maxY) {
			maxY = sphere->GetPositionY();
		}
	}
	minX -= 5; minY -= 5; maxX += 5; maxY += 5;
    m_boundingBox = QRectF(minX, minY, maxX-minX, maxY-minY);
}

void Exercise52::updateCamera()
{
    float adjustment = 1.0;
	QPointF center = m_boundingBox.center();
	m_camera->setCenter(QVector3D(center.x(), 0.0f, center.y()));
	
	float oneRad = 180.0 / M_PI;
    if (m_fromTop)
    {
        //TODO: Adjust the camera parameters for camera pos 1 (top-view)
		float camDistance = 90.0;
		float radLeft = atan(m_boundingBox.left() / camDistance);
		float radRight = atan(m_boundingBox.right() / camDistance);
		float radTop = atan(m_boundingBox.top() / camDistance);
		float radBottom = atan(m_boundingBox.bottom() / camDistance);
		
		float fovX = abs(radLeft - radRight);
		float fovY = abs(radTop - radBottom);

		float fov = std::max(fovX, fovY) * oneRad;
		m_camera->setFovy(fov);
    }
    else
    {
        //TODO: Adjust the camera parameters for camera pos 2 (front-right-view)
		const QVector3D cameraPos(80.0, 80.0, 80.0);

		QVector3D cameraToTopLeft(QVector3D(m_boundingBox.left(), 0.0, m_boundingBox.top()) - cameraPos);
		QVector3D cameraToBottomRight(QVector3D(m_boundingBox.right(), 0.0, m_boundingBox.bottom()) - cameraPos);

		float radX = acos(QVector3D::dotProduct(cameraToTopLeft, cameraToBottomRight) / (cameraToTopLeft.length() * cameraToBottomRight.length()));
		float fovX = abs(radX) * oneRad;

		QVector3D cameraToBottomLeft(QVector3D(m_boundingBox.left(), 0.0, m_boundingBox.bottom()) - cameraPos);
		QVector3D cameraToTopRight(QVector3D(m_boundingBox.right(), 0.0, m_boundingBox.top()) - cameraPos);

		float radY = acos(QVector3D::dotProduct(cameraToBottomLeft, cameraToTopRight) / (cameraToBottomLeft.length() * cameraToTopRight.length()));
		float fovY = abs(radY) * oneRad;

		float fov = std::max(fovX, fovY);
		m_camera->setFovy(fov);
    }
}

int main(int argc, char *argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Create main window
    GLViewer viewer(new Exercise52);
    viewer.show();

    // Run application
    return app.exec();
}
