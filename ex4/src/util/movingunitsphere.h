#pragma once

#include <QVector2D>
#include <QVector4D>

#include "icosahedron.h"

class MovingUnitSphere : public Icosahedron
{
public:
	MovingUnitSphere();
	MovingUnitSphere(const QVector2D &pos, const QVector2D &dir, const QVector4D &color, float speed, float weight = 0.0f);
	~MovingUnitSphere();

	QVector2D getPos() const;
	QVector4D getColor() const;

	inline QVector4D SetColor(float r, float g, float b)
	{
		m_color.setX(r);
		m_color.setY(g);
		m_color.setZ(b);
		m_color.setW(1);

		return m_color;
	}

    inline void UpdatePositions(float x, float y) {m_pos.setX(x); m_pos.setY(y);}

    inline QVector2D GetDirection(){return m_direction;}

     inline float GetDirectionX(){return m_direction.x();}
     inline float GetDirectionY(){return m_direction.y();}

	 inline QVector2D GetPosition() { return m_pos; }

     inline float GetPositionX(){return m_pos.x();}
     inline float GetPositionY(){return m_pos.y();}

     inline float GetWeight(){return m_weight;}
     inline float GetSpeed(){return m_speed;}

	 inline void SetDirection(QVector2D dir) { m_direction = dir; }
     inline void SetDirectionX(float dirX){ m_direction.setX(dirX * m_direction.x());}
     inline void SetDirectionY(float dirY){ m_direction.setY(dirY * m_direction.y());}

	 inline void SetPosition(QVector2D pos) { m_pos = pos; }
	 inline void SetPositionX(float posX) { m_pos.setX(posX); }
	 inline void SetPositionY(float posY) { m_pos.setY(posY); }

     inline void SetWeight(float weight){m_weight = weight;}

     inline void SetSpeed(float speed){m_speed = speed;}
     inline float GetSpeed(float speed){return m_speed;};

private:
	QVector4D m_color;
	QVector2D m_direction;
	QVector2D m_pos;
	float m_speed;
	float m_weight;
	
};
