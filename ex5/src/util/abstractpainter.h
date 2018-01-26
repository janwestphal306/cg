#pragma once

#include <QList>
#include <QSize>
#include <QKeyEvent>

#include "openglfunctions.h"
#include <QOpenGLShader>

class QStringList;
class QOpenGLShaderProgram;

class Camera;
class AbstractSceneObject;

class AbstractPainter : public OpenGLFunctions
{
public:
    /** this should setup the OpenGL state and allocate all required static 
        resources (context is 3.2 core for this one).
    */
	virtual bool initialize(Camera &camera);


    virtual void render(const Camera &camera) = 0;
    virtual void resize(int width, int height) = 0;

    virtual void update();

	virtual bool onKeyPressed(QKeyEvent *event, Camera &camera);
protected:
    QList<QOpenGLShader*> m_shaders;

    GLuint getOrCreateTexture(const QString & fileName);
	void addShader(QOpenGLShader::ShaderType shaderType, const QString &shaderFilename, QOpenGLShaderProgram &program);
    QOpenGLShaderProgram* createBasicShaderProgram(const QString &vertexShaderFileName, const QString &fragmentShaderFileName);
	QOpenGLShaderProgram* createBasicShaderProgram(const QString &vertexShaderFileName, const QString &geometricShaderShaderFileName, const QString &fragmentShaderFileName);
};
