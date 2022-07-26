#include "glwidget.h"
#include "mainwindow.h"
#include "planets.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QVector3D>
#include <math.h>
#include <sys/time.h>
#include <ctime>
#include <stdlib.h>

#include <QTime>


#define M_PI_DIV180 0.01745329251994329576

auto StartTime = 0;
auto NowTime = 0;

bool DrawingPlanet = false;
float TempPlanetX = 0;
float TempPlanetY = 0;
float TempPlanetMass = 1;
float ScaleFactor = 1;

Planets Planet;
bool ConstrainInside = false;
bool Play = true;

using std::chrono::milliseconds;
using std::chrono::system_clock;

using std::chrono::duration_cast;

QVector3D perp(const QVector3D &v) {
    qreal min = abs(v.x());
    QVector3D cardinalAxis(1, 0, 0);

    if (abs(v.y()) < min) {
        min = abs(v.y());
        cardinalAxis = QVector3D(0, 1, 0);
    }

    if (abs(v.z()) < min) {
        cardinalAxis = QVector3D(0, 0, 1);
    }

    return QVector3D::crossProduct(v, cardinalAxis);
}



// note: qreal is a typedef of either float or double
void drawCone(const QVector3D &d, const QVector3D &a,
              const float h, const qreal rd, const int n) {
    QVector3D c = a + (-d * h);
    QVector3D e0 = perp(d);
    QVector3D e1 = QVector3D::crossProduct(e0, d);
    qreal angInc = 360.0 / n * M_PI_DIV180;

    // calculate points around directrix
    QVector<QVector3D> pts;
    for (int i = 0; i <= n; ++i) {
        qreal rad = angInc * i;
        QVector3D p = c + (((e0 * cos(rad)) + (e1 * sin(rad))) * rd);
        pts.push_back(p);
    }

    // draw cone top
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(a.x(), a.y(), a.z());
    for (int i = 0; i <= n; ++i) {
        glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
    }
    glEnd();

    // draw cone bottom
    //glBegin(GL_TRIANGLE_FAN);
    //glVertex3f(c.x(), c.y(), c.z());
    //for (int i = n-1; i >= 0; --i) {
    //    glVertex3f(pts[i].x(), pts[i].y(), pts[i].z());
    //}
    //glEnd();
}


void DrawPoints(vector<vector<float>> Points)
{
    glBegin(GL_POINTS);
    glColor3f(0,0,0);
    glPointSize(1000);
    for (int i = 0; i < Points.size(); i++)
    {
        glVertex2f(Points[i][0],Points[i][1]);
    }
    glEnd();
}



void DrawCones(vector<vector<float>> Points, qreal rd, float r, float g, float b)
{
    QVector3D d(0, 0, -1); //direction of the cone
    QVector3D a(0, 0, 0);//tip of the cone start
    qreal h = .5;
    qreal n = 10;
    for (int i = 0; i < Points.size(); i++)
    {
        a.setX(Points[i][0]);
        a.setY(Points[i][1]);
        glColor3f(r, g, b);
        drawCone(d, a, h, rd, n);

    }
}
void DrawCones(vector<float> xPos, vector<float> yPos, vector<float> rd)
{
    QVector3D d(0, 0, -1); //direction of the cone
    QVector3D a(0, 0, 0);//tip of the cone start
    qreal h = .5;
    qreal n = 20;
    for (unsigned int i = 0; i < xPos.size(); i++)
    {
        a.setX(xPos[i]* ScaleFactor) ;
        a.setY(yPos[i]* ScaleFactor);
        srand(i*1000);
        float r = float(rand()%1000)/1000;
        float g = float(rand()%1000)/1000;
        float b = float(rand()%1000)/1000;
        glColor3f(r, g, b);
        drawCone(d, a, h, rd[i]/100 * ScaleFactor, n);

    }
}

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    //Planet.AddPoint(0,.5,-1,0, 5);
    //Planet.AddPoint(0,0,0,0, 5);
    //Planet.AddPoint(0,-.5,.5,0, 10);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer.start();
}


void GLWidget::initializeGL()
{
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}
#include <thread>
using std::this_thread::sleep_for;
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width(), height());
    glLoadIdentity(); // reset transformation state
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0); // apply rotation on x
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0); // apply rotation on y
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0); // apply rotation on z
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_PROGRAM_POINT_SIZE);

    vector<float> xPos = Planet.xPositions;
    vector<float> yPos = Planet.yPositions;

    StartTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    vector<float> radius = Planet.Mass;
    if (DrawingPlanet)
    {
        xPos.push_back(TempPlanetX);
        yPos.push_back(TempPlanetY);
        radius.push_back(TempPlanetMass);
    }
    DrawCones(xPos, yPos, radius);
    if (Play)
        Planet.NextTimeInterval(.005, ConstrainInside);
    NowTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    while (NowTime - StartTime < 5)
    {
        //qDebug() << "Inside Loop Time" << NowTime - StartTime;
        sleep_for(.02ms);
        NowTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    qDebug() << "Time take is " <<  NowTime - StartTime;

}

void MainWindow::on_ConstrainInside_stateChanged(int arg1)
{
    if (ConstrainInside)
    {
        ConstrainInside = false;
    }else
    {
        ConstrainInside = true;
    }
}



void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    DrawingPlanet = true;
    lastPos = event->pos();
    TempPlanetX = (float(lastPos.x()-width())/width() * 2 + 1) / ScaleFactor;
    TempPlanetY = (-float(lastPos.y()-height())/height() * 2 - 1) / ScaleFactor ;

    TempPlanetMass = 5;
}



void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    if (DrawingPlanet)
    {
         if (numDegrees.y() < 0)  TempPlanetMass = TempPlanetMass/1.05;
         if (numDegrees.y() > 0)  TempPlanetMass = TempPlanetMass*1.05;
    }else
    {
        if (numDegrees.y() < 0)  ScaleFactor = ScaleFactor/1.05;
        if (numDegrees.y() > 0)  ScaleFactor = ScaleFactor*1.05;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint currentPosition = event->pos();
    float xPos = (float(currentPosition.x()-width())/width() * 2 + 1) / ScaleFactor;
    float yPos = (-float(currentPosition.y()-height())/height() * 2 - 1) / ScaleFactor ;

    float VelocityX = xPos - TempPlanetX;
    float VelocityY = yPos - TempPlanetY;

    if (event->button() == Qt::LeftButton)
    {
        Planet.AddPoint(TempPlanetX,TempPlanetY,VelocityX,VelocityY, TempPlanetMass, false);
        qDebug() << "Left Button Pressed";
    }
    else if (event -> button() == Qt::RightButton)
    {
        Planet.AddPoint(TempPlanetX,TempPlanetY,VelocityX,VelocityY, TempPlanetMass, true);
        qDebug() << "Right Button Pressed";
    }
    DrawingPlanet = false;
    qDebug() << "Added Planet x:" << xPos << " y: " << yPos ;
}

void MainWindow::on_pushButton_clicked()
{
    Planet.RemoveAllPlanets();
}


void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
    }
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}

void MainWindow::on_PauseCheckBox_stateChanged(int arg1)
{
    if (Play)
        Play = false;
    else
        Play = true;
}

