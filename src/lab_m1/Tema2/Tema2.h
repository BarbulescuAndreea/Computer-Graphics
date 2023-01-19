#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema_camera.h"
#include <vector>

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();
        float orthoRight, orthoUp, orthoDown, orthoLeft;
        void Init() override;

        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
         void CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);
        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderScene();

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::T2Camera *camera;
        glm::mat4 projectionMatrix;
        float aspect = window->props.aspectRatio;
        float zNear = 0.01f;
        float zFar = 200.0f;
        glm::vec3 directions, position, rotation, positObstacol, positObstacol2;
        glm::vector<glm::vec3> nextDir, nextDir2;
        glm::vec3 start, end, start2, end2;
        int i = 0, j = 0;
        bool stop = false;
        //glm::vector unghi;

        ViewportArea miniViewportArea;
        GLenum polygonMode;
        glm::vector<VertexFormat> tr1V, tr2V, tr3V, tr4V, tr5V, tr6V, tr7V, tr8V, tr9V, tr10V, tr11V, tr12V, tr13V, tr14V, tr15V, tr16V, tr17V, tr18V, tr19V, tr20V,
        tr21V, tr22V, tr23V, tr24V, tr25V, tr26V, tr27V, tr28V, tr29V, tr30V, tr31V, tr32V, tr33V, tr34V, tr35V, tr36V;
        float pw = 0;
    };
}   // namespace m1