#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "Calculator/MarchingSquares.h"
#include "Calculator/Expression.h"

#include "Engine/Renderer.h"
#include "Engine/VertexBuffer.h"
#include "Engine/IndexBuffer.h"
#include "Engine/VertexArray.h"
#include "Engine/VertexBufferLayout.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Window.h"

int main(void)
{
    std::cout << "Enter an implicit function (right hand side should be zero and emitted):\n";
    std::string s;
    std::getline(std::cin, s);


    Window window;
    
    if(glewInit() != GLEW_OK) 
        return -1;

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    {

        std::deque<Token> temp = ExprToTokens(s);
        std::queue<Token> test = ShuntingYard(temp);

        Function function = Function(test);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        VertexArray va;
        VertexBuffer vb(function.getVertices(), function.getVertexSize());
        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(function.getIndices(), function.getIndexCount());
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float increment = 0.05f;

        float currentFrame = 0.0f;
        float lastFrame = 0.0f;
        float deltaTime = 0.0f;
        bool hasUpdated = false;

        /* Loop until the user closes the window */
        while (!window.ShouldClose())
        {
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            std::cout << "FPS: " << 1 / deltaTime << '\n';

            /* Render here */
            renderer.Clear();

            shader.Bind();
            //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            float xoffset = 0;
            float yoffset = 0;
            float zoom = 0;

            if (window.GetKey(GLFW_KEY_W) == GLFW_PRESS)
                yoffset -= increment;
            if (window.GetKey(GLFW_KEY_A) == GLFW_PRESS)
                xoffset += increment;
            if (window.GetKey(GLFW_KEY_S) == GLFW_PRESS)
                yoffset += increment;
            if (window.GetKey(GLFW_KEY_D) == GLFW_PRESS)
                xoffset -= increment;
            if (window.GetKey(GLFW_KEY_Q) == GLFW_PRESS)
                zoom += increment;
            if (window.GetKey(GLFW_KEY_E) == GLFW_PRESS)
                zoom -= increment;

            if (xoffset || yoffset || zoom)
            {
                function.IncrementDimensions(xoffset - zoom, yoffset - zoom, xoffset + zoom, yoffset + zoom);
                function.RecomputeVertices();
                vb.UpdateBuffer(function.getVertices(), function.getVertexSize());
                ib.UpdateBuffer(function.getIndices(), function.getIndexCount());
            }

            renderer.DrawLines(va, ib, shader);
            /* Swap front and back buffers */
            window.SwapBuffers();

            /* Poll for and process events */
            window.PollEvents();

        }
    }

    return 0;
}