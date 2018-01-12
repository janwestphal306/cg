#include "exercise43.h"

#include <QGuiApplication>

#include "util/box.h"
#include "util/glviewer.h"
#include "util/camera.h"

#include <iostream>

Exercise43::Exercise43()
    : AbstractExercise()
    , m_globalDefMode(None)
    , m_animationFrame(0)
{
    m_box = new Box(2.0f, 10, 4.0f, 20, 2.0f, 10);
}

Exercise43::~Exercise43()
{
    delete m_box;
}

const QString Exercise43::hints() const
{
    return "Use [1],[2],[3],[4],[5] to change global deformation.";
}

bool Exercise43::initialize()
{
    AbstractExercise::initialize();

    m_program.reset(createBasicShaderProgram("data/box.vert", "data/box.frag"));
    m_program->bind();

    m_box->initialize(*m_program);
    m_boxTransform = m_box->getTransform();
	
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return true;
}

void Exercise43::update()
{
    m_animationFrame++;
    if (m_animationFrame > 200)
    {
        m_animationFrame = 0;
    }
}

void Exercise43::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->setUniformValue("lightsource", QVector3D(0.0, 10.0, 0.0));
    m_program->setUniformValue("viewprojection", m_camera->viewProjection());

    m_program->setUniformValue("transform", m_boxTransform);
    m_program->setUniformValue("normalMatrix", (m_camera->view() * m_boxTransform).normalMatrix());
    m_program->setUniformValue("color", QVector4D(1.0, 0.0, 0.0, 1.0));
    m_program->setUniformValue("useLighting", true);

    auto overallDim = m_box->getOverallObjectDimensions();
    m_program->setUniformValue("overallObjectDimensions", QVector3D(overallDim[0], overallDim[1], overallDim[2]));
    m_program->setUniformValue("animationFrame", std::abs(m_animationFrame - 100) * 0.01f);
    m_program->setUniformValue("globalDeformationMode", m_globalDefMode);

    glLineWidth(1.0f);
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_box->draw(*this);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glLineWidth(3.0f);
    m_program->setUniformValue("useLighting", false);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_box->draw(*this);
}

bool Exercise43::onKeyPressed(QKeyEvent *keyEvent)
{
    GlobalDeformationMode newGlobalDefMode = m_globalDefMode;
    switch(keyEvent->key())
    {
        case Qt::Key_1:
            newGlobalDefMode = None;
            break;
        case Qt::Key_2:
            newGlobalDefMode = Pinch;
            break;
        case Qt::Key_3:
            newGlobalDefMode = Mold;
            break;
        case Qt::Key_4:
            newGlobalDefMode = Twist;
            break;
        case Qt::Key_5:
            newGlobalDefMode = Bend;
            break;
        default:
            break;
    }

    bool changed = newGlobalDefMode != m_globalDefMode;

    if (changed)
    {
        m_globalDefMode = newGlobalDefMode;
        m_animationFrame = 0;
    }

    return changed || AbstractExercise::onKeyPressed(keyEvent);
}

int main(int argc, char *argv[])
{
    // Create application
    QGuiApplication app(argc, argv);

    // Create main window
    GLViewer viewer(new Exercise43);
    viewer.show();

    // Run application
    return app.exec();
}
