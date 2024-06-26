#include "funcgera.h"
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Precisa: " << argv[0] << " <primitive> [parameters...] <outputFile>" << std::endl;
        return 1;
    }

    std::string primitive = argv[1];
    const char* outputFile = argv[argc - 1];

    //caso seja plane 
    if (primitive == "plane") {
        if (argc != 5) {
            std::cout << "Precisa: " << argv[0] << " plane <size> <divisions> <outputFile>" << std::endl;
            return 1;
        }
        int size = std::stoi(argv[2]);
        int division = std::stoi(argv[3]);
        createPlane(size, division, outputFile);
        std::cout << "Sucesso " << std::endl;
        //std::cout << size << std::endl;
    }
    //case seja box
    else if (primitive == "box") {
        if (argc < 4) {
            std::cout << "Precisa: " << argv[0] << " box <dimension> <divisions> <outputFile>" << std::endl;
            return 1;
        }
        float dimension = std::stof(argv[2]);
        int divisions = std::stoi(argv[3]);
        createBox(dimension, divisions, outputFile);
        std::cout << "Sucesso " << std::endl;
    }
    //caso esfera
    else if (primitive == "sphere") {
        if (argc < 5) {
            std::cout << "Precisa: " << argv[0] << " sphere <radius> <slices> <stacks> <outputFile>" << std::endl;
            return 1;
        }
        float radius = std::stof(argv[2]);
        //std::cout << radius << std::endl;
        int slices = std::stoi(argv[3]);
        int stacks = std::stoi(argv[4]);
        createSphere(radius, slices, stacks, outputFile);
        std::cout << "Sucesso " << std::endl;
    }
    //caso cone
    else if (primitive == "cone") {
        if (argc < 6) {
            std::cout << "Precisa: " << argv[0] << " cone <bottomRadius> <height> <slices> <stacks> <outputFile>" << std::endl;
            return 1;
        }
        float bottomRadius = std::stof(argv[2]);
        float height = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        createCone(bottomRadius, height, slices, stacks, outputFile);
        std::cout << "Sucesso " << std::endl;
    }
    //caso torus
    else if (primitive == "torus") {
        if (argc < 6) {
            std::cout << "Precisa: " << argv[0] << " torus <radius> <largura> <slices> <stacks> <outputFile>" << std::endl;
            return 1;
        }
        float radius = std::stof(argv[2]);
        float largura = std::stof(argv[3]);
        int slices = std::stoi(argv[4]);
        int stacks = std::stoi(argv[5]);
        createTorus(radius, largura, slices, stacks, outputFile);
        std::cout << "Sucesso " << std::endl;
    }
    //caso Bezier
    else if (primitive == "bezier") {
        if (argc < 4) {
            std::cout << "Precisa: " << argv[0] << " bezier <inputFile> <tesselation> <outputFile>" << std::endl;
            return 1;
        }
        const char* inputFile = argv[2];
        float tess_level = std::stoi(argv[3]);
        createBezier(inputFile,tess_level,outputFile);
        std::cout << "Sucesso " << std::endl;
    }
    //caso invalido
    else {
        std::cout << "Invalid primitive: " << primitive << std::endl;
        return 1;
    }

    return 0;
}