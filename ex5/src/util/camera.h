#pragma once

#include <QSize>
#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:
    Camera(
        const QVector3D & eye    = QVector3D(0.0f, 0.0f, 1.0f)
    ,   const QVector3D & center = QVector3D(0.0f, 0.0f, 0.0f) 
    ,   const QVector3D & up     = QVector3D(0.0f, 1.0f, 0.0f));

    const QVector3D &eye() const;
    void setEye(const QVector3D &eye);
    const QVector3D &center() const;
    void setCenter(const QVector3D &center);
    const QVector3D &up() const;
    void setUp(const QVector3D &up);

    float zNear() const;
    void setZNear(float zNear);
    float zFar() const;
    void setZFar(float zFar);

    float fovy() const;
    void setFovy(float fovy);

    const QSize &viewport() const;
    void setViewport(const QSize & viewport);

    // lazy matrices getters

    const QMatrix4x4 view() const;
	const QMatrix4x4 projection() const;
	const QMatrix4x4 viewProjection() const;

    void update();

protected:
    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;

    float m_fovy;
    float m_aspect;
    float m_zNear;
    float m_zFar;
    QSize m_viewport;
};
