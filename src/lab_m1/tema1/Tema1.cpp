#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <math.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/random.hpp>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;
using namespace chrono;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
 }
void Tema1::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
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

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 10));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(2, 2, 0);
    float squareSide = 100;

    xmax = GetSceneCamera()->GetProjectionInfo().width;
    ymax = GetSceneCamera()->GetProjectionInfo().height;

    direction.x = glm::linearRand(0.2f, 1.0f); // generez random o directie in care sa se duca pe x si pe y
    direction.y = glm::linearRand(0.1f, 1.0f);
    direction = glm::normalize(direction); // o normalizez ca sa nu creasca viteza in functie de directie 

    position = glm::vec3(5, -1, 0); // 5 -1 0
    scaleRef = 120;
    scaleRef2 = 40;
    gloante = 3; // la inceput jucatorul are 3 gloante
    jucatorLives = 3;
    duration = 0;
    speedus = 1.5f;
    duckLives = 3;
    scor = 5;

    // CORPUL
    vector<VertexFormat> FirstTriangleV
    {
            // coordonate, normala, culoare
        VertexFormat(glm::vec3(-1.5f, -1, 0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(-1.5f, 1,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(1.5f, 0,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> FirstTriangleI =
    {
        0, 1, 2
    };

    // DREPTUNGHI INCADRARE RATA
    vector<VertexFormat> dreptunghiIncVer
    {
        VertexFormat(glm::vec3(-2, -1, 0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-2, 1,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(2, 1,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(2, -1,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6))
    };
    
    vector<unsigned int> dreptunghiIncInd =
    {
        0, 1, 2,
        0, 2, 3
    };

    // ARIPA DE SUS
    vector<VertexFormat> UpHandTriangleV
    {
        VertexFormat(glm::vec3(-0.5f, 0, 0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0.5f, 0,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, 1.5f,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> UpHandTriangleI =
    {
        0, 1, 2
    };

    // ARIPA DE JOS
    vector<VertexFormat> DownHandTriangleV
    {
        VertexFormat(glm::vec3(-0.5f, 0, 0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0.5f, 0,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, -1.5f,  0), glm::vec3(0.7, 0.4, 0.2), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> DownHandTriangleI =
    {
        0, 1, 2,
        1, 3, 2
    };

    // CIOC 
    vector<VertexFormat> NoseTriangleV
    {
        VertexFormat(glm::vec3(1.8, 0.2, 0), glm::vec3(1, 1, 0), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(1.8, -0.2,  0), glm::vec3(1, 1, 0), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(2.6, 0,  0), glm::vec3(1, 1, 0), glm::vec3(0.2, 0.8, 0.6)) 
    };


    vector<unsigned int> NoseTriangleI =
    {
        0, 1, 2
    };

    vector<VertexFormat> livesV
    {
        VertexFormat(glm::vec3(-1, -1, 0), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0)), 
        VertexFormat(glm::vec3(-1, 1,  0), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(1, 0,  0), glm::vec3(0, 1, 1), glm::vec3(1, 0, 0)) 
    };


    vector<unsigned int> livesI =
    {
        0, 1, 2
    };

    // CAP
    vector <VertexFormat> vertices;
    float x = 4.0f;
    int y = 0, z = 0;// mai putine triunghiuri => se deformeaza cercul, se patrateste pe margini
    float radius = 1.5;
    vertices.push_back(VertexFormat(glm::vec3(4.0f, 0, 0), glm::vec3(0, 0.5, 0), glm::vec3(0.2, 0.6, 0.8))); // centrul
    for(int i = 0; i <= 30; i++) { // am luat nr de triunghiuri sa fie 25
        vertices.push_back(VertexFormat( // adaug la finalul vectorului de vertexi cate unul pe rand care formeaza cate un triunghi ce vin lipite intre ele
        glm::vec3 (x + (radius * cos((float)i * 2 * M_PI / 30)), y + (radius * sin((float)i * 2 * M_PI / 30)), 0), glm::vec3(0, 0.5, 0), glm::vec3(0.2, 0.6, 0.8)));
    }

    vector<unsigned int> indices;
    for(int i = 0; i <= 30; i++){
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }


    vector<VertexFormat> dreptunghi
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(window->GetResolution().x, 0,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)), 
        VertexFormat(glm::vec3(window->GetResolution().x, (window->GetResolution().y) / 4,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6)),
        VertexFormat(glm::vec3(0, (window->GetResolution().y) / 4,  0), glm::vec3(0.5, 1, 0.5), glm::vec3(0.2, 0.8, 0.6))
    };

    vector<unsigned int> dreptunghiI =
    {
        0, 1, 2,
        2, 3, 0
    };
    

    vector<VertexFormat> gloanteV
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(-1, -1, 0), glm::vec3(0, 1, 1), glm::vec3(0.7, 0.8, 0.6)), 
        VertexFormat(glm::vec3(-1, 1,  0), glm::vec3(0, 1, 1), glm::vec3(0.7, 0.8, 0.6)), 
        VertexFormat(glm::vec3(1, 1,  0), glm::vec3(0, 1, 1), glm::vec3(0.7, 0.8, 0.6)),
        VertexFormat(glm::vec3(1, -1,  0), glm::vec3(0, 1, 1), glm::vec3(0.7, 0.8, 0.6))
    };
    
    vector<unsigned int> gloanteI =
    {
        0, 1, 2,
        0, 2, 3
    };

    vector<VertexFormat> dreptunghi2V
    {
        // coordonate, normala, culoare
        VertexFormat(glm::vec3(-1.5, -1.5, 0), glm::vec3(0, 1, 1), glm::vec3(1, 0.4, 0)), 
        VertexFormat(glm::vec3(-1.5, 1.5,  0), glm::vec3(0, 1, 1), glm::vec3(1, 0.4, 0)), 
        VertexFormat(glm::vec3(1.5, 1.5,  0), glm::vec3(0, 1, 1), glm::vec3(1, 0.4, 0)),
        VertexFormat(glm::vec3(1.5, -1.5,  0), glm::vec3(0, 1, 1), glm::vec3(1, 0.4, 0))
    };
    
    vector<unsigned int> dreptunghi2I =
    {
        0, 1, 2,
        0, 2, 3
    };

    meshes["firsttriangle"] = new Mesh("generated triangle 1");
    meshes["firsttriangle"]->InitFromData(FirstTriangleV, FirstTriangleI);

    CreateMesh("firsttriangle", FirstTriangleV, FirstTriangleI);
    CreateMesh("secondtriangle", UpHandTriangleV, UpHandTriangleI);
    CreateMesh("second2triangle", DownHandTriangleV, DownHandTriangleI);
    CreateMesh("second3triangle", NoseTriangleV, NoseTriangleI);
    CreateMesh("circle", vertices, indices);
    CreateMesh("dreptunghi", dreptunghi, dreptunghiI);
    CreateMesh("dreptunghiIncadrare", dreptunghiIncVer, dreptunghiIncInd);
    CreateMesh("lives", livesV, livesI);
    CreateMesh("gloante", gloanteV, gloanteI);
    CreateMesh("scor", dreptunghi2V, dreptunghi2I);

    translateX = 0;
    translateY = 0;

    scaleX = 1;
    scaleY = 1;

    angularStep = 0;

    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    Mesh* square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 1.4, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{   
    if(genNewDuck == true && jucatorLives > 0){ // daca jucatorul nu mai are vieti nu mai apar rate
        unghi = glm::linearRand((double)0.3f, M_PI / 4 - 0.3f); // unghi intre 0.3 si 3.14/2-0.3
        direction.x = 5 * cos(unghi);
        direction.y = 5 * sin(unghi);
        direction = glm::normalize(direction);
        scaleRef = 120;
        scaleRef2 = 40;

        position = glm::vec3(3, 3, 0);
        duration = 0;
        genNewDuck = false;
        alive = true;
        escapeRata = false;
        nrofDuck += 1;
        gloante = 3;
    }
    if(escapeRata){ // daca rata evadeaza sa mearga in sus
        direction.x = 0.0f;
        direction.y = 1.0f;
    }
    position += direction * deltaTimeSeconds * 400.0f * (float)(1 + nrofDuck / 5); // vitezas
    duration += deltaTimeSeconds;

    // dreptunghiul verde de jos 
    modelMatrix6 = glm::mat3(1);
    RenderMesh2D(meshes["dreptunghi"], shaders["VertexColor"], modelMatrix6);

    // lives
    for(int i = 1; i <= jucatorLives; i++){ // vietile
        modelMatrix7 = glm::mat3(1);
        modelMatrix7 *= transform2D::Translate(i * 150, GetSceneCamera()->GetProjectionInfo().height - 100);
        modelMatrix7 *= transform2D::Scale(40, 40);
        RenderMesh2D(meshes["lives"], shaders["VertexNormal"], modelMatrix7);   
    }

    for(int i = 1; i <= gloante; i++){ // vietile
        modelMatrix8 = glm::mat3(1);
        modelMatrix8 *= transform2D::Translate(i * 150, GetSceneCamera()->GetProjectionInfo().height - 230);
        modelMatrix8 *= transform2D::Scale(40, 40);
        RenderMesh2D(meshes["gloante"], shaders["VertexNormal"], modelMatrix8);   
    }

    // dreptunghiul variabil de scor
    modelMatrix10 = glm::mat3(1);
    modelMatrix10 *= transform2D::Translate(150, 1050);
    modelMatrix10 *= transform2D::Translate(-1.5, -0.2);
    if(hitDuck && scor < 65){
        scor = scor + 10;
        hitDuck = false;
    }
    modelMatrix10 *= transform2D::Scale(scor, 20);
    modelMatrix10 *= transform2D::Translate(1.5, 0.2);
    RenderMesh2D(meshes["scor"], shaders["VertexColor"], modelMatrix10);

    // dreptunghiul de contur al scorului
    modelMatrix9 = glm::mat3(1);
    modelMatrix9 *= transform2D::Translate(250, 1050);
    modelMatrix9 *= transform2D::Scale(80, 30);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // render mode ul wireframe
    RenderMesh2D(meshes["scor"], shaders["VertexNormal"], modelMatrix9);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    

    // asezare cap
    modelMatrix5 = glm::mat3(1);
    modelMatrix5 *= transform2D::Translate(position.x, position.y); // translate urile in functie de pozitie 
    modelMatrix5 *= transform2D::Scale(scaleRef2, 40);
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix5);

    // ASEZARE CORP
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(position.x, position.y);
    modelMatrix *= transform2D::Scale(scaleRef, 120);
    RenderMesh2D(meshes["firsttriangle"], shaders["VertexColor"], modelMatrix);
    
    // ASEZARE ARIPA SUS
    modelMatrix2 = glm::mat3(1);
    modelMatrix2 *= transform2D::Translate(position.x, position.y);
    modelMatrix2 *= transform2D::Scale(scaleRef, 120);
    

    if(right){ // rotatie stanga dreapta aripi
        rotateVal += deltaTimeSeconds * 15;
        if(rotateVal > 17){
            right = false;
        }
    }else{
        rotateVal -= deltaTimeSeconds * 15;
        if(rotateVal < -17){
            right = true;
        }
    }
    modelMatrix2 *= transform2D::Rotate(glm::radians(rotateVal));
    RenderMesh2D(meshes["secondtriangle"], shaders["VertexColor"], modelMatrix2);


    // ASEZARE ARIPA JOS 
    modelMatrix3 = glm::mat3(1);
    modelMatrix3 *= transform2D::Translate(position.x, position.y);
    modelMatrix3 *= transform2D::Scale(scaleRef, 120);
    modelMatrix3 *= transform2D::Rotate(glm::radians(rotateVal));
    RenderMesh2D(meshes["second2triangle"], shaders["VertexColor"], modelMatrix3);

    // ASEZARE CIOC
    modelMatrix4 = glm::mat3(1);
    modelMatrix4 *= transform2D::Translate(position.x, position.y);
    modelMatrix4 *= transform2D::Scale(scaleRef, 120);

    RenderMesh2D(meshes["second3triangle"], shaders["VertexColor"], modelMatrix4);
    if(alive == false){ // daca a murit rata
        if(position.y <= 0){
            escapeRata = false;
            genNewDuck = true;
        }
    }else if(gloante != 0 && duration < 13){ // daca mai sunt gloante dar timpul maxim nu a trecut inca
                                            // rata e inca in viata, mai poate fi impuscata, ramane pe ecran

        if(position.x >= xmax){ // iese din ecran prin dreapta 
            direction = glm::reflect(direction, glm::vec2(-1, 0)); 
            scaleRef *= -1;
            scaleRef2 *= -1;
        }
        if(position.x <=  0){ // iese din ecran prin stanga
            direction = glm::reflect(direction, glm::vec2(1, 0));
            scaleRef *= -1;
            scaleRef2 *= -1;
        }
        if(position.y + 10 >= ymax){ // iese din ecran pe sus
            direction = glm::reflect(direction, glm::vec2(0, -1));
        }
        if(position.y <= 0){ // iese din ecran pe jos
            direction = glm::reflect(direction, glm::vec2(0, 1));
        }
    }else if(((gloante != 0 && duration > 13) || (gloante == 0 && alive == true)) && escapeRata == false){ // daca mai sunt gloante si timpul maxim a trecut SAU
                                                                                            //nu mai am gloante dar rata e inca in viata(nu a fost nimerita)                                                                                  
        jucatorLives--; // rata scapa => jucatorul pierde, ii scade viata
        escapeRata = true;
    }else if(escapeRata == true && position.y >= ymax){ // daca rata a scapat si a si iesit din ecran
        escapeRata = false;
        genNewDuck = true;
    }

}

void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    mouseX = 2 * mouseX;
    mouseY = 2 * (window->GetResolution().y / 2.f - mouseY);
    if( mouseX > position.x - 200 && mouseX < position.x + 200 && mouseY < position.y + 200 && mouseY > position.y - 200){ // o nimereste
        alive = false;
        hitDuck = true;
        direction.x = 0.0f;
        direction.y = -1.0f;
    }// daca a impuscat dar nu a nimerit rata
    gloante -= 1;
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}
