#pragma once

#include "util/abstractexercise.h"
#include "util/unitcube.h"

#include <QScopedPointer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class PolygonalDrawable;

class Exercise41 : public AbstractExercise
{
public:
	Exercise41();
	~Exercise41() override;

	void update() override;
	void render() override;

    const QString hints() const override;

protected:
    virtual bool onKeyPressed(QKeyEvent* keyEvent) override;

    virtual bool initialize() override;

    QMatrix4x4 interpolateEuler     (float t);
    QMatrix4x4 interpolateQuaternion(float t);
    QMatrix4x4 interpolateMatrix    (const float t);

	static QVector4D slerp(const QVector4D &a, const QVector4D &b, float t);
	static float lerp(float a, float b, float t);

	void drawEnvironment(float x0, float y0, float x1, float y1, const QMatrix4x4 & globalTransform, const Camera &camera);
	void drawUnitBase(const QMatrix4x4 &globalTransform);

	QVector3D m_startAngle;
	QVector3D m_endAngle;

    PolygonalDrawable * m_drawable;
    UnitCube m_cube;

    QScopedPointer<QOpenGLShaderProgram> m_program;
    QScopedPointer<QOpenGLShaderProgram> m_cubeProgram;

	int m_frame;
};
