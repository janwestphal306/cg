#include "polygonaldrawable.h"

#include <cassert>

#include <QOpenGLShaderProgram>

#include "openglfunctions.h"


PolygonalDrawable::PolygonalDrawable()
: m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
}

PolygonalDrawable::~PolygonalDrawable()
{
}

void PolygonalDrawable::draw(OpenGLFunctions & gl)
{
    m_vao.bind();

    gl.glEnable(GL_DEPTH_TEST);
    gl.glEnable(GL_CULL_FACE);

    if (m_indices.size())
    {
        gl.glDrawElements(m_mode, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
    }
    else
    {
        gl.glDrawArrays(m_mode, 0, m_vertices.size());
    }

    gl.glDisable(GL_DEPTH_TEST);
    gl.glDisable(GL_CULL_FACE);
}

const GLenum PolygonalDrawable::mode() const
{
    return m_mode;
}

void PolygonalDrawable::setMode(const GLenum mode)
{
    m_mode = mode;
}

const QVector<QVector3D> & PolygonalDrawable::vertices() const
{
    return m_vertices;
}

QVector<QVector3D> & PolygonalDrawable::vertices()
{
    return m_vertices;
}

const QVector<QVector3D> & PolygonalDrawable::normals() const
{
    return m_normals;
}

QVector<QVector3D> & PolygonalDrawable::normals()
{
    return m_normals;
}

const QVector<QVector2D> & PolygonalDrawable::texcs() const
{
    return m_texcs;
}

QVector<QVector2D> & PolygonalDrawable::texcs()
{
    return m_texcs;
}

const QVector<quint32> & PolygonalDrawable::indices() const
{
    return m_indices;
}

QVector<quint32> & PolygonalDrawable::indices()
{
    return m_indices;
}

void PolygonalDrawable::retrieveNormals()
{
    assert(m_indices.size() % 3 == 0);

	auto m = *std::max_element(m_indices.begin(), m_indices.end());

	if (m_normals.size() < static_cast<int>(m))
	{
		m_normals.resize(m + 1);
	}        

    for (int i = 0; i < m_indices.size(); i += 3)
    {
        auto i0 = m_indices[i + 0];
		auto i1 = m_indices[i + 1];
		auto i2 = m_indices[i + 2];

        auto a = (m_vertices[i2] - m_vertices[i0]).normalized();
		auto b = (m_vertices[i1] - m_vertices[i0]).normalized();

		auto n = a * b;

        m_normals[i0] = n;
        m_normals[i1] = n;
        m_normals[i2] = n;
    }
}

void PolygonalDrawable::createVAO(QOpenGLShaderProgram * program)
{
    m_vao.create();
    m_vao.bind();

    m_vertexBuffer = QOpenGLBuffer();
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.allocate(m_vertices.constData(), m_vertices.size() * sizeof(QVector3D));

    program->enableAttributeArray("in_vertex");
    program->setAttributeBuffer("in_vertex", GL_FLOAT, 0, 3, 3 * sizeof(float));

    if (m_normals.size())
    {
        m_normalBuffer = QOpenGLBuffer();
        m_normalBuffer.create();
        m_normalBuffer.bind();
        m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_normalBuffer.allocate(m_normals.constData(), m_normals.size() * sizeof(QVector3D));

        program->enableAttributeArray("in_normal");
        program->setAttributeBuffer("in_normal", GL_FLOAT, 0, 3, 3 * sizeof(float));
    }

    if (m_texcoordBuffer.size())
    {
        m_texcoordBuffer.create();
        m_texcoordBuffer.bind();
        m_texcoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_texcoordBuffer.allocate(m_texcs.constData(), m_texcs.size() * sizeof(QVector2D));

        program->enableAttributeArray("in_texcoord");
        program->setAttributeBuffer("in_texcoord", GL_FLOAT, 0, 2, 2 * sizeof(float));
    }

    if (m_indices.size())
    {
        m_indexBuffer.create();
        m_indexBuffer.bind();
        m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_indexBuffer.allocate(m_indices.constData(), m_indices.size() * sizeof(unsigned int));
    }
}
