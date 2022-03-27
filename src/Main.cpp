#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include "Calculator/MarchingSquares.h"

#include "Engine/Renderer.h"
#include "Engine/VertexBuffer.h"
#include "Engine/IndexBuffer.h"
#include "Engine/VertexArray.h"
#include "Engine/VertexBufferLayout.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Window.h"

float circle(float x, float y)
{
    return y*y + x*x - 1;
}

float f(float x, float y)
{
    return y - sqrt(x);
}

int main(void)
{
    Window window;
    
    if(glewInit() != GLEW_OK) 
        return -1;

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    {
        Function function = Function(&circle, -2, -2, 2, 2);

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

        float increment = 0.005f;
        float xoffset = 0;

        /* Loop until the user closes the window */
        while (!window.ShouldClose())
        {
            /* Render here */
            renderer.Clear(0, 0, 0, 1);

            shader.Bind();
            //shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            vb.UpdateBuffer(function.getVertices(), function.getVertexSize());
            ib.UpdateBuffer(function.getIndices(), function.getIndexCount());

            renderer.DrawLines(va, ib, shader);

            if (xoffset > 1.0f) increment = -0.005f;
            else if (xoffset < 0.0f) increment = 0.005f;
            xoffset += increment;

            int state = window.GetKey(GLFW_KEY_SPACE);
            if (state == GLFW_PRESS)
            {
                function.SetDimensions(-2 + xoffset, -2 + xoffset, 2 - xoffset, 2 - xoffset);
                function.RecomputeVertices();
            }

            /* Swap front and back buffers */
            window.SwapBuffers();

            /* Poll for and process events */
            window.PollEvents();

        }
    }

    return 0;
}