#include "camera.h"

#include <cassert>

Camera::Camera(
    const QVector3D &eye
,   const QVector3D &center
,   const QVector3D &up)
	: m_eye(eye)
	, m_center(center)
	, m_up(up)

	, m_fovy(40.f)
	, m_aspect(1.f)
	, m_zNear(0.1f)
	, m_zFar(1024.0f)
{
}

const QVector3D & Camera::eye() const
{
    return m_eye;
}

void Camera::setEye(const QVector3D & eye)
{
    m_eye = eye;
}

const QVector3D & Camera::center() const
{
    return m_center;
}

void Camera::setCenter(const QVector3D & center)
{
	m_center = center;
}

const QVector3D & Camera::up() const
{
    return m_up;
}

void Camera::setUp(const QVector3D & up)
{
    m_up = up;
}

float Camera::zNear() const
{
    return m_zNear;
}

void Camera::setZNear(const float zNear)
{
    m_zNear = zNear;
    assert(m_zNear > 0.0);
}

float Camera::zFar() const
{
    return m_zFar;
}

void Camera::setZFar(const float zFar)
{
    m_zFar = zFar;
    assert(m_zFar > m_zNear);
}

float Camera::fovy() const
{
    return m_fovy;
}

void Camera::setFovy(const float fovy)
{
    m_fovy = fovy;
    assert(m_fovy > 0.0);
}

const QSize &Camera::viewport() const
{
    return m_viewport;
}

void Camera::setViewport(const QSize &viewport)
{
    m_aspect = viewport.width() / std::max(static_cast<float>(viewport.height()), 1.f);
    m_viewport = viewport;
}

const QMatrix4x4 Camera::view() const
{
	QMatrix4x4 viewMatrix;
	viewMatrix.lookAt(m_eye, m_center, m_up);

	return viewMatrix;
}

const QMatrix4x4 Camera::projection() const
{
	QMatrix4x4 projectionMatrix;
	projectionMatrix.perspective(m_fovy, m_aspect, m_zNear, m_zFar);

	return projectionMatrix;
}

const QMatrix4x4 Camera::viewProjection() const
{    
	return projection() * view();
}