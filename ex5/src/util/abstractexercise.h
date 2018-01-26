#pragma once

#include "abstractpainter.h"

#include <QKeyEvent>
#include <QMatrix4x4>

class AbstractExercise : public AbstractPainter
{
public:
    AbstractExercise();

    virtual const QString hints() const;

    void resize(int width, int height) override;

	bool onKeyPressed(QKeyEvent *event, Camera &camera) override;

	bool redrawRequested();
	void requestRedraw();

	bool initialize(Camera &camera) override;

	virtual bool hasAnimation() = 0;
protected:
	bool m_redrawRequested;
	bool m_animationPaused;
};
