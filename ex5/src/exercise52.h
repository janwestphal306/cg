#pragma once

#include <array>

#include <QMatrix4x4>

#include "util/abstractexercise.h"

class PolygonalDrawable;

class Exercise52 : public AbstractExercise
{
public:
    Exercise52();
    ~Exercise52() override;

protected:
	bool hasAnimation() override;
    bool initialize(Camera &camera) override;
	void update() override;
    void render(const Camera &camera) override;
	const QString hints() const override;

    QScopedPointer<QOpenGLShaderProgram> m_program;
    PolygonalDrawable* m_drawable;
	int m_animationFrame;
};
