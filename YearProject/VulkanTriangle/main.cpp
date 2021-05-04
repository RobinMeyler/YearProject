// Author: Robin Meyler
// Date: 2020/2021
// Title: GPU Accelerated A* pathfind with the Vulkan API
// Description: Final Year project for Computer Games Development in IT Carlow

// It is important to note here that Vulkan was release in 2016 and is only supported on newer, higher level graphics cards, this was tested on an Nvidia RTX 3070 GPU.
// It is likely this program will not run on an older GPU and or if they Vulkan driver is not present on the GPU
// If the program does not run, this may be the reasoning.

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
#include <cstdint>		// Necessary for UINT32_MAX
#include <set>
#include <fstream>
#include <array>
#include "GlobalStructs.h"
#include "Render.h"

Render m_renderer;
const float CAMERA_CHANGE = 1.0f;

class HelloTriangleApplication {            // Seperated into a better structure later
public:
	std::vector<Node*> m_gameNodes;
	std::vector<Cube*> m_gameCubes;
	Cube* m_goal;
	std::vector<glm::vec2> m_matchingPositions;
	int timer = 0;

	void run() {
		initWindow();                       // Setup GLFW window and settings for Vulkan

		IMGUI_CHECKVERSION();				// Setup Dear ImGui context
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForVulkan(window, true);

		setupCubeMap();						// Setup grid, agents and goal
		m_renderer.addVBOs(&m_gameCubes);	// add the setup info
		m_renderer.setupVulkan(window);		// Initalize vulkan

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

			// Moving Camera around
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && timer > 10 )
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(CAMERA_CHANGE, 0.0f, 0.0f), 0);
			}
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, CAMERA_CHANGE, 0.0f), 0);
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, -CAMERA_CHANGE, 0.0f), 0);
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && timer > 10 )
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(-CAMERA_CHANGE, 0.0f, 0.0f), 0);
			}

			// Tilt Camera
			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, CAMERA_CHANGE*2, 0.0f), 2);
			}
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, -CAMERA_CHANGE*2, 0.0f), 2);
			}
			timer++;

			m_renderer.draw();
		}
	}

	void cleanup()
	{
		m_renderer.cleanUp();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void setupCubeMap()
	{
		// Base cube used for the insatanced rendering
	    Cube* cube = new Cube(0, 0, 2.0f);
		m_gameCubes.push_back(cube);
		
		// Grid
		int count = 0;
		for (int i = 0; i < gridSize; i = i + 2)
		{
			for (int j = 0; j < gridSize; j = j + 2)
			{
				int rdm = rand() % 10;
				Node* node = new Node;
				node->costToGoal = 1;
				node->totalCostFromStart = 0;
				node->totalCostAccumlative = 100000;
				node->marked = 0;
				node->ID = (count);
				node->previousID = -1;
				node->passable = 1;
				if (j == 0 || j == (gridSize - 2) || i == 0 || i == (gridSize - 2) || rdm % 10 == 1)
					node->passable = 0;

				// Neighbours
				// Left
				if (j != 0)
					node->arcIDs[0] = ((count) - 1);
				else
					node->arcIDs[0] = -1;

				// Right
				if (j != (gridSize - 2))
					node->arcIDs[1] = ((count) + 1);
				else
					node->arcIDs[1] = -1;

				// Up
				if (i != 0)
					node->arcIDs[2] = ((count) - (gridSize/2));
				else
					node->arcIDs[2] = -1;

				// Down
				if (i != (gridSize-2))
					node->arcIDs[3] = ((count) + (gridSize/2));
				else
					node->arcIDs[3] = -1;
				
				m_matchingPositions.push_back(glm::vec2(j, i));
				m_gameNodes.push_back(node);
				count++;
			}
		}

		for (auto &nod : m_gameNodes)
		{
			nod->costToGoal = abs(m_matchingPositions.at(goalID).x - m_matchingPositions.at(nod->ID).x) + abs(m_matchingPositions.at(goalID).y - m_matchingPositions.at(nod->ID).y);
		}

		// Randomise starts of agents
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, gridSizeTotal-1);
		std::vector<int> starts;
		for (int i = 0; i < numOfAgents; i++)
		{
			long int random = dis(gen);
			while (m_gameNodes.at(random)->passable == 0)
			{
				random = dis(gen);
			}
			starts.push_back(random);
			Cube* cub = new Cube(m_matchingPositions.at(random).x, m_matchingPositions.at(random).y, 2.0f);
			cub->updateColor(glm::vec3(0.0f, 0.0f, 0.0f));
			m_gameCubes.push_back(cub);
		}
		m_renderer.setStarts(starts);

		m_goal = new Cube(m_matchingPositions.at(goalID).x, m_matchingPositions.at(goalID).y, 2.0f);
		m_goal->updateColor(glm::vec3(0.0f, 0.0f, 1.0f));
		m_gameCubes.push_back(m_goal);

		// Tell the renderer the infor it needs
		m_renderer.setMatchingPositions(&m_matchingPositions);
		m_renderer.addNodes(&m_gameNodes);
		m_renderer.creatBufferObjects(m_gameCubes.size());
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
		system("PAUSE");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

