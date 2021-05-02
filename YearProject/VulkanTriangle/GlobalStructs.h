#pragma once

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


struct UniformBufferObject {
	alignas(16)glm::mat4 model;
	alignas(16)glm::mat4 view;
	alignas(16)glm::mat4 proj;
};

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

const int WIDTH = 2400;      // Screen deetz
const int HEIGHT = 1800;

const int numOfAgents = 200;
const int gridSize = 500;	// Actually 500 X 500 cus the cubes are 2 units wide
const int goalID = 25851;
const int pathMax = 1000;
const int gridSizeTotal = (gridSize/2) * (gridSize / 2); // 250,000


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

