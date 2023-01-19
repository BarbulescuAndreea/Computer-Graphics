#include "lab_m1/Tema2/Tema2.h"
#include <vector>
#include <string>
#include <iostream>
#include <glm/gtc/random.hpp>
#include <chrono>
#include <thread>
using namespace std;
using namespace m1;

Tema2::Tema2()
{
}
Tema2::~Tema2()
{
}

void Tema2::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO); //initializare - cate declar : 1, si adresa unde declar
    glBindVertexArray(VAO); // uneste VBO cu IBO

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // gl_array_buffer - e vb de un VBO 
    // creez bufferul: tipul, dimensiunea buffer, de unde incepe sa scrie, ramane aceiasi forma
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);


    unsigned int IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // gl_array_buffer - e vb de un VBO 
    // creez bufferul: tipul, dimensiunea buffer, de unde incepe sa scrie, ramane aceiasi forma
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Check for OpenGL errors
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}

// verificare daca masina este pe pista
// se calculeaza aria triunghiurilor pistei
double triangleArea(glm::vec3 A, glm::vec3 B, glm::vec3 C){
    glm::vec2 AB = glm::vec2(B.x - A.x, B.z - A.z);
    glm::vec2 AC = glm::vec2(C.x - A.x, C.z - A.z);
    double crossProduct = AB.x * AC.y - AB.y * AC.x;
    return abs(crossProduct) / 2;
}
// si se verifica daca masina este cu coordonatele in aceasta arie, daca este inseamna ca e pe pista
bool insideTriangle(glm::vec3 Point, glm::vec3 A, glm::vec3 B, glm::vec3 C){
    double triArea = triangleArea(A, B, C);
    double areaSum = 0;
    areaSum = areaSum + triangleArea(A, B, Point);
    areaSum = areaSum + triangleArea(A, C, Point);
    areaSum = areaSum + triangleArea(B, C, Point);
    // val aproximativa, precizie de 000001
    if(abs(triArea - areaSum) < .000001){
        return true;
    }else{
        return false;
    }
}

void Tema2::Init()
{   
    polygonMode = GL_FILL;
    camera = new implemented::T2Camera();
    camera->distanceToTarget -= 1;
    camera->Set(glm::vec3(1.8, 0, 2.4), glm::vec3(1, 0, 2), glm::vec3(0,1,0)); // modific camera set cu pozitia masinii initiale, si centrul aprox pe ea
        // ca sa fie camera aprox pe masina
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    directions.x = 0.001;
    directions.z = 0;
    directions.y = 0; // directia pe y e 0 - nu se duce in sus
    directions = glm::normalize(directions);
    position = glm::vec3(1.7, 0, 2.2); 
    rotation = glm::vec3(0, 60, 0);

    glm::ivec2 resolution = window->GetResolution();

    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f); // setez dimensiunile viewport ului, cat de sus/jos sa fie, si ce 
                        // lungime/latime sa aiba
    nextDir = {glm::vec3(1.72, 0, 2.31), glm::vec3(2.79, 0, 2.34), glm::vec3(4.21, 0, 1.4), glm::vec3(4.86, 0, 0.29), glm::vec3(4.3, 0, -1.1), glm::vec3(3.5, 0, -1.4),
            glm::vec3(2.52, 0, -0.85), glm::vec3(1.4, 0, -0.47),glm::vec3(0.32, 0, -0.85), glm::vec3(-0.64, 0, -1.73), glm::vec3(-2, 0, -2),
            glm::vec3(-3.45, 0, -1.05), glm::vec3(-3.93, 0, -0.19), glm::vec3(-3.84, 0, 0.97), glm::vec3(-2.76, 0, 1.49), glm::vec3(-1.74, 0, 1.23),
            glm::vec3(-0.46, 0,  0.69), glm::vec3(0.59, 0, 1.25)};
    positObstacol = nextDir[0]; // initial poz obstacol e cea initiala
    start = nextDir[0]; // initial start e prima
    end = nextDir[1]; // end e ultima

    nextDir2 = {glm::vec3(0.06, 0, 1.24), glm::vec3(1, 0, 1.86), glm::vec3(2.24, 0, 2.47), glm::vec3(3.71, 0, 1.96), glm::vec3(4.81, 0, 0.77),
            glm::vec3(4.62, 0, -0.63), glm::vec3(3.93, 0, -1.39),glm::vec3(2.79, 0, -1.14), glm::vec3(1.8, 0, -0.33), glm::vec3(0.7, 0, -0.59),
            glm::vec3(-0.33, 0, -1.69), glm::vec3(-1.83, 0, -2.11), glm::vec3(-3.32, 0, -1.38), glm::vec3(-4, 0, -0.5), glm::vec3(-4.03, 0, 0.78),
            glm::vec3(-3.2, 0,  1.5), glm::vec3(-2.03, 0, 1.6), glm::vec3(-1.32, 0, 1.26)}; 
    positObstacol2 = nextDir2[0];
    start2 = nextDir2[0];
    end2 = nextDir2[1];


    tr1V =
        {
            VertexFormat(glm::vec3(-2.64, 0.1,  1.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.2, 0.8, 0.2)), // S
            VertexFormat(glm::vec3(-1.91, 0.1,  0.82), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // V
            VertexFormat(glm::vec3(-2.89,  0.1,  0.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)) // T
        };
    vector<unsigned int> tr1I =
    {
        0, 1, 2
    };
    CreateMesh("tr1", tr1V, tr1I);
    tr2V =
        {
            VertexFormat(glm::vec3(-2.64, 0.1,  1.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.2, 0.8, 0.2)), // S
            VertexFormat(glm::vec3(-1.91, 0.1,  0.82), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // V
            VertexFormat(glm::vec3(-1.5,  0.1,  1.8), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)) // U
        };
    vector<unsigned int> tr2I =
    {
        0, 1, 2
    };
    CreateMesh("tr2", tr2V, tr2I);
    tr3V =
        {
            VertexFormat(glm::vec3(-1.5,  0.1,  1.8), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)), // U
            VertexFormat(glm::vec3(-0.3, 0.1,  0.39), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Z
            VertexFormat(glm::vec3(-1.91, 0.1,  0.82), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // V
        };
    vector<unsigned int> tr3I =
    {
        0, 1, 2
    };
    CreateMesh("tr3", tr3V, tr3I);
    tr4V =
        {
            VertexFormat(glm::vec3(-1.5,  0.1,  1.8), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)), // U
            VertexFormat(glm::vec3(-0.3, 0.1,  0.39), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Z
            VertexFormat(glm::vec3(-0.72,  0.1,  1.18), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)) // W
        };
    vector<unsigned int> tr4I =
    {
        0, 1, 2
    };
    CreateMesh("tr4", tr4V, tr4I);
    tr5V = 
        {
            VertexFormat(glm::vec3(-0.72,  0.1,  1.18), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)), // W
            VertexFormat(glm::vec3(0.9, 0.1, 0.91), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // B1
            VertexFormat(glm::vec3(-0.3, 0.1,  0.39), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // Z
        };
    vector<unsigned int> tr5I =
    {
        0, 1, 2
    };
    CreateMesh("tr5", tr5V, tr5I);
    tr6V = 
        {
            VertexFormat(glm::vec3(-0.72,  0.1,  1.18), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.1)), // W
            VertexFormat(glm::vec3(0.9, 0.1, 0.91), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // B1
            VertexFormat(glm::vec3(0.29, 0.1,  1.58), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // A1
        };
    vector<unsigned int> tr6I =
    {
        0, 1, 2
    };
    CreateMesh("tr6", tr6V, tr6I);
    tr7V = 
        {
            VertexFormat(glm::vec3(0.29, 0.1,  1.58), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // A1
            VertexFormat(glm::vec3(1.59, 0.1, 1.72), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D1
            VertexFormat(glm::vec3(0.9, 0.1, 0.91), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // B1
        };
    vector<unsigned int> tr7I =
    {
        0, 1, 2
    };
    CreateMesh("tr7", tr7V, tr7I);
    tr8V =
        {
            VertexFormat(glm::vec3(0.29, 0.1,  1.58), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // A1
            VertexFormat(glm::vec3(1.59, 0.1, 1.72), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D1
            VertexFormat(glm::vec3(1.41, 0.1,  2.73), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // C1
        };
    vector<unsigned int> tr8I =
    {
        0, 1, 2
    };
    CreateMesh("tr8", tr8V, tr8I);
    tr9V =
        {
            VertexFormat(glm::vec3(1.41, 0.1,  2.73), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) ,// C1
            VertexFormat(glm::vec3(2.54, 0.1, 1.96), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // F1
            VertexFormat(glm::vec3(1.59, 0.1, 1.72), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D1
        };
    vector<unsigned int> tr9I =
    {
        0, 1, 2
    };
    CreateMesh("tr9", tr9V, tr9I);
    tr10V =
        {
            VertexFormat(glm::vec3(1.41, 0.1,  2.73), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) ,// C1
            VertexFormat(glm::vec3(2.54, 0.1, 1.96), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // F1
            VertexFormat(glm::vec3(3.07, 0.1,  2.76), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // E1
        };
    vector<unsigned int> tr10I =
    {
        0, 1, 2
    };
    CreateMesh("tr10", tr10V, tr10I);
    tr11V =
        {
            VertexFormat(glm::vec3(3.07, 0.1,  2.76), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // E1
            VertexFormat(glm::vec3(3.82, 0.1, 1.17), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // H1
            VertexFormat(glm::vec3(2.54, 0.1, 1.96), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // F1
        };
    vector<unsigned int> tr11I =
    {
        0, 1, 2
    };
    CreateMesh("tr11", tr11V, tr11I);
    tr12V =
        {
            VertexFormat(glm::vec3(3.07, 0.1,  2.76), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // E1
            VertexFormat(glm::vec3(3.82, 0.1, 1.17), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // H1
            VertexFormat(glm::vec3(4.63, 0.1, 1.65), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // G1
        };
    vector<unsigned int> tr12I =
    {
        0, 1, 2
    };
    CreateMesh("tr12", tr12V, tr12I);
    tr13V =
        {
            VertexFormat(glm::vec3(4.63, 0.1, 1.65), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // G1
            VertexFormat(glm::vec3(4.46, 0.1, 0.45), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // J1
            VertexFormat(glm::vec3(3.82, 0.1, 1.17), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // H1
        };
    vector<unsigned int> tr13I =
    {
        0, 1, 2
    };
    CreateMesh("tr13", tr13V, tr13I);
    tr14V = 
        {
            VertexFormat(glm::vec3(4.63, 0.1, 1.65), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // G1
            VertexFormat(glm::vec3(4.46, 0.1, 0.45), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // J1
            VertexFormat(glm::vec3(5.23, 0.1, 0.14), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // I1
            
        };
    vector<unsigned int> tr14I =
    {
        0, 1, 2
    };
    CreateMesh("tr14", tr14V, tr14I);
    tr15V = 
        {
            VertexFormat(glm::vec3(5.23, 0.1, 0.14), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // I1
            VertexFormat(glm::vec3(4.11, 0.1, -0.6), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // K1
            VertexFormat(glm::vec3(4.46, 0.1, 0.45), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // J1
        };
    vector<unsigned int> tr15I =
    {
        0, 1, 2
    };
    CreateMesh("tr15", tr15V, tr15I);
    tr16V = 
        {
            VertexFormat(glm::vec3(5.23, 0.1, 0.14), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // I1
            VertexFormat(glm::vec3(4.11, 0.1, -0.6), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // K1
            VertexFormat(glm::vec3(4.44, 0.1, -1.48), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // L1
        };
    vector<unsigned int> tr16I =
    {
        0, 1, 2
    };
    CreateMesh("tr16", tr16V, tr16I);
    tr17V = 
        {
            VertexFormat(glm::vec3(4.44, 0.1, -1.48), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // L1
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // F2
            VertexFormat(glm::vec3(4.11, 0.1, -0.6), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2))// K1
        };
    vector<unsigned int> tr17I =
    {
        0, 1, 2
    };
    CreateMesh("tr17", tr17V, tr17I);
    tr18V =
        {
            VertexFormat(glm::vec3(4.44, 0.1, -1.48), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // L1
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // F2
            VertexFormat(glm::vec3(3.31, 0.1, -1.74), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // E2
        };
    vector<unsigned int> tr18I =
    {
        0, 1, 2
    };
    CreateMesh("tr18", tr18V, tr18I);
    tr19V = 
        {
            VertexFormat(glm::vec3(3.31, 0.1, -1.74), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // E2
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // M1
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2))  // F2
        };
    vector<unsigned int> tr19I =
    {
        0, 1, 2
    };
    CreateMesh("tr19", tr19V, tr19I);
    tr20V = 
        {
            VertexFormat(glm::vec3(3.31, 0.1, -1.74), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // E2
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // M1
            VertexFormat(glm::vec3(2.37, 0.1, -1.31), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2))  // N1
        };
    vector<unsigned int> tr20I =
    {
        0, 1, 2
    };
    CreateMesh("tr20", tr20V, tr20I);
    tr21V =
        {
            VertexFormat(glm::vec3(2.37, 0.1, -1.31), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // N1
            VertexFormat(glm::vec3(1.23, 0.1, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // O1
            VertexFormat(glm::vec3(3.76, 0.1, -0.93), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // M1
        };
    vector<unsigned int> tr21I =
    {
        0, 1, 2
    };
    CreateMesh("tr21", tr21V, tr21I);
    tr22V = 
        {
            VertexFormat(glm::vec3(2.37, 0.1, -1.31), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // N1
            VertexFormat(glm::vec3(1.23, 0.1, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // O1
            VertexFormat(glm::vec3(1.56, 0.1, -0.94), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // P1
        };
    vector<unsigned int> tr22I =
    {
        0, 1, 2
    };
    CreateMesh("tr22", tr22V, tr22I);
    tr23V = 
        {
            VertexFormat(glm::vec3(1.56, 0.1, -0.94), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // P1
            VertexFormat(glm::vec3(0.1, 0.1, -0.61), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Q1
            VertexFormat(glm::vec3(1.23, 0.1, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // O1
        };
    vector<unsigned int> tr23I =
    {
        0, 1, 2
    };
    CreateMesh("tr23", tr23V, tr23I);
    tr24V =
        {
            VertexFormat(glm::vec3(1.56, 0.1, -0.94), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // P1
            VertexFormat(glm::vec3(0.1, 0.1, -0.61), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Q1
            VertexFormat(glm::vec3(0.57, 0.1, -1.12), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // R1
        };
    vector<unsigned int> tr24I =
    {
        0, 1, 2
    };
    CreateMesh("tr24", tr24V, tr24I);
    tr25V = 
        {
            VertexFormat(glm::vec3(0.57, 0.1, -1.12), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // R1
            VertexFormat(glm::vec3(-0.73, 0.1, -1.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // S1
            VertexFormat(glm::vec3(0.1, 0.1, -0.61), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Q1
        };
    vector<unsigned int> tr25I =
    {
        0, 1, 2
    };
    CreateMesh("tr25", tr25V, tr25I);

    tr26V = 
        {
            VertexFormat(glm::vec3(0.57, 0.1, -1.12), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // R1
            VertexFormat(glm::vec3(-0.73, 0.1, -1.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // S1
            VertexFormat(glm::vec3(-0.54, 0.1, -2.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // T1
        };
    vector<unsigned int> tr26I =
    {
        0, 1, 2
    };
    CreateMesh("tr26", tr26V, tr26I);
    tr27V =
        {
            VertexFormat(glm::vec3(-0.54, 0.1, -2.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // T1
            VertexFormat(glm::vec3(-1.72, 0.1, -1.57), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // U1
            VertexFormat(glm::vec3(-0.73, 0.1, -1.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // S1
        };
    vector<unsigned int> tr27I =
    {
        0, 1, 2
    };
    CreateMesh("tr27", tr27V, tr27I);
    tr28V =
        {
            VertexFormat(glm::vec3(-0.54, 0.1, -2.26), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // T1
            VertexFormat(glm::vec3(-1.72, 0.1, -1.57), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // U1
            VertexFormat(glm::vec3(-2.31, 0.1, -2.47), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // V1
        };
    vector<unsigned int> tr28I =
    {
        0, 1, 2
    };
    CreateMesh("tr28", tr28V, tr28I);
    tr29V =
        {
            VertexFormat(glm::vec3(-2.31, 0.1, -2.47), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // V1
            VertexFormat(glm::vec3(-3.14, 0.1, -0.88), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // W1
            VertexFormat(glm::vec3(-1.72, 0.1, -1.57), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // U1
        };
    vector<unsigned int> tr29I =
    {
        0, 1, 2
    };
    CreateMesh("tr29", tr29V, tr29I);
    tr30V =
        {
            VertexFormat(glm::vec3(-2.31, 0.1, -2.47), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // V1
            VertexFormat(glm::vec3(-3.14, 0.1, -0.88), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // W1
            VertexFormat(glm::vec3(-3.79, 0.1, -1.24), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)) // Z1
        };
    vector<unsigned int> tr30I =
    {
        0, 1, 2
    };
    CreateMesh("tr30", tr30V, tr30I);
    tr31V =
        {
            VertexFormat(glm::vec3(-3.79, 0.1, -1.24), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Z1
            VertexFormat(glm::vec3(-3.6, 0.1, -0.22), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // B2
            VertexFormat(glm::vec3(-3.14, 0.1, -0.88), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // W1
        };
    vector<unsigned int> tr31I =
    {
        0, 1, 2
    };
    CreateMesh("tr31", tr31V, tr31I);

    tr32V =
        {
            VertexFormat(glm::vec3(-3.79, 0.1, -1.24), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // Z1
            VertexFormat(glm::vec3(-3.6, 0.1, -0.22), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // B2
            VertexFormat(glm::vec3(-4.4, 0.1, -0.15), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // A2
        };
    vector<unsigned int> tr32I =
    {
        0, 1, 2
    };
    CreateMesh("tr32", tr32V, tr32I);
    tr33V =
        {
            VertexFormat(glm::vec3(-4.4, 0.1, -0.15), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // A2
            VertexFormat(glm::vec3(-3.66, 0.1, 0.59), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D2
            VertexFormat(glm::vec3(-3.6, 0.1, -0.22), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // B2
        };
    vector<unsigned int> tr33I =
    {
        0, 1, 2
    };
    CreateMesh("tr33", tr33V, tr33I);
    tr34V =
        {
            VertexFormat(glm::vec3(-4.4, 0.1, -0.15), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // A2
            VertexFormat(glm::vec3(-3.66, 0.1, 0.59), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D2
            VertexFormat(glm::vec3(-4.03, 0.1, 1.37), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // C2
        };
    vector<unsigned int> tr34I =
    {
        0, 1, 2
    };
    CreateMesh("tr34", tr34V, tr34I);
    tr35V =
        {
            VertexFormat(glm::vec3(-4.03, 0.1, 1.37), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // C2
            VertexFormat(glm::vec3(-2.89, 0.1, 0.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // T
            VertexFormat(glm::vec3(-3.66, 0.1, 0.59), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // D2
        };
    vector<unsigned int> tr35I =
    {
        0, 1, 2
    };
    CreateMesh("tr35", tr35V, tr35I);
    tr36V =
        {
            VertexFormat(glm::vec3(-4.03, 0.1, 1.37), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // C2
            VertexFormat(glm::vec3(-2.89, 0.1, 0.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // T
            VertexFormat(glm::vec3(-2.64, 0.1, 1.97), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.9, 0.4, 0.2)), // S
        };
    vector<unsigned int> tr36I =
    {
        0, 1, 2
    };
    CreateMesh("tr36", tr36V, tr36I);
    // iarba va fi formata din 4 patrate
     vector<VertexFormat> patrat1
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-25, 0,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-25, 0,  25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6))
    };
    vector<unsigned int> patrat1I =
    {
        0, 1, 2,
        2, 3, 0
    };
    CreateMesh("iarba1", patrat1, patrat1I);
    vector<VertexFormat> patrat2
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(25, 0,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(25, 0,  -25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  -25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6))
    };
    vector<unsigned int> patrat2I =
    {
        0, 1, 2,
        2, 3, 0
    };
    CreateMesh("iarba2", patrat2, patrat2I);
    vector<VertexFormat> patrat3
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(25, 0,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(25, 0,  25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6))
    };
    vector<unsigned int> patrat3I =
    {
        0, 1, 2,
        2, 3, 0
    };
    CreateMesh("iarba3", patrat3, patrat3I);
    vector<VertexFormat> patrat4
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-25, 0,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-25, 0,  -25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 0,  -25), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6))
    };
    vector<unsigned int> patrat4I =
    {
        0, 1, 2,
        2, 3, 0
    };
    CreateMesh("iarba4", patrat4, patrat4I);
    vector<VertexFormat> trunchiV
        {
            VertexFormat(glm::vec3(-0.1, -0.6,  0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(0.1, -0.6,  0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-0.1,  0.6,  0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3( 0.1,  0.6,  0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-0.1, -0.6, -0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3( 0.1, -0.6, -0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-0.1,  0.6, -0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3( 0.1,  0.6, -0.15), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.1, 0.5, 0.8)),
        };
    vector<unsigned int> trunchiI =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
    CreateMesh("trunchi", trunchiV, trunchiI);
    vector<VertexFormat> coroanaV
        {
            VertexFormat(glm::vec3(-0.25, -0.6,  0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(0.25, -0.6,  0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-0.25,  0.05,  0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3( 0.25,  0.05,  0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-0.25, -0.6, -0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3( 0.25, -0.6, -0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-0.25,  0.05, -0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3( 0.25,  0.05, -0.35), glm::vec3(0.0,0.5,0.0), glm::vec3(0.1, 0.5, 0.8)),
        };
        vector<unsigned int> coroanaI =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
    CreateMesh("coroana", coroanaV, coroanaI);
    vector<VertexFormat> obstacoleV
        {
            VertexFormat(glm::vec3(-0.05, -0.2,  0.1), glm::vec3(1, 0, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(0.05, -0.2,  0.1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-0.05,  0.05,  0.1), glm::vec3(1, 0, 1), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3( 0.05,  0.05,  0.1), glm::vec3(1, 0, 1), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-0.05, -0.2, -0.1), glm::vec3(1, 0, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3( 0.05, -0.2, -0.1), glm::vec3(1, 0, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-0.05,  0.05, -0.1), glm::vec3(1, 0, 1), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3( 0.05,  0.05, -0.1), glm::vec3(1, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };
        vector<unsigned int> obstacoleI =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
    CreateMesh("masina", obstacoleV, obstacoleI);

    vector<VertexFormat> obstacoleadvV
        {
            VertexFormat(glm::vec3(-0.05, -0.2,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(0.05, -0.2,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-0.05,  0.02,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3( 0.05,  0.02,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-0.05, -0.2, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3( 0.05, -0.2, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-0.05,  0.02, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3( 0.05,  0.02, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.1, 0.5, 0.8)),
        };
        vector<unsigned int> obstacoleadvI =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
    CreateMesh("obstacol", obstacoleadvV, obstacoleadvI);

    vector<VertexFormat> obstacoleadv2V
        {
            VertexFormat(glm::vec3(-0.05, -0.2,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(0.05, -0.2,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-0.05,  0.02,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3( 0.05,  0.02,  0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-0.05, -0.2, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3( 0.05, -0.2, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-0.05,  0.02, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3( 0.05,  0.02, -0.05), glm::vec3(1, 1, 0), glm::vec3(0.1, 0.5, 0.8)),
        };
        vector<unsigned int> obstacoleadv2I =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
    CreateMesh("obstacol2", obstacoleadv2V, obstacoleadv2I);
}

void Tema2::FrameStart()
{
    glClearColor(0, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene() {

    glm::mat4 modelMatrix3, modelMatrix4;
    glm::mat4 modelMatrix2 = glm::mat4(1);
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        RenderMesh(meshes["tr1"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr2"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr3"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr4"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr5"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr6"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr7"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr8"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr8"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr9"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr10"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr11"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr12"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr13"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr14"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr15"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr16"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr17"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr18"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr19"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr20"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr21"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr22"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr23"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr24"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr25"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr26"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr27"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr28"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr29"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr30"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr31"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr32"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr33"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr34"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr35"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["tr36"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["iarba1"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["iarba2"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["iarba3"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["iarba4"], shaders["VertexColor"], modelMatrix);
    }
    {   
        // copaci
        // 1
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, 1.8));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, 1.85));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 2
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, -2.45));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-40.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, -2.4));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-40.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 3
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(5.4, 0, -0.5));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(5.4, 0, -0.5));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 4
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-4.7, 0, 0));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(15.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-4.7, 0, 0));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(15.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 5
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(3.6, 0, 2.8));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(3.6, 0, 2.8));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 6
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, -2.6));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(130.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, -2.6));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(130.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 7
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, 2.2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-100.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, 2.2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-100.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 8
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, -0.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-2.7, 0, -0.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(30.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 9
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.7, 0, -0.2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(20.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.7, 0, -0.2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(20.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 10
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.1, 0, 1.4));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-105.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2.1, 0, 1.4));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-105.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 11
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-1.1, 0, 2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-1.1, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 0, 2));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 12
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(3.6, 0, 1));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(3.6, 0, 1));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 13
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-3.9, 0, -1.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-3.9, 0, -1.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(-45.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
        // 14
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2, 0, -1.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(110.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["trunchi"], shaders["VertexColor"], modelMatrix2);
        modelMatrix2 = glm::mat4(1);
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0, 1.1, 0));
        modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(2, 0, -1.7));
        modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(110.0f), glm::vec3(0, 1, 0));
        RenderMesh(meshes["coroana"], shaders["VertexColor"], modelMatrix2);
    }
    {   
        // masina randata
        // 0.3 la y ca sa nu fie fix in pista, sa fie putin mai inalta
        modelMatrix3 = glm::mat4(1); 
        // initial masina se afla pe pista intr o pozitie, apoi aceasta este updatata la fiecare moment de timp
        modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(position.x, .3, position.z));
        modelMatrix3 = glm::rotate(modelMatrix3, RADIANS(rotation.y), glm::vec3(0, 1, 0));
        RenderMesh(meshes["masina"], shaders["VertexColor"], modelMatrix3);
    }

    {
        // facere obstacole sa mearga pe un traseu prestabilit
        // start, end sa tina pozitia curenta si pozitia urmatoare din vectorul pozitiilor obstacolului
        modelMatrix3 = glm::mat4(1);
        if(fabs(positObstacol.x - end.x) < 0.01){ 
            i++;
            if(i == 18){ // daca am ajuns la sf vectorului de pozitii prestabilite pt obstacol, atunci o iau de la capat
                i = 0;
            }
            start = nextDir[i];
            end = nextDir[i+1];
            positObstacol = start;
            if(i == 17){ // penultima pozitie din vector
                start = nextDir[17];
                end = nextDir[0];
                positObstacol = start;
            }
        }
     
        positObstacol.x += (end.x - start.x) * 0.01; // unde sa fie obstacolul la fiecare moment de timp
        positObstacol.z += (end.z - start.z) * 0.01;
        modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(positObstacol.x, .3, positObstacol.z));
        RenderMesh(meshes["obstacol"], shaders["VertexNormal"], modelMatrix3);
    }

    {
            // aceiasi pasi si pentru obstacolul 2
        modelMatrix4 = glm::mat4(1);
        if(fabs(positObstacol2.x - end2.x) < 0.01){ 
            j++;
            if(j == 18){
                j = 0;
            }
            start2 = nextDir2[j];
            end2 = nextDir2[j+1];
            positObstacol2 = start2;
            if(j == 17){
                start2 = nextDir2[17];
                end2 = nextDir2[0];
                positObstacol2 = start2;
            }
        }
        positObstacol2.x += (end2.x - start2.x) * 0.012;
        positObstacol2.z += (end2.z - start2.z) * 0.012;
        modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(positObstacol2.x, .3, positObstacol2.z));
        RenderMesh(meshes["obstacol2"], shaders["VertexColor"], modelMatrix4);
    }
}

void Tema2::Update(float deltaTimeSeconds)
{  
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y); // am setat view port ul
    RenderScene(); // l am randat
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
    orthoLeft = position.x + 5.0f; // am ales valori pentru campuri
    orthoRight = position.x - 5.0f;
    orthoDown = position.z - 5.0f;
    orthoUp = position.z + 5.0f;
    zFar = 200.0f;
    zNear = 0.01f; // maresc ft tare ma apropii mult de iarba si nu mai vad altceva , copacii sunt "prea sus" pt la cat de jos e near ul meu
    // Din moment ce se dorește o vedere de ansamblu asupra unei bucăți din traseu, se foloseste proiectie ortografica
    // am nev de ea pt minimap ca harta trebuie sa se vada de sus 2d, fara ortho s ar fi vazut doar o bucatica de traseu
    //  left, right, bottom, top sunt relative față de centrul ferestrei (0, 0) și definesc înălțimea și lățimea ferestrei de proiecție
    projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoDown, orthoUp, zNear, zFar); // am schimbat projection matrix ul pt 
                    // proiectie ortografica
    camera->RotateFirstPerson_OX(-RADIANS(90)); // am rotit camera cu -90 grade ca sa se vada de sus, first person pentru ca e camera  in fct de masina
                // nu din spate
    camera->RotateFirstPerson_OY(-camera->rott.y); // setez rotatia pe celelalte axe - valoarea cu care se roteste camera jos ca sa nu se miste
                    // impreuna cu masina
    camera->RotateFirstPerson_OZ(-camera->rott.z);
    camera->position = position; //setez camera in viewport deasupra masinii
    camera->position.y = 3; // putin mai inalta
    RenderScene(); // randez scena obtinuta in view port
    // setez inapoi projectionMatrix si camera cum erau initial in scena mare
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    camera->RotateFirstPerson_OX(RADIANS(90));
    camera->RotateFirstPerson_OY(camera->rott.y);
    camera->RotateFirstPerson_OZ(camera->rott.z);
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;
    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{   
    float cameraSpeed = 2.0f;
    glm::vec3 cpy = position;
    // verificare distanta pana la obstacol ca sa se opreasca daca se loveste de vreun obstacol
    // daca distanta de la obiect la obstacolul 1/2 e mai are decat 0.06 atunci poate merge, altfel se opreste pt 
    // un anumit timp pw += deltatimeseconds(1 secunda in cazul ales de mine)
    if(glm::distance(position, positObstacol2) > 0.06 && pw == 0 && glm::distance(position, positObstacol) > 0.06){
        //sleep(5);
        if(window->KeyHold(GLFW_KEY_W)){
                cpy += camera->forward * deltaTime * 1.5f;
        }
        if(window->KeyHold(GLFW_KEY_Q)){
                cpy += camera->forward * deltaTime * 2.5f; // speed more
        }
        if(window->KeyHold(GLFW_KEY_S)){
                cpy -= camera->forward * deltaTime * 1.5f;
        }
        if(window->KeyHold(GLFW_KEY_A)){
            // daca se roteste masina, atunci sa se roteasca si camera in spatele ei cu aceiasi valoare
            // rotation pe y ca sa se roteasca obiectul, daca era pe x obiectul nu se rotea obiectul iar camera se rotea in jurul axei ox, adica
            // in susul scenei si venea pe dedesubt
            rotation.y += deltaTime * 60.0f;
            camera->RotateThirdPerson_OY(RADIANS(deltaTime * 60.0f));
        }
        if(window->KeyHold(GLFW_KEY_D)){
            rotation.y -= deltaTime * 60.0f;
            camera->RotateThirdPerson_OY(-RADIANS(deltaTime * 60.0f));
        }
        // pozitionare camera unde trebuie.
        camera->position = position - camera->forward * camera->distanceToTarget;
        camera->position.y = 0.5; // fara 0.5 s ar vedea scena de dedesubt

        // verific la fiecare moment daca masina mea se afla pe pista, daca se afla atunci se poate duce la pozitia noua(cpy);
        vector <vector <VertexFormat>> vec = {tr1V, tr2V, tr3V, tr4V, tr5V, tr6V, tr7V, tr8V, tr9V, tr10V, tr11V, tr12V, tr13V, tr14V, tr15V, tr16V, tr17V,
                                    tr18V, tr19V, tr20V, tr21V, tr22V, tr23V, tr24V, tr25V, tr26V, tr27V, tr28V, tr29V, tr30V, 
                                    tr31V, tr32V, tr33V, tr34V, tr35V, tr36V};
        for(auto & vect : vec){
            if(insideTriangle(glm::vec3(cpy.x, 0, cpy.z), vect[0].position, vect[1].position, vect[2].position) == true){
                // daca se afla pe pista, atunci vechea pozitie primeste noua pozitie pt a se deplasa, daca nu, atunci raman pe loc
                position = cpy;
                break;
            }  
        }
    }else{
        pw += deltaTime;
        camera->position = position - camera->forward * camera->distanceToTarget;
        camera->position.y = 0.5;
        if(pw >= 1){
            pw = 0;
        }
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
}
void Tema2::OnKeyRelease(int key, int mods)
{
}
void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}
void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}
void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}
void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
void Tema2::OnWindowResize(int width, int height)
{
}