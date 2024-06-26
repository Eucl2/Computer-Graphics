#include "funcgera.h"
#include <cmath>
#define M_PI 3.14159265358979323846
#include <sstream>
#include <iomanip> 
#include <fstream>
#include <iostream>


void normalize(float* a) {
    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    if (l != 0) {
        a[0] = a[0] / l;
        a[1] = a[1] / l;
        a[2] = a[2] / l;
    }
}

void cross(float* a, float* b, float* res) {

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}


//### ESFERA #####
void createSphere(float radius, int slices, int stacks, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices

    float x1, x2, x3, x4;
    float y1, y2;
    float z1, z2, z3, z4;
    float sigma = 2*M_PI / slices; //variação angular da slice
    float theta = M_PI / stacks;    //variação angular da stack
    //float zeta = M_PI/ 2;       //ângulo a partir do eixo z

    //normais 

    float p1n[3], p2n[3], p3n[3], p4n[3];



    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            x1 = radius * cos(M_PI/2 - theta * j) * sin(sigma * i);
            x2 = radius * cos(M_PI/2 - theta * (j + 1)) * sin(sigma * i);
            x3 = radius * cos(M_PI/2 - theta * (j + 1)) * sin(sigma * (i + 1));
            x4 = radius * cos(M_PI/2 - theta * j) * sin(sigma * (i + 1));
            p1n[0] = cos(M_PI / 2 - theta * j) * sin(sigma * i);
            p2n[0] = cos(M_PI / 2 - theta * (j + 1)) * sin(sigma * i);
            p3n[0] = cos(M_PI / 2 - theta * (j + 1)) * sin(sigma * (i + 1));
            p4n[0] = cos(M_PI / 2 - theta * j) * sin(sigma * (i + 1));

            y1 = radius * sin(M_PI/2 - theta * j);
            y2 = radius * sin(M_PI/2 - theta * (j + 1));
            p1n[1] = sin(M_PI / 2 - theta * j);
            p4n[1] = sin(M_PI / 2 - theta * j);
            p2n[1] = sin(M_PI / 2 - theta * (j + 1));
            p3n[1] = sin(M_PI / 2 - theta * (j + 1));

            z1 = radius * cos(M_PI/2 - theta * j) * cos(sigma * i);
            z2 = radius * cos(M_PI/2 - theta * (j + 1)) * cos(sigma * i);
            z3 = radius * cos(M_PI/2 - theta * (j + 1)) * cos(sigma * (i + 1));
            z4 = radius * cos(M_PI/2 - theta * j) * cos(sigma * (i + 1));
            p1n[2] = cos(M_PI / 2 - theta * j) * cos(sigma * i);
            p2n[2] = cos(M_PI / 2 - theta * (j + 1)) * cos(sigma * i);
            p3n[2] = cos(M_PI / 2 - theta * (j + 1)) * cos(sigma * (i + 1));
            p4n[2] = cos(M_PI / 2 - theta * j) * cos(sigma * (i + 1));

            float tex1 = i/slices;
            float tex2 = (i + 1)/slices;
            float tez1 = j/stacks;
            float tez2 = (j + 1)/stacks;


            //normalize
            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);


            //vertices para o triangulo superior do quadrilatero
            if (j != stacks - 1) {
                vertices.push_back({ x1, y1,  z1, p1n[0], p1n[1], p1n[2], tex1, tez1 });
                vertices.push_back({ x2,  y2, z2, p2n[0], p2n[1], p2n[2], tex1, tez2 });
                vertices.push_back({ x3,  y2, z3, p3n[0], p3n[1], p3n[2], tex2, tez2 });
            }
            //vertices para o triangulo inferior do quadrilatero
            if (j != 0) {
                vertices.push_back({ x1,  y1,  z1, p1n[0], p1n[1], p1n[2], tex1, tez1 });
                vertices.push_back({ x3,  y2,  z3, p3n[0], p3n[1], p3n[2], tex2, tez2 });
                vertices.push_back({ x4,  y1,  z4, p4n[0], p4n[1], p4n[2], tex2, tez1 });
            }


        }
    }
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}



//### BOX ######
void createBox(float dimension, int divisions, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices

    float metdim = dimension / 2.0f; //metade da dimensão total da caixa
    float unidade = dimension / divisions; //distância entre cada linha ou coluna
    float x1, x2;
    float y1, y2;
    float z1, z2;
    float tex1, tex2;//textura
    float tez1, tez2;
     
    //normais do cubo
    float cima[3] = { 0,1,0 };
    float baixo[3] = { 0,-1,0 };
    float esquerda[3] = { -1,0,0 };
    float direita[3] = { 1,0,0 };
    float frente[3] = { 0,0,1 };
    float traseira[3] = { 0,0,-1 };
    float texsize = 1.0f / divisions; 
    
    //faces superior e inferior da box eixo y
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * unidade - metdim;
            z1 = j * unidade - metdim;
            x2 = (i + 1) * unidade - metdim;
            z2 = (j + 1) * unidade - metdim;
            tex1 = i * texsize;
            tex2 = (i+1) * texsize;
            tez1 = j * texsize;
            tez2 = (j + 1) * texsize;


            vertices.push_back({ x1,  metdim,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  metdim,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ x2,  metdim,  z1, cima[0], cima[1], cima[2], tex2, tez1 });

  
            vertices.push_back({ x1,  metdim,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x1,  metdim,  z2, cima[0], cima[1], cima[2], tex1, tez2 });
            vertices.push_back({ x2,  metdim,  z2, cima[0], cima[1], cima[2], tex2, tez2 });

           
            vertices.push_back({ x2,  -metdim,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ x1,  -metdim,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  -metdim,  z1, cima[0], cima[1], cima[2], tex2, tez1 });

   
            vertices.push_back({ x1,  -metdim,  z2, cima[0], cima[1], cima[2], tex1, tez2 });
            vertices.push_back({ x1,  -metdim,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  -metdim,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
        }
    }
  
   
    //face frontal e traseira eixo z
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * unidade - metdim;
            y1 = j * unidade - metdim;
            x2 = (i + 1) * unidade - metdim;
            y2 = (j + 1) * unidade - metdim;
            tex1 = i * texsize;
            tex2 = (i + 1) * texsize;
            tez1 = j * texsize;
            tez2 = (j + 1) * texsize;

    
            vertices.push_back({ x2,  y2,  metdim, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ x1,  y1,  metdim, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  y1,  metdim, cima[0], cima[1], cima[2], tex2, tez1 });

    
            vertices.push_back({ x1,  y2,  metdim, cima[0], cima[1], cima[2], tex1, tez2 });
            vertices.push_back({ x1,  y1,  metdim, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  y2,  metdim, cima[0], cima[1], cima[2], tex2, tez2 });


            vertices.push_back({ x1,  y1,  -metdim, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x2,  y2,  -metdim, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ x2,  y1,  -metdim, cima[0], cima[1], cima[2], tex2, tez1 });


            vertices.push_back({ x1,  y1,  -metdim, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ x1,  y2,  -metdim, cima[0], cima[1], cima[2], tex1, tez2 });
            vertices.push_back({ x2,  y2,  -metdim, cima[0], cima[1], cima[2], tex2, tez2 });
        }
    }
    
    //face lateral esquerda e direita eixo do x
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            z1 = i * unidade - metdim;
            y1 = j * unidade - metdim;
            z2 = (i + 1) * unidade - metdim;
            y2 = (j + 1) * unidade - metdim;
            tex1 = i * texsize;
            tex2 = (i + 1) * texsize;
            tez1 = j * texsize;
            tez2 = (j + 1) * texsize;


            vertices.push_back({ metdim,  y1,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ metdim,  y2,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ metdim,  y1,  z2, cima[0], cima[1], cima[2], tex1, tez2 });
            

           
            vertices.push_back({ metdim,  y1,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ metdim,  y2,  z1, cima[0], cima[1], cima[2], tex2, tez1 });
            vertices.push_back({ metdim,  y2,  z2, cima[0], cima[1], cima[2], tex2, tez2 });


            vertices.push_back({ -metdim,  y2,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
            vertices.push_back({ -metdim,  y1,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ -metdim,  y1,  z2, cima[0], cima[1], cima[2], tex1, tez2 });


            vertices.push_back({ -metdim,  y2,  z1, cima[0], cima[1], cima[2], tex2, tez1 });
            vertices.push_back({ -metdim,  y1,  z1, cima[0], cima[1], cima[2], tex1, tez1 });
            vertices.push_back({ -metdim,  y2,  z2, cima[0], cima[1], cima[2], tex2, tez2 });
          
        }
    }
    
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}



//###PLANO####
void createPlane(float size, int divisions, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices
    //dist metade
    float dist = size / divisions;// distância entre vértices adjacentes em cada eixo.
    float metade = size / 2.0f;// metade do comprimento de um dos lados do plano
    float x1, x2;
    float z1, z2;
    float tex1, tex2;//textura
    float tez1, tez2;

    //normal do plano
    float nor[3] = { 0,1,0 };
    float texsize = 1.0f / divisions;


    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * dist - metade;
            z1 = j * dist - metade;
            x2 = (i + 1) * dist - metade;
            z2 = (j + 1) * dist - metade;
            tex1 = i * texsize;
            tex2 = (i + 1) * texsize;
            tez1 = j * texsize;
            tez2 = (j + 1) * texsize;


            vertices.push_back({ x1,  0,  z1, nor[0], nor[1], nor[2], tex1, tez1 });
            vertices.push_back({ x2,  0,  z2, nor[0], nor[1], nor[2], tex2, tez2 });
            vertices.push_back({ x2,  0,  z1, nor[0], nor[1], nor[2], tex2, tez1 });

            
            vertices.push_back({ x1,  0,  z1, nor[0], nor[1], nor[2], tex1, tez1 });
            vertices.push_back({ x1,  0,  z2, nor[0], nor[1], nor[2], tex1, tez2 });
            vertices.push_back({ x2,  0,  z2, nor[0], nor[1], nor[2], tex2, tez2 });
            
        }
    }


    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}


//### CONE #####
void createCone(float bottomRadius, float height, int slices, int stacks, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices
    float radiusS = height / bottomRadius; //fator de escala no raio da base
    float heightS = height / stacks; //distância vertical entre as fatias do cone
    float alfa = 2 * M_PI / slices; //ângulo entre as slices
    float x1, x2, x3, x4;
    float y1, y2;
    float z1, z2, z3, z4;
    float h1, h2;
    float r1, r2;
    float ny = sin(atan(bottomRadius / height)); 

    //normais 

    float p1n[3], p2n[3], p3n[3], p4n[3], pbn[3] = {0,-1,0}; 
    float tex1, tex2, tez1, tez2, txb1, txb2, tzb1, tzb2;



    //base do cone
    for (int i = 0; i < slices; i++) {
        x1 = bottomRadius * sin(alfa * i);
        x2 = bottomRadius * sin(alfa * (i + 1));
        z1 = bottomRadius * cos(alfa * i);
        z2 = bottomRadius * cos(alfa * (i + 1));
        txb1 = 0.5 + sin(alfa * i);
        txb2 = 0.5 + sin(alfa * (i + 1));
        tzb1 = 0.5 + cos(alfa * i);
        tzb2 = 0.5 + cos(alfa * (i + 1));

       


        vertices.push_back({ x1,  0,  z1, pbn[0], pbn[1], pbn[0], txb1, tzb1 });
        vertices.push_back({ 0,  0,  0, pbn[0], pbn[1], pbn[0], 0.5, 0.5 });
        vertices.push_back({ x2,  0,  z2, pbn[0], pbn[1], pbn[0], txb2, tzb2 });
    }

    //restante do cone
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            h1 = height - (i * heightS);
            h2 = height - ((i + 1) * heightS);
            r1 = h1 / radiusS;
            r2 = h2 / radiusS;

            x1 = r1 * sin(alfa * j);
            x2 = r1 * sin(alfa * (j + 1));
            x3 = r2 * sin(alfa * (j + 1));
            x4 = r2 * sin(alfa * j);

            p1n[0] = sin(alfa * j);
            p2n[0] = sin(alfa * (j + 1));
            p3n[0] = sin(alfa * (j + 1));
            p4n[0] = sin(alfa * j);

            y1 = (i * heightS);
            y2 = (i + 1) * heightS;

            p1n[1] = ny;
            p2n[1] = ny;
            p3n[1] = ny;
            p4n[1] = ny;

            z1 = r1 * cos(alfa * j);
            z2 = r1 * cos(alfa * (j + 1));
            z3 = r2 * cos(alfa * (j + 1));
            z4 = r2 * cos(alfa * j);

            p1n[2] = cos(alfa * j);
            p2n[2] = cos(alfa * (j + 1));
            p3n[2] = cos(alfa * (j + 1));
            p4n[2] = cos(alfa * j);

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);

            tex1 = j / slices;
            tex2 = (j + 1) / slices;
            tez1 = i / stacks;
            tez2 = (i + 1) / stacks;



            vertices.push_back({ x1,  y1,  z1, p1n[0], p1n[1], p1n[2], tex1, tez1 });
            vertices.push_back({ x2,  y1,  z2, p2n[0], p2n[1], p2n[2], tex2, tez1 });
            vertices.push_back({ x4,  y2,  z4, p4n[0], p4n[1], p4n[2], tex1, tez2 });

            if (j != slices - 1) {

                vertices.push_back({ x4,  y2,  z4, p4n[0], p4n[1], p4n[2], tex1, tez2 });
                vertices.push_back({ x2,  y1,  z2, p2n[0], p2n[1], p2n[2], tex2, tez1 });
                vertices.push_back({ x3,  y2,  z3, p3n[0], p3n[1], p3n[2], tex2, tez2 });
            }

        }
    }
    
    //glEnd();

    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}

// ### TORUS #####
void createTorus(float radius, float largura, int slices, int stacks, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices

    float x1, x2, x3, x4;
    float y1, y2, y3, y4;
    float z1, z2, z3, z4;
    float alfa = (2 * M_PI) / stacks; //variação angular da stack
    float beta = (2* M_PI) / slices;  //variação angular da slice

    //normais 

    float p1n[3], p2n[3], p3n[3], p4n[3]; 
    
    // Calcula os valores do maior e o menor raio do torus 
    float omega, iota;
    omega = (radius + largura) / 2;
    iota = radius - largura;


    //Criar os vertices  
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = (omega + iota * cos(alfa * i)) * cos(beta * j);
            x2 = (omega + iota * cos(alfa * (i + 1))) * cos(beta * j);
            x3 = (omega + iota * cos(alfa * (i + 1))) * cos(beta * (j + 1));
            x4 = (omega + iota * cos(alfa * i)) * cos(beta * (j + 1));

            p1n[0] = iota * cos(alfa * i)* cos(beta * j);
            p2n[0] = iota * cos(alfa * (i + 1))* cos(beta * j);
            p3n[0] = iota * cos(alfa * (i + 1))* cos(beta * (j + 1));
            p4n[0] = iota * cos(alfa * i)* cos(beta * (j + 1));

            y1 = iota * sin(alfa * i);
            y2 = iota * sin(alfa * (i + 1));
            y3 = iota * sin(alfa * (i + 1));
            y4 = iota * sin(alfa * i);

            p1n[1] = iota * sin(alfa * i);
            p2n[1] = iota * sin(alfa * (i + 1));
            p3n[1] = iota * sin(alfa * (i + 1));
            p4n[1] = iota * sin(alfa * i);

            z1 = (omega + iota * cos(alfa * i)) * sin(beta * j);
            z2 = (omega + iota * cos(alfa * (i + 1))) * sin(beta * j);
            z3 = (omega + iota * cos(alfa * (i + 1))) * sin(beta * (j + 1));
            z4 = (omega + iota * cos(alfa * i)) * sin(beta * (j + 1));

            p1n[2] = iota * cos(alfa * i)* sin(beta * j);
            p2n[2] = iota * cos(alfa * (i + 1))* sin(beta * j);
            p3n[2] = iota * cos(alfa * (i + 1))* sin(beta * (j + 1));
            p4n[2] = iota * cos(alfa * i)* sin(beta * (j + 1));

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);


            float tex1 = i / stacks;
            float tex2 = (i + 1) / stacks;
            float tez1 = j / slices;
            float tez2 = (j + 1) / slices;

    
            vertices.push_back({ x1,  y1,  z1, p1n[0], p1n[1], p1n[2], tex1, tez1 });
            vertices.push_back({ x2,  y2,  z2, p2n[0], p2n[1], p2n[2], tex2, tez1 });
            vertices.push_back({ x4,  y4,  z4, p4n[0], p4n[1], p4n[2], tex1, tez2 });

            
            vertices.push_back({ x2,  y2,  z2, p2n[0], p2n[1], p2n[2], tex2, tez1 });
            vertices.push_back({ x3,  y3,  z3, p3n[0], p3n[1], p3n[2], tex2, tez2 });
            vertices.push_back({ x4,  y4,  z4, p4n[0], p4n[1], p4n[2], tex1, tez2 });
        }
    }
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}

//### CURVAS ####

void multMatrixVector(float m[4][4], float* v, float* res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }

}

void multMatrixMatrix(float a[4][4], float b[4][4], float res[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            res[i][j] = 0;
            for (int k = 0; k < 4; k++)
                res[i][j] += a[i][k] * b[k][j];
        }
}

float be(float U, float V, float m[4][4]) {

    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V, 3);
    v[1] = powf(V, 2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m, v, aux);

    r = powf(U, 3) * aux[0] + powf(U, 2) * aux[1] + U * aux[2] + aux[3];



    return r;

}


float Du(float U, float V, float m[4][4]) {

    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V, 3);
    v[1] = powf(V, 2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m, v, aux);

    r = 3 * powf(U, 2) * aux[0] + 2 * U * aux[1] + aux[2];



    return r;

}

float Dv(float U, float V, float m[4][4]) {

    float aux[4];
    float v[4];
    float r;

    v[0] = 3 * powf(V, 2);
    v[1] = V * 2;
    v[2] = 1;
    v[3] = 0;

    multMatrixVector(m, v, aux);

    r = powf(U, 3) * aux[0] + powf(U, 2) * aux[1] + U * aux[2] + aux[3];



    return r;

}

void generateSurface(float mx[4][4], float my[4][4], float mz[4][4], int tesselation, std::vector<Vertex>& vertices) {
    //std::vector<Vertex> vertices;

    float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    float tesselation_level = 1.0 / tesselation;

    //normais 
    float p1u[3], p1v[3], p1n[3], p2u[3], p2v[3], p2n[3], p3u[3], p3v[3], p3n[3], p4u[3], p4v[3], p4n[3];

    for (float i = 0; i < 1; i += tesselation_level) {
        for (float j = 0; j < 1; j += tesselation_level) {
            x1 = be(i, j, mx);
            x2 = be(i + tesselation_level, j, mx);
            x3 = be(i + tesselation_level, j + tesselation_level, mx);
            x4 = be(i, j + tesselation_level, mx);

            p1u[0] = Du(i, j, mx);
            p1v[0] = Dv(i, j, mx);
            p2u[0] = Du(i + tesselation_level, j, mx);
            p2v[0] = Dv(i + tesselation_level, j, mx);
            p3u[0] = Du(i + tesselation_level, j + tesselation_level, mx);
            p3v[0] = Dv(i + tesselation_level, j + tesselation_level, mx);
            p4u[0] = Du(i, j + tesselation_level, mx);
            p4v[0] = Dv(i, j + tesselation_level, mx);

            y1 = be(i, j, my);
            y2 = be(i + tesselation_level, j, my);
            y3 = be(i + tesselation_level, j + tesselation_level, my);
            y4 = be(i, j + tesselation_level, my);

            p1u[1] = Du(i, j, my);
            p1v[1] = Dv(i, j, my);
            p2u[1] = Du(i + tesselation_level, j, my);
            p2v[1] = Dv(i + tesselation_level, j, my);
            p3u[1] = Du(i + tesselation_level, j + tesselation_level, my);
            p3v[1] = Dv(i + tesselation_level, j + tesselation_level, my);
            p4u[1] = Du(i, j + tesselation_level, my);
            p4v[1] = Dv(i, j + tesselation_level, my);

            z1 = be(i, j, mz);
            z2 = be(i + tesselation_level, j, mz);
            z3 = be(i + tesselation_level, j + tesselation_level, mz);
            z4 = be(i, j + tesselation_level, mz);

            p1u[2] = Du(i, j, mz);
            p1v[2] = Dv(i, j, mz);
            p2u[2] = Du(i + tesselation_level, j, mz);
            p2v[2] = Dv(i + tesselation_level, j, mz);
            p3u[2] = Du(i + tesselation_level, j + tesselation_level, mz);
            p3v[2] = Dv(i + tesselation_level, j + tesselation_level, mz);
            p4u[2] = Du(i, j + tesselation_level, mz);
            p4v[2] = Dv(i, j + tesselation_level, mz);


            cross(p1u, p1v, p1n);
            cross(p2u, p2v, p2n);
            cross(p3u, p3v, p3n);
            cross(p4u, p4v, p4n);

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);

            float tex1 = j;
            float tex2 = j + tesselation_level;
            float tez1 = i;
            float tez2 = i + tesselation_level;


            vertices.push_back({ x1,  y1,  z1, p1n[0], p1n[1], p1n[2], tex1, tez1 });
            vertices.push_back({ x2,  y2,  z2, p2n[0], p2n[1], p2n[2], tex1, tez2 });
            vertices.push_back({ x4,  y4,  z4, p4n[0], p4n[1], p4n[2], tex2, tez2 });



            vertices.push_back({ x2,  y2,  z2, p2n[0], p2n[1], p2n[2], tex2, tez2 });
            vertices.push_back({ x3,  y3,  z3, p3n[0], p3n[1], p3n[2], tex2, tez1 });
            vertices.push_back({ x4,  y4,  z4, p4n[0], p4n[1], p4n[2], tex1, tez1 });


        }
    }
}

void createBezier(const char* input, int tesselation, const char* filename) {
    std:: ifstream infile("..//..//ficheiros//patches//" + (std::string)input);
    std:: vector<std::vector<int>> indices;
    std:: vector<Vertex> point;
    float mx[4][4];
    float my[4][4];
    float mz[4][4];
    int num_patches, num_points;
    float bezierMat[4][4] = { {-1.0f,  3.0f, -3.0f, 1.0f},
                             { 3.0f, -6.0f,  3.0f, 0.0f},
                             {-3.0f,  3.0f,  0.0f, 0.0f},
                             { 1.0f,  0.0f,  0.0f, 0.0f} };

    infile >> num_patches;

    for (int i = 0; i < num_patches; i++) {
        std::vector<int> vi;
        int p;
        std::string virg;
        for (int j = 0; j < 16; j++) {
            infile >> p;
            if (j != 15)
                infile >> virg;
            vi.push_back(p);
        }
        indices.push_back(vi);
    }

    infile >> num_points;

    for (int i = 0; i < num_points; i++) {
        float x, y, z;
        std::string virg;
        infile >> x;
        infile >> virg;
        infile >> y;
        infile >> virg;
        infile >> z;

        point.push_back({ x, y, z });
    }

    float aux[4][4];

    std::vector<Vertex> vertices;

    for (std::vector<int> indice : indices) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mx[j][i] = point[indice[i * 4 + j]].x;
                my[j][i] = point[indice[i * 4 + j]].y;
                mz[j][i] = point[indice[i * 4 + j]].z;
            }
        }
        multMatrixMatrix(bezierMat, mx, aux);
        multMatrixMatrix(aux, bezierMat, mx);

        multMatrixMatrix(bezierMat, my, aux);
        multMatrixMatrix(aux, bezierMat, my);

        multMatrixMatrix(bezierMat, mz, aux);
        multMatrixMatrix(aux, bezierMat, mz);

        generateSurface(mx, my, mz, tesselation, vertices);
    }
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << " " << v.n1 << " " << v.n2 << " " << v.n3 << " " << v.t1 << " " << v.t2 << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();



}


