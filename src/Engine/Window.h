#pragma once
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* m_WindowID;

public:
	Window();
	~Window();

	void SwapBuffers() { glfwSwapBuffers(m_WindowID); };
	int ShouldClose() { return glfwWindowShouldClose(m_WindowID); };
	void PollEvents() { glfwPollEvents(); };
	int GetKey(int key) { return glfwGetKey(m_WindowID, key); }
};