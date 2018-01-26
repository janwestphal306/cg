#pragma once

#include <array>

#include <QMatrix4x4>

#include "util/abstractexercise.h"

class PolygonalDrawable;

class Exercise51 : public AbstractExercise
{
public:
    Exercise51();
    ~Exercise51() override;

protected:
	bool hasAnimation() override;
	bool initialize(Camera &camera) override;
    void render(const Camera &camera) override;
	bool onKeyPressed(QKeyEvent *event, Camera &camera) override;
	const QString hints() const override;

	void updateCone();


    QScopedPointer<QOpenGLShaderProgram> m_program;

	int m_lateralSurfaces;
	bool m_wireframeModeEnabled;
};
