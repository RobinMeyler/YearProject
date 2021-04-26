#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <chrono>
#include <random>

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
	glm::vec2 position;
	int padding;        // Aligning the memoryw with the base/ largest data -> Vec2
};


const int numOfAgents = 1000;
const int gridSize = 100;	// Actually 500 X 500 cus the cubes are 2 units wide
const int goalID = 1051;
const int pathMax = 500;


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



