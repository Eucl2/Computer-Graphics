#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include<GL/glew.h>
#include <GL/glut.h>

#endif

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "tinyxml2.h"



// estrutura para representar um ponto no espa�o
struct Ponto {
    float x, y, z;
    Ponto(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

//transforma��es 
class Transformation {
public:
    void virtual apply() = 0;
};

//transla��o
class Translate : public Transformation {
    float x, y, z;
public:
    Translate(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void apply() {
        glTranslated(x, y, z);
    }
};

//rota��o com �ngulo
class RotateAngle : public Transformation {
    float x, y, z, angle;
public:
    RotateAngle(float angle, float x, float y, float z) {
        this->angle = angle;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void apply() {
        glRotatef(angle, x, y, z);
    }
};

//rota��o com tempo
class RotateTime : public Transformation {
    float x, y, z, time;
public:
    RotateTime(float time, float x, float y, float z) {
        this->time = time*1000;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void apply() {
        float angle = glutGet(GLUT_ELAPSED_TIME) * 360 / time;
        glRotatef(angle, x, y, z);
    }
};

//escala
class Scale : public Transformation {
    float x, y, z;
public:
    Scale(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void apply() {
        glScalef(x, y, z);
    }
};

//Fun�oes auxiliares para a cria�ao da curva

void convertPonto(Ponto p, float* point) {
    point[0] = p.x;
    point[1] = p.y;
    point[2] = p.z;

}

//contruir 4x4 matriz de rota�ao
void buildRotMatrix(float* x, float* y, float* z, float* m) {
    int idx = 0;
    for (int i = 0; i < 3; i++) {
        m[idx++] = x[i];
    }
    m[idx++] = 0;
    for (int i = 0; i < 3; i++) {
        m[idx++] = y[i];
    }
    m[idx++] = 0;
    for (int i = 0; i < 3; i++) {
        m[idx++] = z[i];
    }
    m[idx++] = 0;
    m[idx++] = 0;
    m[idx++] = 0;
    m[idx++] = 0;
    m[idx] = 1;
}

void cruzamento(float* a, float* b, float* res) {
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float* a) {
    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / 1;
    a[1] = a[1] / 1;
    a[2] = a[2] / 1;
}

void multMatrixVector(float* m, float* v, float* res) {
    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}



void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {
    //catmull-rom matriz
    float m[4][4] = {
        {-0.5f,  1.5f, -1.5f,  0.5f},
        { 1.0f, -2.5f,  2.0f, -0.5f},
        {-0.5f,  0.0f,  0.5f,  0.0f},
        { 0.0f,  1.0f,  0.0f,  0.0f}
    };

    for (int i = 0; i < 3; i++) {
        float A[4];
        float temp[] = { p0[i],p1[i],p2[i],p3[i] };
        multMatrixVector((float*)m, temp, A);


        pos[i] = pow(t, 3) * A[0] + pow(t, 2) * A[1] + t * A[2] + A[3];
        deriv[i] = 3 * pow(t, 2) * A[0] + 2 * t * A[1] + A[2];
    }
}


void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, std::vector<Ponto> p) {
    int POINT_COUNT = p.size();

    float t = gt * POINT_COUNT;
    int index = floor(t);
    t = t - index;

    //indices guarda os pontos
    int indices[4];
    indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
    indices[1] = (indices[0] + 1) % POINT_COUNT;
    indices[2] = (indices[1] + 1) % POINT_COUNT;
    indices[3] = (indices[2] + 1) % POINT_COUNT;

    float p0[3], p1[3], p2[3], p3[3];
    convertPonto(p[indices[0]], p0);
    convertPonto(p[indices[1]], p1);
    convertPonto(p[indices[2]], p2);
    convertPonto(p[indices[3]], p3);

    getCatmullRomPoint(t, p0, p1, p2, p3, pos, deriv);

}

void renderCatmullRomCurve(std::vector<Ponto> control_points) {
    float pos[3], deriv[3];
    float LINE_SEGMENTS = 100;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < LINE_SEGMENTS; i++) {
        getGlobalCatmullRomPoint(1 / LINE_SEGMENTS * i, pos, deriv, control_points);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}


//Curva
class Curve : public Transformation {
    float t, time, current_time;
    std::vector<Ponto> control_points;
    bool align;
    float prev_y[3];
public:
    Curve(std::vector<Ponto> points, bool align, float time) {
        this->t = 0;
        this->control_points = points;
        this->time = time * 1000;
        this->current_time = 0;
        this->align = align;
        this->prev_y[0] = 0;
        this->prev_y[1] = 1;
        this->prev_y[2] = 0;

    }

    void apply() {
        float pos[3], deriv[3];
        renderCatmullRomCurve(this->control_points);
        getGlobalCatmullRomPoint(t, pos, deriv, control_points);

        glTranslatef(pos[0], pos[1], pos[2]);
        if (align) {
            float x[3] = { deriv[0],deriv[1],deriv[2] };
            float y[3];
            float z[3];
            float m[16];

            normalize(x);
            cruzamento(x, prev_y, z);
            normalize(z);
            cruzamento(z, x, y);

            normalize(y);
            memcpy(prev_y, y, 3 * sizeof(float));

            buildRotMatrix(x, y, z, m);
            glMultMatrixf(m);
        }
        float new_time = glutGet(GLUT_ELAPSED_TIME);
        float diff = new_time - current_time;

        t += diff / time;
        current_time = new_time;
    }
};


//estrutura de modelo com vetor de pontos e de transforma��es
struct Model {
    std::vector<Ponto> points;//vetor de pontos que formam o modelo
    std::vector<Transformation*> transformations;//vetor de pointers para tranforma�oes que devem ser aplicadas ao modelo
    //recebe o vetor de pontos e o vetor de tranforma�oes e armazena nos respetivos membros da estrutura
    Model(std::vector<Ponto> p, std::vector<Transformation*> t) {
        this->points = p;
        this->transformations = t;
    }
};


// path para os ficheiros 3d
std::string path3d;

// path para os ficheiros xml
std::string xmlpath;
/*
// vetor de vetores de Ponto onde vao ser armazenados os pontos lidoSs dos ficheiros .3d
std::vector<std::vector<Ponto>> models;
*/
std::vector<Model> models;

float alpha = 0.0f, beta = 0.0f, radius = 5.0f;
float positionX, positionY, positionZ;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
float upX = 0.0, upY = 1.0, upZ = 0.0;
float fov = 45.0f, perto = 1.0f, longe = 1000.0f;
int windowX = 0, windowY = 0;


//calculo da posi��o da camara
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

    //evitar divisao por zero quando a janela � muito pequena
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


//Recebe o nome do arquivo como parametro e retorna o vetor de pontos que formam o modelo 3D
std::vector<Ponto> getModel(std::string source) {
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
    file_input.close();
    return model;

    }
//verificar os grupos aninhados no ficheiro xml
void readGroup(tinyxml2::XMLElement* group, std::vector<Transformation*> ts) {
    using namespace tinyxml2;
    std::vector<Transformation*> backup = ts;

    while (group) {
        ts = backup;
        XMLElement* transformation = group->FirstChildElement("transform");

        if (transformation) {
            for (XMLElement* t = transformation->FirstChildElement(); t; t = t->NextSiblingElement()) {
                std::string name = std::string(t->Name());

                if (name == "translate") {

                    if (t->Attribute("time") == nullptr) {
                        float x, y, z;
                        x = atof(t->Attribute("x"));
                        y = atof(t->Attribute("y"));
                        z = atof(t->Attribute("z"));

                        ts.push_back(new Translate(x, y, z));
                    }
                    else {
                        float time;
                        std::string align;
                        std::vector<Ponto> curve;
                        time = atof(t->Attribute("time"));
                        align = t->Attribute("align");


                        for (XMLElement* p = t->FirstChildElement("point"); p; p = p->NextSiblingElement("point")) {
                            float x, y, z;
                            x = atof(p->Attribute("x"));
                            y = atof(p->Attribute("y"));
                            z = atof(p->Attribute("z"));

                            curve.push_back(Ponto(x, y, z));
                        }

                        ts.push_back(new Curve(curve, align == "True", time));
                    }              
                }
                else if (name == "rotate") {

                    if (t->Attribute("time") == nullptr) {
                        float x, y, z, angle, time;
                        angle = atof(t->Attribute("angle"));
                        x = atof(t->Attribute("x"));
                        y = atof(t->Attribute("y"));
                        z = atof(t->Attribute("z"));

                        ts.push_back(new RotateAngle(angle, x, y, z));

                    }
                    else {
                        float x, y, z, angle, time;
                        time = atof(t->Attribute("time"));
                        x = atof(t->Attribute("x"));
                        y = atof(t->Attribute("y"));
                        z = atof(t->Attribute("z"));

                        ts.push_back(new RotateTime(time, x, y, z));


                    }
                }
                else if (name == "scale") {
                    float x, y, z;
                    x = atof(t->Attribute("x"));
                    y = atof(t->Attribute("y"));
                    z = atof(t->Attribute("z"));

                    ts.push_back(new Scale(x, y, z));
                }
                else {
                    std::cout << "error: incorrect syntax" << std::endl;
                }
            }
        }

        XMLElement* MODELS = group->FirstChildElement("models");

        if (MODELS) {
            for (XMLElement* modelo = MODELS->FirstChildElement("model"); modelo; modelo = modelo->NextSiblingElement()) {
                std::vector<Ponto> points = getModel(path3d + modelo->Attribute("file"));
                models.push_back(Model(points, ts));
            }
        }

        readGroup(group->FirstChildElement("group"), ts);
        group = group->NextSiblingElement("group");
    }
}

// le o ficheiro xml com as configura��es desejadas
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


    radius = sqrt(positionX * positionX + positionY * positionY + positionZ * positionZ);
    alpha = asin(positionX / (radius * cos(beta)));
    beta = asin(positionY / radius);
    

    XMLElement* group = doc.FirstChildElement("world")->FirstChildElement("group");

    std::vector<Transformation*> t;
    readGroup(group, t);

}


//desenha as figuras com os pontos armazenados no vetor models
void drawModels() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (Model model : models) {
        glPushMatrix();
        for (Transformation* t : model.transformations) {
            t->apply();
        }

        glBegin(GL_TRIANGLES);
        for (Ponto p : model.points) {
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
        glPopMatrix();
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
        radius -= 10.0f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_F2:  //F2 faz Zoom
        radius += 10.0f;
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
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    

    //VBO's
    glEnableClientState(GL_VERTEX_ARRAY);
   
   //keys
    glutSpecialFunc(keyboardFunc);


    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    // enter GLUTs main cicle
    glutMainLoop();

    return 1;
}