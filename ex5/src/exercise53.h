#pragma once

#include <array>

#include <QMatrix4x4>
#include <QVector2D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "util/abstractexercise.h"

class Exercise53 : public AbstractExercise
{
public:
    Exercise53();
    ~Exercise53() override;

protected:
	bool hasAnimation() override;
	bool initialize(Camera &camera) override;
	void update() override;
    void render(const Camera &camera) override;

	const QString hints() const override;
	bool onKeyPressed(QKeyEvent *keyEvent, Camera &camera) override;

    QScopedPointer<QOpenGLShaderProgram> m_program;
	QOpenGLBuffer m_vertexBuffer;
	std::vector<QVector3D> m_vertices;
    QOpenGLBuffer m_texcoordBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vao;

	int m_animationFrame;
	bool m_wireframeModeEnabled;
	int m_maximumTessGenLevel;
};
