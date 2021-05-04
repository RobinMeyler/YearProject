#pragma once

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
#include <cstdint>		// Necessary for UINT32_MAX
#include <set>
#include <fstream>
#include <array>

#include "GlobalStructs.h"
#include "Cube.h"


// There are many queue families
	// From which speific queues are derived to manage code
	// This struct is used to check if they are available to the GPU
	// Also to hold information about the Q's
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;          // Optional is empty till assigned (So it won't mix up index 0 from nothing)
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();          
	}
};
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;		// What the GPU allows a swaps chain to do
	std::vector<VkSurfaceFormatKHR> formats;	// Pixel info
	std::vector<VkPresentModeKHR> presentModes;	// Buffer/frame settings
};

// Helper functions
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
static std::vector<char> readFile(const std::string& filename);
static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

const int MAX_FRAMES_IN_FLIGHT = 2;

class Render {
public:
	// static vairables used for IMGUI
	static float speed;
	static float zoom;
	static float timeTaken;
	static bool pathFind;
	static bool canReset;
	static bool up;
	static bool left;
	static bool right;
	static bool down;

	// Contructor
	Render();

	// Initalization methods
	void createVulkanInstance();
	void creatVulkanSurface(GLFWwindow* t_window);
	void chooseGPUDevice();
	void createVulkanLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createVulkanRenderPass();
	void createDescriptorSetLayout();
	void createVulkanGraphicsPipeline();
	void createFrameBuffers();
	void createCommandPool();
	void creatBufferObjects(int t_count);
	void createVertexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory);
	void createIndexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory);
	void createUniformBuffers();
	void createInstanceBuffer();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();		
	void createSyncObjects();	
	void createComputePipelineLayout();
	void createPathsBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position);
	void createNodeBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position);
	void createComputePipeline(const std::string& shaderName);
	void allocateBufferMemoryAndBind(VkBuffer& buffers, VkDeviceMemory& bufferMemory, int t_offset);
	void allocateDescriptorSets(const std::vector<VkBuffer>& buffers);
	void createComputeCommandPoolAndBuffer();

	// Functions called outside of the class
	void setupVulkan(GLFWwindow* t_window);
	void draw();
	void cleanUp();
	void addNodes(std::vector<Node*> *t_nodes);
	void setStarts(std::vector<int> t_starts);
	void setMatchingPositions(std::vector<glm::vec2>* t_pos);
	void addVBOs(std::vector<Cube*> *t_cubes);

	// Updating program
	void updateCameraPosition(glm::vec3 t_changeInCameraPosition, int t_specify);
	void updateUniformBuffer(uint32_t currentImage);
	void updateBufferMemory(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory);
	void resetAgents();

	// Helper functions called within setup classes to achieve something
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkQueue* getGraphicsQueue();
	VkQueue* getPresentQueue();
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createCommandPool(VkCommandPool* commandPool, VkCommandPoolCreateFlags flags);
	void createCommandBuffers(VkCommandBuffer* commandBuffer, uint32_t commandBufferCount, VkCommandPool& commandPool);

	// Cleanup
	void recreateSwapchain();
	void cleanupSwapChain();

	// Public variables
	bool doPathfinding = false;
	bool update = false;
	bool framebufferResized = false;
	int overallImageCount = 1;
	int minimumImageCount = 1;

private:

	// Main vulkan required vairables
	// Window
	GLFWwindow* window;     
	VkSurfaceKHR surface;

	// Base info
	VkInstance instance;                  
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;      
	VkDevice device;						
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	// Frame information
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// Pipeline
	VkPipeline graphicsPipelineNONinstanced;
	VkPipeline graphicsPipeline;
	VkPipeline pipelineCompute;
	VkPipelineLayout pipelineLayout;
	VkPipelineLayout pipelineLayoutCompute;

	// Render pass
	VkRenderPass renderPass;

	// Descriptor Set for uniforms
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkDescriptorSetLayout setLayout;
	VkDescriptorPool descriptorPoolCompute;
	VkDescriptorSet descriptorSetCompute;

	// Command buffer
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandBuffer computeCommandBuffer;

	// Synchronization
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;
	uint32_t currentIndex = 0;

	// Device Memory
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<VkDeviceMemory> indexBufferMemorys;
	std::vector<VkDeviceMemory> vertexBufferMemorys;
	VkDeviceMemory memoryNode;
	VkDeviceMemory memoryPaths;
	VkDeviceMemory stagingBufferMemory;

	// Buffers
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkBuffer> m_buffers;
	VkBuffer stagingBuffer;
	VkBuffer buffersNodes;
	VkBuffer buffersPaths;
	InstanceBuffer instanceBuffer;

	// IMGUI UI
	VkDescriptorPool guiPool;
	VkCommandPool guiPools;
	VkRenderPass imGuiRenderPass;
	std::vector<VkCommandBuffer> guiCMDbuffers;
	std::vector<VkFramebuffer> guiFrameBuffers;

	// Structs
	std::vector<Cube*>* cubes;
	std::vector<Cube*> cubesNew;
	std::vector<Node*>* nodes;
	std::vector<Path*> backPaths;
	std::vector<NodeData*> backData;

	// Important progam information
	std::vector<std::vector<int>> backfinalPaths;
	std::vector<int> starts;
	std::vector<int> nexts;
	std::vector<glm::vec2>* matchingPos;
	glm::vec3 eye = glm::vec3((float)(gridSize / 2), (float)(gridSize / 2), 400.0f);
	glm::vec3 lookAT = glm::vec3((float)(gridSize / 2), (float)(gridSize / 2), 0.0f);

	int next = 1;
	int last = 0;
	int wait = 0;
	bool readyForReset = false;
};	

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Render*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}
