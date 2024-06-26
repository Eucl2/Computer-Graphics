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

#include <IL/il.h>


// estrutura para representar um ponto no espaço
struct Ponto {
    float x, y, z;
    Ponto(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};
//normais 
struct Normal {
    float nx, ny, nz;
    Normal(float nx, float ny, float nz) {
        this->nx = nx;
        this->ny = ny;
        this->nz = nz;
    }
};
//coordenadas de textura
struct TexturaC {
    float tx, tz;
    TexturaC(float tx, float tz) {
        this->tx = tx;
        this->tz = tz;
    }
};

//transformações 
class Transformation {
public:
    void virtual apply() = 0;
};


//translação
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

//rotação com ângulo
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

//rotação com tempo
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

//Funçoes auxiliares para a criaçao da curva

void convertPonto(Ponto p, float* point) {
    point[0] = p.x;
    point[1] = p.y;
    point[2] = p.z;

}

//contruir 4x4 matriz de rotaçao
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

//iluminacao
int getLight(int nLight) {
    int CLight;
    switch (nLight) {
    case 0: CLight = GL_LIGHT0; break;
    case 1: CLight = GL_LIGHT1; break;
    case 2: CLight = GL_LIGHT2; break;
    case 3: CLight = GL_LIGHT3; break;
    case 4: CLight = GL_LIGHT4; break;
    case 5: CLight = GL_LIGHT5; break;
    case 6: CLight = GL_LIGHT6; break;
    case 7: CLight = GL_LIGHT7; break;
    default: exit(1);

    }
    return CLight;
}

class Light {
public:
    int index;
    void virtual apply() = 0;
};

class LightPoint : public Light {
    float pos[4];
public:
    LightPoint(float a, float b, float c, int i) {
        this->pos[0] = a;
        this->pos[1] = b;
        this->pos[2] = c;
        this->pos[3] = 1.0f;
        this->index = getLight(i);

    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->pos);
    }
};

class LightDirectional : public Light {
    float dir[4];
public:
    LightDirectional(float a, float b, float c, int i) {
        this->dir[0] = a;
        this->dir[1] = b;
        this->dir[2] = c;
        this->dir[3] = 0.0f;
        this->index = getLight(i);

    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->dir);
    }
};

class LightSpotlight : public Light {
    float pos[4], dir[4], cutoff;
public:
    LightSpotlight(float a, float b, float c, float da, float db, float dc, float ct, int i) {
        this->pos[0] = a;
        this->pos[1] = b;
        this->pos[2] = c;
        this->pos[3] = 1.0f;
        this->dir[0] = da;
        this->dir[1] = db;
        this->dir[2] = dc;
        this->dir[3] = 0.0f;
        this->cutoff = ct;
        this->index = getLight(i);
    }

    void apply() {
        glLightfv(this->index, GL_POSITION, this->pos);
        glLightfv(this->index, GL_SPOT_DIRECTION, this->dir);
        glLightfv(this->index, GL_SPOT_CUTOFF, &(this->cutoff));


    }
};

class Color {
public:
    void virtual apply() = 0;
};

class Diffuse : public Color {
    float rgb[4];
public:
    Diffuse(float a, float b, float c) {
        this->rgb[0] = a;
        this->rgb[1] = b;
        this->rgb[2] = c;
        this->rgb[3] = 1.0f;
    }

    void apply() {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, this->rgb);
    }


};

class Ambient : public Color {
    float rgb[4];
public:
    Ambient(float a, float b, float c) {
        this->rgb[0] = a;
        this->rgb[1] = b;
        this->rgb[2] = c;
        this->rgb[3] = 1.0f;
    }

    void apply() {
        glMaterialfv(GL_FRONT, GL_AMBIENT, this->rgb);
    }
};

class Specular : public Color {
    float rgb[4];
public:
    Specular(float a, float b, float c) {
        this->rgb[0] = a;
        this->rgb[1] = b;
        this->rgb[2] = c;
        this->rgb[3] = 1.0f;

    }

    void apply() {
        glMaterialfv(GL_FRONT, GL_SPECULAR, this->rgb);
    }
};

class Emissive : public Color {
    float rgb[4];
public:
    Emissive(float a, float b, float c) {
        this->rgb[0] = a;
        this->rgb[1] = b;
        this->rgb[2] = c;
        this->rgb[3] = 1.0f;

    }

    void apply() {
        glMaterialfv(GL_FRONT, GL_EMISSION, this->rgb);
    }
};

class Shininess : public Color {
    float s;
public:
    Shininess(float a) {
        this->s = a;
    }

    void apply() {
        glMaterialf(GL_FRONT, GL_SHININESS, this->s);
    }
};


//estrutura de modelo com vetor de pontos e de transformações
struct Model {
    GLuint texture;
    std::vector<Color*> colors;
    std::vector<Transformation*> transformations;//vetor de pointers para tranformaçoes que devem ser aplicadas ao modelo
    std::vector<Ponto> points;
    std::vector<Normal> normals;
    std::vector<TexturaC> texturasc;
    //GLuint vertexCount, vertices, normals, texCoord, texID;
    //std::vector<float> vertices, normais, texturas;
    Model(std::vector<Ponto> p, std::vector<Normal> n, std::vector<TexturaC> texc, std::vector<Transformation*> t, std::vector<Color*> c, GLuint tex) {
        this->points = p;
        this->normals = n;
        this->texturasc = texc;
        this->transformations = t;
        this->colors = c;
        this->texture = tex;
    }
};

//path para as texturas 
std::string pathtexturas;

// path para os ficheiros 3d
std::string path3d;

// path para os ficheiros xml
std::string xmlpath;

std::vector<Model> models;

//luz
std::vector<Light*> lights;

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


//Recebe o nome do arquivo como parametro e retorna o vetor de pontos que formam o modelo 3D
std::vector<Ponto> getModelPontos(std::string source) {
    std::ifstream file_input(source);
    file_input.ignore(10000, '\n'); // skip da primeira linha
    float x, y, z, nx, ny, nz, tx, tz;
    //GLuint vertexCount1, vertices1, normals1, texCoord1;

    std::vector<Ponto> model;

    while (file_input >> x >> y >> z >> nx >> ny >> nz >> tx >> tz) {
        //std::cout << x << "\n";
        //std::cout << y << "\n";
        //std::cout << z << "\n";
        model.push_back(Ponto(x, y, z));
    }
    file_input.close();
    return model;

    }

//Recebe o ficheiro e guarda as coordenadas normais
std::vector<Normal> getModelNormais(std::string source) {
    std::ifstream file_input(source);
    file_input.ignore(10000, '\n'); // skip da primeira linha
    float x, y, z, nx, ny, nz, tx, tz;
    //GLuint vertexCount1, vertices1, normals1, texCoord1;

    std::vector<Normal> model;

    while (file_input >> x >> y >> z >> nx >> ny >> nz >> tx >> tz) {
        //std::cout << x << "\n";
        //std::cout << y << "\n";
        //std::cout << z << "\n";
        model.push_back(Normal(nx, ny, nz));
    }
    file_input.close();
    return model;

}

//Recebe o ficheiro e guarda as coordernadas textura
std::vector<TexturaC> getModelTexturaC(std::string source) {
    std::ifstream file_input(source);
    file_input.ignore(10000, '\n'); // skip da primeira linha
    float x, y, z, nx, ny, nz, tx, tz;
    //GLuint vertexCount1, vertices1, normals1, texCoord1;

    std::vector<TexturaC> model;

    while (file_input >> x >> y >> z >> nx >> ny >> nz >> tx >> tz) {
        //std::cout << x << "\n";
        //std::cout << y << "\n";
        //std::cout << z << "\n";
        model.push_back(TexturaC(tx,tz));
    }
    file_input.close();
    return model;

}
#include <random>

//int generateTextureID() {
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<unsigned int> dis;
//
//    return dis(gen);
//}

int loadTexture(std::string texture_name) {
    //GLuint texID;
    unsigned int t, tw, th;
    unsigned char* texData;
    unsigned int texID;
    //unsigned int texID = generateTextureID();
    glGenTextures(1, &texID);
    //std::cout << "entrei" << "\n";
    
    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilGenImages(1, &t);
    ilBindImage(t);
    ilLoadImage((ILstring)texture_name.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1, &texID);
    //std::cout << "o valor de =" << texID << "\n";

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //std::cout << "o valor de =" << texID << "\n";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //std::cout << "o valor de =" << texID << "\n";
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    //std::cout << "o valor de =" << texID << "\n";
    //glGenerateMipmap(GL_TEXTURE_2D);
    //std::cout << "o valor de =" << texID << "\n";
    glBindTexture(GL_TEXTURE_2D, 0);

    //std::cout << "o valor de=" << texID << "\n";

    return texID;
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
                int texture_id = -1;

                std::vector<Ponto> points = getModelPontos(path3d + modelo->Attribute("file"));
                std::vector<Normal> normais = getModelNormais(path3d + modelo->Attribute("file"));
                std::vector<TexturaC> texturaC = getModelTexturaC(path3d + modelo->Attribute("file"));
                

                XMLElement* texture = modelo->FirstChildElement("texture");
                if (texture) {
                    texture_id = loadTexture(pathtexturas + texture->Attribute("file"));
                }

                std::vector<Color*> colors;
                XMLElement* COLORS = modelo->FirstChildElement("color");
                if (COLORS) {
                    for (XMLElement* COLOR = COLORS->FirstChildElement(); COLOR; COLOR = COLOR->NextSiblingElement()) {
                        std::string ColorName = std::string(COLOR->Name());

                        if (ColorName == "diffuse") {
                            float r = atof(COLOR->Attribute("R")) / 255;
                            float rg = atof(COLOR->Attribute("G")) / 255;
                            float rgb = atof(COLOR->Attribute("B")) / 255;

                            colors.push_back(new Diffuse(r, rg, rgb));
                        }
                        else if (ColorName == "ambient") {
                            float r = atof(COLOR->Attribute("R")) / 255;
                            float rg = atof(COLOR->Attribute("G")) / 255;
                            float rgb = atof(COLOR->Attribute("B")) / 255;

                            colors.push_back(new Ambient(r, rg, rgb));
                        }
                        else if (ColorName == "specular") {
                            float r = atof(COLOR->Attribute("R")) / 255;
                            float rg = atof(COLOR->Attribute("G")) / 255;
                            float rgb = atof(COLOR->Attribute("B")) / 255;

                            colors.push_back(new Specular(r, rg, rgb));
                        }
                        else if (ColorName == "emissive") {
                            float r = atof(COLOR->Attribute("R")) / 255;
                            float rg = atof(COLOR->Attribute("G")) / 255;
                            float rgb = atof(COLOR->Attribute("B")) / 255;

                            colors.push_back(new Emissive(r, rg, rgb));
                        }
                        else if (ColorName == "shininess") {
                            float value = atof(COLOR->Attribute("value"));

                            colors.push_back(new Shininess(value));
                        }
                    }
                }
                models.push_back(Model(points, normais, texturaC, ts, colors, texture_id));
            }


        }

        readGroup(group->FirstChildElement("group"), ts);
        group = group->NextSiblingElement("group");
    }
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


    radius = sqrt(positionX * positionX + positionY * positionY + positionZ * positionZ);
    alpha = asin(positionX / (radius * cos(beta)));
    beta = asin(positionY / radius);
    

    XMLElement* group = doc.FirstChildElement("world")->FirstChildElement("group");

    std::vector<Transformation*> t;
    readGroup(group, t);

    XMLElement* LIGHTS = doc.FirstChildElement("world")->FirstChildElement("lights");

    int lightIndex = 0;
    if (LIGHTS) {
        for (XMLElement* l = LIGHTS->FirstChildElement("light"); l; l = l->NextSiblingElement("light")) {
            std::string lightType = l->Attribute("type");
            if (lightType == "point") {
                float x = atof(l->Attribute("posx"));
                float y = atof(l->Attribute("posy"));
                float z = atof(l->Attribute("posz"));

                lights.push_back(new LightPoint(x, y, z, lightIndex));
            }
            else if (lightType == "directional") {
                float dirx = atof(l->Attribute("dirx"));
                float diry = atof(l->Attribute("diry"));
                float dirz = atof(l->Attribute("dirz"));

                lights.push_back(new LightDirectional(dirx, diry, dirz, lightIndex));
            }
            else if (lightType == "spot") {
                float x = atof(l->Attribute("posx"));
                float y = atof(l->Attribute("posy"));
                float z = atof(l->Attribute("posz"));
                float dirx = atof(l->Attribute("dirx"));
                float diry = atof(l->Attribute("diry"));
                float dirz = atof(l->Attribute("dirz"));
                float cutoff = atof(l->Attribute("cutoff"));

                lights.push_back(new LightSpotlight(x, y, z, dirx, diry, dirz, cutoff, lightIndex));
            }

            lightIndex++;
        }
    }
}


//desenha as figuras com os pontos armazenados no vetor models
void drawModels() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (Model model : models) {
        glPushMatrix();
        for (Transformation* t : model.transformations) {
            t->apply();
        }

        for (Color* c : model.colors) {
            c->apply();
        }

        glBegin(GL_TRIANGLES);
        for (Ponto p : model.points) {
            glVertex3f(p.x, p.y, p.z);
        }
        for (Normal n : model.normals) {
            glNormal3f(n.nx, n.ny, n.nz);
        }
        if (model.texture != -1) {
            for (TexturaC tex : model.texturasc) {
                glTexCoord2f(tex.tx, tex.tz);
            }
        }

        glEnd();
        glPopMatrix();
    }
}

void lightsOn() {
    for (Light* l : lights) {
        l->apply();
    }
}

// desenha os eixos x y z
void drawAxis() {

    glDisable(GL_LIGHTING);
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
    glEnable(GL_LIGHTING);
}

void renderScene(void) {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(positionX, positionY, positionZ,lookAtX, lookAtY, lookAtZ, upX, upY, upZ);


    //glPolygonMode(GL_FRONT, GL_LINE);
    drawAxis();
    lightsOn();
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
        radius -= 5.0f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_F2:  //F2 faz Zoom
        radius += 5.0f;
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
    pathtexturas = "..//..//ficheiros//textures//";

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

    glEnable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);

   //put callback registry here
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    

    #ifndef __APPLE__
    // init GLEW
    glewInit();
    #endif


    float dark[4] = { 0.3, 0.3, 0.3, 1.0 };
    float white[4] = { 1.0, 1.0, 1.0, 1.0 };

    for (Light* l : lights) {
        glEnable(l->index);

        // light colors
        glLightfv(l->index, GL_AMBIENT, dark);
        glLightfv(l->index, GL_DIFFUSE, white);
        glLightfv(l->index, GL_SPECULAR, white);

    }

   
   //keys
    glutSpecialFunc(keyboardFunc);

    //vbos
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    
    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUTs main cicle
    glutMainLoop();

    return 1;
}