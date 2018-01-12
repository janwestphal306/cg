#pragma once

#include "util/abstractexercise.h"

enum GlobalDeformationMode
{
    None,
    Pinch,
    Mold,
    Twist,
    Bend
};

class Box;

class Exercise43 : public AbstractExercise
{
public:
	Exercise43();
	virtual ~Exercise43();

    virtual const QString hints() const override;

protected:
    bool onKeyPressed(QKeyEvent *keyEvent) override;
    bool initialize() override;
	void update() override;
    void render() override;

    QScopedPointer<QOpenGLShaderProgram> m_program;
    QMatrix4x4  m_boxTransform;
    Box         *m_box;
    GlobalDeformationMode   m_globalDefMode;
    
	int m_animationFrame;
};
