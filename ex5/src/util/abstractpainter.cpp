#include "abstractpainter.h"

#include <cassert>
#include <QFileInfo>
#include <QGuiApplication>
#include <QStringList>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

QString querys(GLenum penum)
{
    return reinterpret_cast<const char*>(glGetString(penum));
}

GLint queryi(GLenum penum)
{
    GLint result;
    glGetIntegerv(penum, &result);

    return result;
}

bool AbstractPainter::initialize(Camera &camera)
{
    initializeOpenGLFunctions();

	qDebug().noquote() << QString("GPU: %1 (%2, %3)").arg(querys(GL_RENDERER)).arg(querys(GL_VENDOR)).arg(querys(GL_VERSION));
	qDebug().noquote() << QString("GL Version: %1.%2 %3").arg(queryi(GL_MAJOR_VERSION)).arg(queryi(GL_MINOR_VERSION)).arg(queryi(GL_CONTEXT_CORE_PROFILE_BIT) ? "Core" : "Compatibility");

    return true;
}

void AbstractPainter::update()
{
}

bool AbstractPainter::onKeyPressed(QKeyEvent *event, Camera &camera)
{
    return false;
}

GLuint AbstractPainter::getOrCreateTexture(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull())
    {
        qDebug() << "Loading image from" << fileName << "failed.";
        return -1;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height()
        , 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    return texture;
}

void AbstractPainter::addShader(const QOpenGLShader::ShaderType shaderType, const QString &shaderFilename, QOpenGLShaderProgram &program)
{
	if(!program.addShaderFromSourceFile(shaderType, shaderFilename))
	{
		qDebug() << QObject::tr("Could not add shader %1").arg(QFileInfo(shaderFilename).absoluteFilePath());
		QCoreApplication::exit(1);
	}
}

QOpenGLShaderProgram* AbstractPainter::createBasicShaderProgram(const QString &vertexShaderFileName, const QString &fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();
    
	if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFileName))
	{
		qDebug() << QObject::tr("Could not add vertex shader %1").arg(QFileInfo(vertexShaderFileName).absoluteFilePath());
		QCoreApplication::exit(1);
	}
	if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFileName))
	{
		qDebug() << QObject::tr("Could not add fragment shader %1").arg(QFileInfo(fragmentShaderFileName).absoluteFilePath());
		QCoreApplication::exit(1);
	}

    return program;
}

QOpenGLShaderProgram* AbstractPainter::createBasicShaderProgram(const QString &vertexShaderFileName, const QString &geometricShaderShaderFileName, const QString &fragmentShaderFileName)
{
	auto program = createBasicShaderProgram(vertexShaderFileName, fragmentShaderFileName);

	if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry, geometricShaderShaderFileName))
	{
		qDebug() << QObject::tr("Could not add geometry shader %1").arg(QFileInfo(geometricShaderShaderFileName).absoluteFilePath());
		QCoreApplication::exit(1);
	}

	return program;
}
