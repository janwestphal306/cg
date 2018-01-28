#include "exercise51.h"

#include <cassert>

#include <QGuiApplication>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QtMath>

#include "util/camera.h"
#include "util/glviewer.h"
#include "util/polygonaldrawable.h"
#include "util/objio.h"

#define SURFACES (2*m_lateralSurfaces)

struct Vertex
{
	GLfloat x, y, z;
};

struct Surface
{
	Vertex vertices[3];
};

Exercise51::Exercise51()
	: AbstractExercise()
	, m_lateralSurfaces(16)
	, m_wireframeModeEnabled(false)
{
}

Exercise51::~Exercise51()
{
}

bool Exercise51::hasAnimation()
{
	return false;
}

GLuint vao;
GLuint vbo[2];

bool Exercise51::initialize(Camera &camera)
{
	AbstractExercise::initialize(camera);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	camera.setCenter(QVector3D(0.0f, 1.0f, 0.0f));
	camera.setEye(QVector3D(0.0f, 2.0f, 6.0f));

	m_program.reset(createBasicShaderProgram("data/cone.vert", "data/cone.frag"));
	m_program->bind();

	//TODO: Create VAO & VBOs
	// Die VBOs werden jedesmal in updateCone erstellt und gebunden,
	// da andernfalls ab 45 Flächen zu Artefakten kommt.
	glGenVertexArrays(1, &vao);

	updateCone();

	return true;
}

void Exercise51::render(const Camera &camera)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	m_program->setUniformValue("color", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
	m_program->setUniformValue("normalMatrix", camera.view().normalMatrix());
	m_program->setUniformValue("lightsource", QVector3D(0.0, 10.0, 4.0));
	m_program->bind();

	m_program->setUniformValue("viewprojection", camera.viewProjection());

	if (m_wireframeModeEnabled)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// TODO: Add call to glDrawArrays
	// qDebug() << "render" << sizeof(surfaces);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, SURFACES * 9);

	// aufräumen
	// glDeleteVertexArrays(1, &vao);
	glBindVertexArray(0);
	glUseProgram(0);

	if (m_wireframeModeEnabled)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}

void setVertex(Vertex *vertex, GLfloat x, GLfloat y, GLfloat z) {
	vertex->x = x;
	vertex->y = y;
	vertex->z = z;
}

void setSurface(Surface *surface, Vertex *vertex0, Vertex *vertex1, Vertex *vertex2) {
	surface->vertices[0] = *vertex0;
	surface->vertices[1] = *vertex1;
	surface->vertices[2] = *vertex2;
}

void Exercise51::updateCone()
{
	qDebug() << "Lateral surfaces: " << m_lateralSurfaces;

	assert(m_lateralSurfaces >= 3);

	//TODO: Update buffer content
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	Surface *surfaces = (Surface*)calloc(SURFACES, sizeof(Surface));
	Surface *normales = (Surface*)calloc(SURFACES, sizeof(Surface));

	GLfloat r = 1.0f, h = 2.0f;
	Vertex startVertex = Vertex(), top = Vertex(), center = Vertex(), n_top = Vertex(), n_center = Vertex();

	setVertex(&startVertex, r, 0.0f, 0.0f);
	setVertex(&top, 0.0f, h, 0.0f);
	setVertex(&center, 0, 0, 0);
	setVertex(&n_top, 0, 1, 0);
	setVertex(&n_center, 0, -1, 0);

	Vertex lastVertex = startVertex;

	for (int i = 1; i <= m_lateralSurfaces; i++) {
		GLfloat ratio = i < m_lateralSurfaces ? GLfloat(i) / m_lateralSurfaces : 0;
		qreal rad = ratio * 2 * M_PI;
		GLfloat x = qCos(rad) * r, z = qSin(rad) * r;
		Vertex vertex = Vertex();
		setVertex(&vertex, x, 0.0f, z);

		Surface surface = Surface();
		// lateral
		setSurface(&surface, &lastVertex, &vertex, &top);
		surfaces[(i - 1) * 2] = surface;
		// bottom
		setSurface(&surface, &lastVertex, &center, &vertex);
		surfaces[(i - 1) * 2 + 1] = surface;
	
		QVector3D v1 = QVector3D(vertex.x - lastVertex.x, vertex.y - lastVertex.y, vertex.z - lastVertex.z);
		QVector3D v2 = QVector3D(vertex.x - top.x, vertex.y - top.y, vertex.z - top.z);
		QVector3D n = QVector3D::crossProduct(v1, v2).normalized();

		Vertex n_vertex = Vertex();
		setVertex(&n_vertex, n.x(), n.y(), n.z());
		// lateral
		setSurface(&surface, &n_vertex, &n_vertex, &n_vertex);
		normales[(i - 1) * 2] = surface;
		// bottom
		setSurface(&surface, &n_center, &n_center, &n_center);
		normales[(i - 1) * 2 + 1] = surface;

		lastVertex = vertex;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, SURFACES * sizeof(Surface), surfaces, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, SURFACES * sizeof(Surface), normales, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

bool Exercise51::onKeyPressed(QKeyEvent *event, Camera &camera)
{
	bool handled = false;

	if (!event->modifiers())
	{
		handled = true;

		switch (event->key())
		{
		case Qt::Key_W:
			m_wireframeModeEnabled = !m_wireframeModeEnabled;
			break;
		case '1':
			camera.setCenter(QVector3D(0.0f, 1.0f, 0.0f));
			camera.setEye(QVector3D(0.0f, 2.0f, 6.0f));
			break;
		case '2':
			camera.setCenter(QVector3D(0.0f, 1.0f, 0.0f));
			camera.setEye(QVector3D(0.0f, -5.0f, 0.1f));
			break;
		case Qt::Key_Plus:
			m_lateralSurfaces++;
			updateCone();
			break;
		case Qt::Key_Minus:
			if (m_lateralSurfaces > 3)
			{
				m_lateralSurfaces--;
				updateCone();
			}
			break;

		default:
			handled = false;
			break;
		}

		if (handled)
		{
			requestRedraw();
		}
	}

	return handled || AbstractExercise::onKeyPressed(event, camera);
}

const QString Exercise51::hints() const
{
	return "Keys:\n\t[+] / [-] : Increase/Decrease the number of lateral surfaces.\n\t[1] / [2] : Switch camera perspective.\n\t[w] : Toggle wireframe mode.";
}

int main(int argc, char *argv[])
{
	// Create application
	QGuiApplication app(argc, argv);

	// Create main window
	GLViewer viewer(new Exercise51);
	viewer.show();

	// Run application
	return app.exec();
}
