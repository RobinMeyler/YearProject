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
#include "GlobalStructs.h"
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


const float CAMERA_CHANGE = 1.0f;

class HelloTriangleApplication {            // Seperated into a better structure later
public:
	std::vector<Cube*> m_gameCubesMoveable;
	std::vector<Cube*> m_gameCubes;
	std::vector<glm::vec2> m_matchingPositions;
	std::vector<Node*> m_gameNodes;
	std::vector<Cube*> m_AI;
	
	int timer = 0;

	Cube* m_start1;
	Cube* m_start2;
	Cube* m_start3;
	Cube* m_goal;
	void run() {
		initWindow();                       // Setup GLFW window and settings for Vulkan

		setupCubeMap();
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
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && timer > 1000)
			{
				timer = 0;
				m_renderer.doPathfinding = true;
			}
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && timer > 1000 && m_renderer.update == true)
			{
				timer = 0;
				m_renderer.resetAgents();
				m_renderer.update = false;
			}
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
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, 0.0f, -CAMERA_CHANGE*3), 0);
			}
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				m_renderer.updateCameraPosition(glm::vec3(0.0f, 0.0f, CAMERA_CHANGE*3), 0);
			}
			// Tild up
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
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				if(m_renderer.speed > 2)
					m_renderer.speed -= 2;
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && timer > 10)
			{
				timer = 0;
				if (m_renderer.speed < 500)
					m_renderer.speed += 2;
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
		//// Bottome layer
		//int w = 0;
		//int h = 0;
		//for (int i = 0; i < gridSize; i = i+2)
		//{
		//	for(int j = 0; j < gridSize; j= j+2)
		//	{
		//		Cube* cube = new Cube( j, i, 0.0f );
		//		cube->updateColor(glm::vec3(0.5f, 0.5f, 0.5f));
		//		m_gameCubes.push_back(cube);
		//		h++;
		//	}	
		//	w++;
		//	h = 0;
		//}
	
	    Cube* cube = new Cube(0, 0, 2.0f);
		m_gameCubes.push_back(cube);
		// Top layer
		int count = 0;
		int w2 = 0;
		int h2 = 0;
		for (int i = 0; i < gridSize; i = i + 2)
		{
			for (int j = 0; j < gridSize; j = j + 2)
			{
				int rdm = rand() % 10;
				Node* node = new Node;
				node->passable = 1;
				if (j == 0 || j == (gridSize-2) || i == 0 || i == (gridSize-2) || rdm % 10 == 3)
				{
					//Cube* cube = new Cube(j, i, 2.0f);
					////cube->updateColor(glm::vec3(0.0f, 1.0f, 0.0f));
					//m_gameCubes.push_back(cube);
					node->passable = 0;
				}
				count++;
				node->costToGoal = 1;
				node->totalCostFromStart = 0;
				node->totalCostAccumlative = 1000;
				node->marked = 0;

				node->ID = (h2 + w2);
				node->previousID = -1;
				node->position.x = j;
				node->position.y = i;
				m_matchingPositions.push_back(glm::vec2(j, i));
				// Left
				if (j != 0)
					node->arcIDs[0] = ((h2 + w2) - 1);
				else
					node->arcIDs[0] = -1;

				// Right
				if (j != (gridSize - 2))
					node->arcIDs[1] = ((h2 + w2) + 1);
				else
					node->arcIDs[1] = -1;

				// Up
				if (i != 0)
					node->arcIDs[2] = ((h2 + w2) - (gridSize/2));
				else
					node->arcIDs[2] = -1;

				// Down
				if (i != (gridSize-2))
					node->arcIDs[3] = ((h2 + w2) + (gridSize/2));
				else
					node->arcIDs[3] = -1;
				
				m_gameNodes.push_back(node);
				h2++;
			}
			
		}
		for (int i = 0; i < m_gameCubes.size(); i++)
		{
			m_gameNodes.at(i)->costToGoal = abs(m_matchingPositions.at(goalID).x - m_matchingPositions.at(i).x) + abs(m_matchingPositions.at(goalID).y - m_matchingPositions.at(i).y);
		}
		// 18 is test goal
		for (auto &nod : m_gameNodes)
		{
			nod->costToGoal = abs(m_matchingPositions.at(goalID).x - nod->position.x) + abs(m_matchingPositions.at(goalID).y - nod->position.y);
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, gridSizeTotal-1);

		std::vector<int> starts;
		for (int i = 0; i < numOfAgents; i++)
		{
			long int oop = dis(gen);
			while (m_gameNodes.at(oop)->passable == 0)
			{
				oop = dis(gen);
			}
			starts.push_back(oop);
			Cube* cub = new Cube(m_matchingPositions.at(oop).x, m_matchingPositions.at(oop).y, 2.0f);
			cub->updateColor(glm::vec3(0.0f, 0.0f, 0.0f));
			m_gameCubes.push_back(cub);
		}
		m_renderer.setStarts(starts);

		m_goal = new Cube(m_gameNodes.at(goalID)->position.x, m_gameNodes.at(goalID)->position.y, 2.0f);
		m_goal->updateColor(glm::vec3(1.0f, 0.0f, 0.0f));
		m_gameCubes.push_back(m_goal);

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
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

