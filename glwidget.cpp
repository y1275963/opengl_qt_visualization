#include "glwidget.h"
#include <QDebug>
// All the code is default.
glwidget::glwidget(QWidget *parent) :
    QGLWidget(parent)
{
    setAutoBufferSwap(true);
    // The first coordinate is for isovalue and the second one is for the interval
    instruction = new filereader(2000.0,1);

}

void glwidget::initializeGL(){
    float lightPos[4] = { 300.0, 184.0, 184.0, 1.0 };
    float lightColAmb[4] = { 0.2, 0.2, 0.2, 1.0 };
    float lightColDif[4] = { 0.8, 0.8, 0.8, 1.0 };
    float lightColSpe[4] = { 1.0, 1.0, 1.0, 1.0 };



      glClearColor(0.0, 0.0, 0.0, 1.0);

      glEnable( GL_SMOOTH );
       glEnable( GL_DEPTH_TEST);

       glEnable(GL_LIGHTING );
       glEnable( GL_LIGHT0 );
       glLightfv( GL_LIGHT0, GL_POSITION, lightPos);
       glLightfv( GL_LIGHT0, GL_AMBIENT, lightColAmb);
       glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColDif);
       glLightfv( GL_LIGHT0, GL_SPECULAR, lightColSpe);

       glEnable(GL_COLOR_MATERIAL);
       glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
       glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);


}

void glwidget::paintGL(){
  //  qDebug() << instruction->drawlist[0];
    QList<QList<QVector3D> >::iterator iter;
  //  iter = instruction->drawlist.begin();
  //  qDebug()<<(*iter)[0];
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QVector3D  norm_temp =  QVector3D();
     glLoadIdentity();
     gluLookAt( 150, 100, 350,  0, 0, 0,  0, 1, 0);

     glTranslatef(-400, -124, -124);
     // retrie the data from drawlist and to draw them all
    for(iter = instruction->drawlist.begin(); iter!= instruction->drawlist.end();++iter){

     norm_temp = QVector3D::normal((*iter)[0],(*iter)[1],(*iter)[2]);

     glColor3f(1.0, 0.0, 0.0);
     glBegin(GL_TRIANGLES);

     glNormal3f(norm_temp.x(),norm_temp.y(),norm_temp.z());

       glVertex3i((*iter)[0].x(),  (*iter)[0].y(), (*iter)[0].z()  );
       glVertex3i((*iter)[1].x(),  (*iter)[1].y(), (*iter)[1].z()  );
       glVertex3i((*iter)[2].x(),  (*iter)[2].y(), (*iter)[2].z() );
     glEnd();
     }
    glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINE_LOOP);
        glVertex3i(  0,  0, 0  );
        glVertex3i(599,  0, 0  );
        glVertex3i(599,247, 0  );
        glVertex3i(  0,247, 0  );
      glEnd();
      glBegin(GL_LINE_LOOP);
        glVertex3i(  0,  0, 247);
        glVertex3i(599,  0, 247);
        glVertex3i(599,247, 247);
        glVertex3i(  0,247, 247);
      glEnd();
      glBegin(GL_LINES);
        glVertex3i(  0,  0, 0  );
        glVertex3i(  0,  0, 247);
        glVertex3i(599,  0, 0  );
        glVertex3i(599,  0, 247);
        glVertex3i(599,247, 0  );
        glVertex3i(599,247, 247);
        glVertex3i(  0,247, 0  );
        glVertex3i(  0,247, 247);
      glEnd();
}

void glwidget::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 80, (double)w / (double)h, 0.1, 1500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
