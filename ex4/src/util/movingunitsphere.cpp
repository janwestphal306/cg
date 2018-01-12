#include "movingunitsphere.h"

#include <QVector2D>

MovingUnitSphere::MovingUnitSphere() : m_direction(QVector2D()), m_pos(QVector2D()), m_color(QVector4D()), m_speed(0.01), m_weight(0.0)
{
    //Direction * speed
}


MovingUnitSphere::MovingUnitSphere(const QVector2D &pos, const QVector2D &dir, const QVector4D &color, float speed, float weight) : m_direction(dir), m_pos(pos), m_color(color), m_speed(speed), m_weight(weight)
{

}

MovingUnitSphere::~MovingUnitSphere()
{
}

QVector2D MovingUnitSphere::getPos() const
{
	return m_pos;
}

QVector4D MovingUnitSphere::getColor() const
{
	return m_color;
}
