// Author: Robin Meyler
// Date: 2020/2021

#include <chrono>
#include <algorithm>	// Min Max
#include <iostream>     // Basic input output
#include <stdexcept>    // Errors
#include <functional>   // Lambda Functions -> Resource management
#include <vector>       // Container
#include <cstdlib>      // Macros -> EXIT_SUCCESS 
#include <cstring>      // Strings
#include <optional>     // Alows for seperating empty / 0 on a return, std::optionl is empty till assigned
#include <map>          // No explored yet
#include <cstdint> // Necessary for UINT32_MAX
#include <set>
#include <fstream>
#include <array>
#include "Render.h"

Render m_renderer;

const std::vector<const char*> validationLayers =
{         // Needed (coming back to Valid layers, fix then)
	"VK_LAYER_KHRONOS_validation"
};

 
// This removes extra checks and work when we are in release and we have removed all bugs(as if)
#ifdef NDEBUG                               // Part of STL, if Debug, else
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


class HelloTriangleApplication {            // Seperated into a better structure later
public:
	std::vector<Cube> m_gameCubes;
	void run() {
		initWindow();                       // Setup GLFW window and settings for Vulkan

		setupCubeMap();
		m_renderer.creatBufferObjects(5);
		m_renderer.addVBOs(&m_gameCubes);
		m_renderer.setupVulkan(window);
	
		mainLoop();                         // Loop
		cleanup();                          // Clearing memory off of the heap
	}
	bool framebufferResized = false;
private:

	GLFWwindow* window;                     // Main window

	void initWindow()
	{
		glfwInit();  // Start it
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // Tell it its not OpenGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);     // Resize isn't straigth forward, set false
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // 4th is which monitor, 5th is GL(no use)
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			m_renderer.draw();
		}
	}

	void cleanup()
	{
		m_renderer.cleanUp();
		glfwDestroyWindow(window);
		glfwTerminate();
		glfwTerminate();
	}

	void setupCubeMap()
	{
		Cube cubeOne(0);
		Cube cubeTwo(-4);
		Cube cubeThree(4);
		Cube cube4(-2);
		Cube cube5(2);
		m_gameCubes.push_back(cubeOne);
		m_gameCubes.push_back(cubeTwo);
		m_gameCubes.push_back(cubeThree);
		m_gameCubes.push_back(cube4);
		m_gameCubes.push_back(cube5);
	}
};

int main()
{
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

