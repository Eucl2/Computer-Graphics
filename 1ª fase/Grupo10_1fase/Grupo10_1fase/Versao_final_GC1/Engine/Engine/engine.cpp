#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "tinyxml2.h"


// estrutura para representar um ponto no espaço
struct Ponto {
    float x, y, z;
    Ponto(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

// path para os ficheiros 3d
std::string path3d;

// path para os ficheiros xml
std::string xmlpath;

// vetor de vetores de Ponto onde vao ser armazenados os pontos lidos dos ficheiros .3d
std::vector<std::vector<Ponto>> models;

float alpha = 0.0f, beta = 0.0f, radius = 5.0f;
float positionX, positionY, positionZ;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
float upX = 0.0, upY = 1.0, upZ = 0.0;
float fov = 45.0f, perto = 1.0f, longe = 1000.0f;
int windowX = 0, windowY = 0;

//calculo da posição da camara
void posCamara() {
    positionX = radius * cos(beta) * sin(alpha);
    positionY = radius * sin(beta);
    positionZ = radius * cos(beta) * cos(alpha);
}

void changeSize(int w, int h) {
    
    //caso no ficheiro .xml ter janela definida
    if (windowX != 0 or windowY != 0) {
        w = windowX;
        h = windowY;
    }

    //evitar divisao por zero quando a janela é muito pequena
    if (h == 0) h = 1;

    //alterar o tamanho da janela para o do ficheiro .xml
    glutReshapeWindow(w, h);

    //ratio da janela 
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(fov, ratio, perto, longe);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

// le os pontos armazenados em no ficheiro .3d e armazena em models vetor de vetores do Ponto
void getModel(std::string source) {
    std::ifstream file_input(source);
    file_input.ignore(10000, '\n');//skip da primeira linha
    float x, y, z;
    std::vector<Ponto> model;

    while (file_input >> x >> y >> z) {
        //std::cout << x << "\n";
        //std::cout << y << "\n";
        //std::cout << z << "\n";
        model.push_back(Ponto(x, y, z));
    }
    models.push_back(model);
    file_input.close();
}

// le o ficheiro xml com as configurações desejadas
void readXml(std::string source) {
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile(source.data());

    
    XMLElement* window = doc.FirstChildElement("world")->FirstChildElement("window");
    windowX = atoi(window->Attribute("width"));
    windowY = atoi(window->Attribute("height"));
    //std::cout << windowX;
    //std::cout << windowY;
    

    XMLElement* camera = doc.FirstChildElement("world")->FirstChildElement("camera");
    XMLElement* position = camera->FirstChildElement("position");
    positionX = atof(position->Attribute("x"));
    positionY = atof(position->Attribute("y"));
    positionZ = atof(position->Attribute("z"));

    XMLElement* lookAt = camera->FirstChildElement("lookAt");
    lookAtX = atof(lookAt->Attribute("x"));
    lookAtY = atof(lookAt->Attribute("y"));
    lookAtZ = atof(lookAt->Attribute("z"));

    XMLElement* up = camera->FirstChildElement("up");
    upX = atof(up->Attribute("x"));
    upY = atof(up->Attribute("y"));
    upZ = atof(up->Attribute("z"));

    XMLElement* projection = camera->FirstChildElement("projection");
    fov = atof(projection->Attribute("fov"));
    perto = atof(projection->Attribute("near"));
    longe = atof(projection->Attribute("far"));

    XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("models");
    XMLElement* model = mod->FirstChildElement("model");
    while (model) {
        std::string model_path = model->Attribute("file");
        getModel(path3d + model_path);
        //std::cout << "aqui " << path_3d + model_path;

        model = model->NextSiblingElement("model");
    }

    radius = sqrt(positionX * positionX + positionY * positionY + positionZ * positionZ);
    alpha = asin(positionX / (radius * cos(beta)));
    beta = asin(positionY / radius);
}


//desenha as figuras com os pontos armazenados no vetor models
void drawModels() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (std::vector<Ponto> model : models) {
        glBegin(GL_TRIANGLES);
        for (Ponto p : model) {
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

// desenha os eixos x y z
void drawAxis() {

    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-radius, 0, 0);
    glVertex3f(radius, 0, 0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, -radius, 0);
    glVertex3f(0, radius, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, -radius);
    glVertex3f(0, 0, radius);

    glEnd();
    glColor3f(1.0, 1.0, 1.0);
}

void renderScene(void) {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(positionX, positionY, positionZ,lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

    glPolygonMode(GL_FRONT, GL_LINE);

    drawAxis();
    drawModels();

    // End of frame
    glutSwapBuffers();
}

void keyboardFunc(int key, int xx, int yy) {

    switch (key) {

    case GLUT_KEY_RIGHT:
        alpha -= 0.1;
        break;

    case GLUT_KEY_LEFT:
        alpha += 0.1;
        break;

    case GLUT_KEY_UP:
        beta += 0.1f;
        if (beta > 1.5f)
            beta = 1.5f;
        break;

    case GLUT_KEY_DOWN:
        beta -= 0.1f;
        if (beta < -1.5f)
            beta = -1.5f;
        break;

    case GLUT_KEY_F1: ////F1 distancia
        radius -= 0.1f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_F2:  //F2 faz Zoom
        radius += 0.1f;
        break;

    default:
        break;
    }
    posCamara();
    glutPostRedisplay();

}

int main(int argc, char** argv) {
    // path para os ficheiros m3d e fxml
    path3d = "..//..//ficheiros//m3d//";
    xmlpath = "..//..//ficheiros//fxml//";


    posCamara();

    if (argc == 2)
        readXml(xmlpath + argv[1]);
       // readXML("..//..//ficheiros//xml//teste.xml");


    // GLUT init here
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("ProjetoCG");

   //put callback registry here
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

   //keys
    glutSpecialFunc(keyboardFunc);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    // enter GLUTs main cicle
    glutMainLoop();

    return 1;
}