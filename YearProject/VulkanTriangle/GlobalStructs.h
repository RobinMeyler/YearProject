#pragma once

// Author: Robin Meyler
// Date: 2020 - 2021

#define GLFW_INCLUDE_VULKAN		// This includes Vulkan
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

const int WIDTH = 1200;      // Screen deetz
const int HEIGHT = 900;

const int numOfAgents = 200;
const int goalID = 5851;
const int pathMax = 1000;

// To change this grid size, you need to get the number of nodes E.g 100x100 = 10,000 in this build
// Then open the AStar.comp file in the Shaders folder in the director, change the number in the nodes array to that value E.g 10,000
// You then have to double click the compile batch file in the Shader folder to recompile the GLSL shader into SPIR-V
// Without these steps, the vectors will not match and it may crash
const int gridSize = 200;	// Actually 100 X 100 cus the cubes are 2 units wide
const int gridSizeTotal = (gridSize / 2) * (gridSize / 2);	// 10,000

struct Node
{
	float costToGoal;
	float totalCostFromStart;
	float totalCostAccumlative;
	int marked;		// Has been reached yet or not
	int passable;
	int ID;
	int previousID;		// Previous id for finidng path
	int arcIDs[4];		// 4 neighbours in grid by ID
};

struct NodeData
{
	int start;		// Start ID
	int goal;		// Goal ID
	Node nodes[gridSizeTotal];
};

struct Path
{
	int pathList[pathMax];
};

// Contains the instanced data
struct InstanceBuffer {
	VkBuffer buffer;
	VkDeviceMemory memory;
	size_t size = 0;
	VkDescriptorBufferInfo descriptor;
};

// Per-instance data block
struct InstanceData {
	glm::vec3 pos;
};

struct UniformBufferObject {
	alignas(16)glm::mat4 model;
	alignas(16)glm::mat4 view;
	alignas(16)glm::mat4 proj;
};

