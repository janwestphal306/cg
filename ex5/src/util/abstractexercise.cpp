#include "abstractexercise.h"

AbstractExercise::AbstractExercise()
	: m_redrawRequested(true)
	, m_animationPaused(false)
{
}

const QString AbstractExercise::hints() const
{
	return QString();
}


bool AbstractExercise::redrawRequested()
{
	auto redraw = m_redrawRequested || (hasAnimation() && !m_animationPaused);
	m_redrawRequested = false;
	return redraw;
}

void AbstractExercise::requestRedraw()
{
	m_redrawRequested = true;
}

void AbstractExercise::resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

bool AbstractExercise::onKeyPressed(QKeyEvent *keyEvent, Camera &camera)
{
    bool changed = false;
	if (keyEvent->key() == Qt::Key_Space && !keyEvent->modifiers())
    {
		m_animationPaused = !m_animationPaused;

        keyEvent->accept();
        changed = true;
    }

    return changed || AbstractPainter::onKeyPressed(keyEvent, camera);
}

bool AbstractExercise::initialize(Camera &camera)
{
	bool result = AbstractPainter::initialize(camera);

	QString usageHints = hints();
	if (!usageHints.isEmpty())
	{
		qDebug().noquote() << usageHints;
	}

	return result;
}
