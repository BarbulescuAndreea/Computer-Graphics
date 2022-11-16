#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

     protected:
        float cx, cy;
        glm::mat3 modelMatrix, modelMatrix2, modelMatrix3, modelMatrix4, modelMatrix5, modelMatrix6, modelMatrix7, modelMatrix8, modelMatrix9, modelMatrix10;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        float change, change2, change3;
        bool right = true, alive = true, escapeRata = false, genNewDuck = true, hitDuck = false;
        float rotateVal;
        glm::vec2 direction;
        glm::vec2 position;
        glm::mat4 realMatrix, realMatrix2;
        float xmax, ymax;
        int scaleRef,scaleRef2, nrofDuck = 0, scor;
        int gloante, jucatorLives, duckLives;
        double duration;
        float unghi, speedus;
    };
}   // namespace m1
