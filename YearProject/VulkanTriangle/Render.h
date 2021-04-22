#pragma once

// Author: Robin Meyler
// Date: 2020/2021

#define GLFW_INCLUDE_VULKAN		// This includes Vulkan
#include <GLFW/glfw3.h>


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
#include "Cube.h"



// There are many queue families
	// From which speific queues are derived to manage code
	// This struct is used to check if they are available to the GPU
	// Also to hold information about the Q's
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;          // Optional is empty till assigned (So it won't mix up index 0 from nothing)
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();           // Bool
	}
};
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;		// What the GPU allows a swaps chain to do
	std::vector<VkSurfaceFormatKHR> formats;	// Pixel info
	std::vector<VkPresentModeKHR> presentModes;	// Buffer/frame settings
};
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

static std::vector<char> readFile(const std::string& filename);
static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

const int MAX_FRAMES_IN_FLIGHT = 2;
const int WIDTH = 1600;      // Screen deetz
const int HEIGHT = 1200;



class Render {
public:
	Render();
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
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();		// Different draw command groups set
	void createSyncObjects();		// For controlling the ordering of commands and draws ( Controls concurent actions )

	void setupVulkan(GLFWwindow* t_window);
	void draw();
	void cleanUp();

	void createComputePipelineLayout();
	void createPathsBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position);
	void createNodeBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position);
	void createComputePipeline(const std::string& shaderName);
	void allocateBufferMemoryAndBind(VkBuffer& buffers, VkDeviceMemory& bufferMemory, int t_offset);
	void allocateDescriptorSets(const std::vector<VkBuffer> &buffers);
	void createComputeCommandPoolAndBuffer();
	void addNodes(std::vector<Node*> *t_nodes);
	void setStarts(std::vector<int> t_starts);

	void addVBOs(std::vector<Cube*> *t_cubes);
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
	void updateUniformBuffer(uint32_t currentImage);
	void recreateSwapchain();
	void cleanupSwapChain();
	void updateBufferMemory(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory);

	std::vector<Cube*>* cubes;
	std::vector<Node*>* nodes;
	bool framebufferResized = false;
private:

	GLFWwindow* window;                     // Main window
	VkInstance instance;                    // Instance of Vulkan, needed for everything.
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;				// Controls the images to be set for viewing
	std::vector<VkImage> swapChainImages;	// The images
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;			// Format settings
	VkExtent2D swapChainExtent;				// Extent settings
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;       // Hold information about the GPU device, set to null
	VkDevice device;						// Logical device
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceMemory> vertexBufferMemorys;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkDeviceMemory> indexBufferMemorys;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;


	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkDescriptorSetLayout setLayout;
	VkPipelineLayout pipelineLayoutCompute;
	VkPipeline pipelineCompute;
	VkDescriptorPool descriptorPoolCompute;
	VkDescriptorSet descriptorSetCompute;
	VkCommandBuffer computeCommandBuffer;
	VkDeviceMemory memoryNode;
	VkDeviceMemory memoryPaths;
	VkBuffer buffersNodes;
	VkBuffer buffersPaths;

	std::vector<VkBuffer> m_buffers;
	Path* returnPaths;
	Path* returnPaths2;
	Path* returnPaths3;
	std::vector<Path*> backPaths;
	std::vector<NodeData*> backData;
	std::vector<std::vector<int>> backfinalPaths;
	std::vector<int> starts;
	std::vector<int> nexts;

	NodeData* dataR;
	NodeData* dataR2;
	NodeData* dataR3;
	int next = 1;
	int last = 0;
	int wait = 0;
	std::vector<int> finalPath;
	std::vector<int> finalPath2;
	std::vector<int> finalPath3;
};	


struct Vertex2 {
	glm::vec3 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex2);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex2, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex2, color);

		return attributeDescriptions;
	}
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
