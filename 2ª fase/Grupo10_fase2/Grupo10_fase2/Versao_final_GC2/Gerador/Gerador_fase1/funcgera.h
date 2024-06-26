#ifndef FUNCGERA_H
#define FUNCGERA_H

#include <vector>
#include <fstream>
#include <iostream>

struct Vertex {
    float x, y, z;
};

void createSphere(float radius, int slices, int stacks, const char* filename);
void createBox(float dim, int divs, const char* filename);
void createPlane(float size, int divisions, const char* filename);
void createCone(float bottomRadius, float height, int slices, int stacks, const char* filename);
void createTorus(float radius, float largura, int slices, int stacks, const char* filename);
#endif