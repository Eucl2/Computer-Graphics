#include "funcgera.h"
#include <cmath>
#define M_PI 3.14159265358979323846
#include <sstream>
#include <iomanip> 
#include <fstream>


//### ESFERA #####
void createSphere(float radius, int slices, int stacks, const char* filename) {
    std::vector<Vertex> vertices;// armazenar os dados dos vértices

    float x1, x2, x3, x4;
    float y1, y2;
    float z1, z2, z3, z4;
    float sigma = 2*M_PI / slices; //variação angular da slice
    float theta = M_PI / stacks;    //variação angular da stack
    //float zeta = M_PI/ 2;       //ângulo a partir do eixo z



    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            x1 = radius * cos(M_PI/2 - theta * j) * sin(sigma * i);
            x2 = radius * cos(M_PI/2 - theta * (j + 1)) * sin(sigma * i);
            x3 = radius * cos(M_PI/2 - theta * (j + 1)) * sin(sigma * (i + 1));
            x4 = radius * cos(M_PI/2 - theta * j) * sin(sigma * (i + 1));

            y1 = radius * sin(M_PI/2 - theta * j);
            y2 = radius * sin(M_PI/2 - theta * (j + 1));

            z1 = radius * cos(M_PI/2 - theta * j) * cos(sigma * i);
            z2 = radius * cos(M_PI/2 - theta * (j + 1)) * cos(sigma * i);
            z3 = radius * cos(M_PI/2 - theta * (j + 1)) * cos(sigma * (i + 1));
            z4 = radius * cos(M_PI/2 - theta * j) * cos(sigma * (i + 1));

            //vertices para o triangulo superior do quadrilatero
            if (j != stacks - 1) {
                vertices.push_back({ x1, y1,  z1 });
                vertices.push_back({ x2,  y2, z2 });
                vertices.push_back({ x3,  y2, z3 });
            }
            //vertices para o triangulo inferior do quadrilatero
            if (j != 0) {
                vertices.push_back({ x1,  y1,  z1 });
                vertices.push_back({ x3,  y2,  z3 });
                vertices.push_back({ x4,  y1,  z4 });
            }


        }
    }
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << "\n";
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
    
    //faces superior e inferior da box eixo y
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * unidade - metdim;
            z1 = j * unidade - metdim;
            x2 = (i + 1) * unidade - metdim;
            z2 = (j + 1) * unidade - metdim;


            vertices.push_back({ x1,  metdim,  z1 });
            vertices.push_back({ x2,  metdim,  z2 });
            vertices.push_back({ x2,  metdim,  z1 });

  
            vertices.push_back({ x1,  metdim,  z1 });
            vertices.push_back({ x1,  metdim,  z2 });
            vertices.push_back({ x2,  metdim,  z2 });

           
            vertices.push_back({ x2,  -metdim,  z2 });
            vertices.push_back({ x1,  -metdim,  z1 });
            vertices.push_back({ x2,  -metdim,  z1 });

   
            vertices.push_back({ x1,  -metdim,  z2 });
            vertices.push_back({ x1,  -metdim,  z1 });
            vertices.push_back({ x2,  -metdim,  z2 });
        }
    }
  
   
    //face frontal e traseira eixo z
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * unidade - metdim;
            y1 = j * unidade - metdim;
            x2 = (i + 1) * unidade - metdim;
            y2 = (j + 1) * unidade - metdim;

    
            vertices.push_back({ x2,  y2,  metdim });
            vertices.push_back({ x1,  y1,  metdim });   
            vertices.push_back({ x2,  y1,  metdim });

    
            vertices.push_back({ x1,  y2,  metdim });
            vertices.push_back({ x1,  y1,  metdim });
            vertices.push_back({ x2,  y2,  metdim });


            vertices.push_back({ x1,  y1,  -metdim });
            vertices.push_back({ x2,  y2,  -metdim });
            vertices.push_back({ x2,  y1,  -metdim });


            vertices.push_back({ x1,  y1,  -metdim });
            vertices.push_back({ x1,  y2,  -metdim });
            vertices.push_back({ x2,  y2,  -metdim });
        }
    }
    
    //face lateral esquerda e direita eixo do x
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            z1 = i * unidade - metdim;
            y1 = j * unidade - metdim;
            z2 = (i + 1) * unidade - metdim;
            y2 = (j + 1) * unidade - metdim;


            vertices.push_back({ metdim,  y1,  z1 });
            vertices.push_back({ metdim,  y2,  z2 });
            vertices.push_back({ metdim,  y1,  z2 });
            

           
            vertices.push_back({ metdim,  y1,  z1 });
            vertices.push_back({ metdim,  y2,  z1 });
            vertices.push_back({ metdim,  y2,  z2 });


            vertices.push_back({ -metdim,  y2,  z2 });
            vertices.push_back({ -metdim,  y1,  z1 });
            vertices.push_back({ -metdim,  y1,  z2 });


            vertices.push_back({ -metdim,  y2,  z1 });
            vertices.push_back({ -metdim,  y1,  z1 });
            vertices.push_back({ -metdim,  y2,  z2 });
          
        }
    }
    
    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << "\n";
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


    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            x1 = i * dist - metade;
            z1 = j * dist - metade;
            x2 = (i + 1) * dist - metade;
            z2 = (j + 1) * dist - metade;


            vertices.push_back({ x1,  0,  z1 });
            vertices.push_back({ x2,  0,  z2 });
            vertices.push_back({ x2,  0,  z1 });

            
            vertices.push_back({ x1,  0,  z1 });
            vertices.push_back({ x1,  0,  z2 });
            vertices.push_back({ x2,  0,  z2 });
            
        }
    }


    // Escreve os vertices no ficheiro 
    std::ofstream outfile("..//..//ficheiros//modelos//" + (std::string)filename);
    std::stringstream ss;
    //ss << std::fixed << std::setprecision(2); // alterar precisao das casas decimais
    ss << vertices.size() << "\n"; // numero de vertices total
    for (const auto& v : vertices) {
        ss << v.x << " " << v.y << " " << v.z << "\n";
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


    //base do cone
    for (int i = 0; i < slices; i++) {
        x1 = bottomRadius * sin(alfa * i);
        x2 = bottomRadius * sin(alfa * (i + 1));
        z1 = bottomRadius * cos(alfa * i);
        z2 = bottomRadius * cos(alfa * (i + 1));


        vertices.push_back({ x1,  0,  z1 });
        vertices.push_back({ 0,  0,  0 });
        vertices.push_back({ x2,  0,  z2 });
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
            y1 = (i * heightS);
            y2 = (i + 1) * heightS;
            z1 = r1 * cos(alfa * j);
            z2 = r1 * cos(alfa * (j + 1));
            z3 = r2 * cos(alfa * (j + 1));
            z4 = r2 * cos(alfa * j);


            vertices.push_back({ x1,  y1,  z1 });
            vertices.push_back({ x2,  y1,  z2 });
            vertices.push_back({ x4,  y2,  z4 });

            if (j != slices - 1) {

                vertices.push_back({ x4,  y2,  z4 });
                vertices.push_back({ x2,  y1,  z2 });
                vertices.push_back({ x3,  y2,  z3 });
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
        ss << v.x << " " << v.y << " " << v.z << "\n";
    }
    outfile << ss.rdbuf(); // escreve o conteudo da stringstream no ficheiro
    outfile.close();
}
