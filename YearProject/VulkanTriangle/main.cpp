//#define GLFW_INCLUDE_VULKAN		// This includes Vulkan
//#include <GLFW/glfw3.h>
//
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/glm.hpp>
//
//#include <chrono>
//#include <algorithm>	// Min Max
//#include <iostream>     // Basic input output
//#include <stdexcept>    // Errors
//#include <functional>   // Lambda Functions -> Resource management
//#include <vector>       // Container
//#include <cstdlib>      // Macros -> EXIT_SUCCESS 
//#include <cstring>      // Strings
//#include <optional>     // Alows for seperating empty / 0 on a return, std::optionl is empty till assigned
//#include <map>          // No explored yet
//#include <cstdint> // Necessary for UINT32_MAX
//#include <set>
//#include <fstream>
//#include <array>
//
//#include "Cube.h"
//
//
//static std::vector<char> readFile(const std::string& filename);
//static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
//
//const int MAX_FRAMES_IN_FLIGHT = 2;
//const int WIDTH = 1600;      // Screen deetz
//const int HEIGHT = 1200;
//
//const std::vector<const char*> validationLayers =
//{         // Needed (coming back to Valid layers, fix then)
//	"VK_LAYER_KHRONOS_validation"
//};
//
//const std::vector<const char*> deviceExtensions = {
//	VK_KHR_SWAPCHAIN_EXTENSION_NAME
//};
//
//
//// This removes extra checks and work when we are in release and we have removed all bugs(as if)
//#ifdef NDEBUG                               // Part of STL, if Debug, else
//const bool enableValidationLayers = false;
//#else
//const bool enableValidationLayers = true;
//#endif
//
//// There are many queue families
//	// From which speific queues are derived to manage code
//	// This struct is used to check if they are available to the GPU
//	// Also to hold information about the Q's
//struct QueueFamilyIndices {
//	std::optional<uint32_t> graphicsFamily;          // Optional is empty till assigned (So it won't mix up index 0 from nothing)
//	std::optional<uint32_t> presentFamily;
//
//	bool isComplete() {
//		return graphicsFamily.has_value() && presentFamily.has_value();           // Bool
//	}
//};
//
//struct SwapChainSupportDetails {
//	VkSurfaceCapabilitiesKHR capabilities;		// What the GPU allows a swaps chain to do
//	std::vector<VkSurfaceFormatKHR> formats;	// Pixel info
//	std::vector<VkPresentModeKHR> presentModes;	// Buffer/frame settings
//};
//
//struct Vertex {
//	glm::vec3 pos;
//	glm::vec3 color;
//
//	static VkVertexInputBindingDescription getBindingDescription()
//	{
//		VkVertexInputBindingDescription bindingDescription{};
//		bindingDescription.binding = 0;
//		bindingDescription.stride = sizeof(Vertex);
//		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//		return bindingDescription;
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
//	{
//		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
//		attributeDescriptions[0].binding = 0;
//		attributeDescriptions[0].location = 0;
//		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attributeDescriptions[0].offset = offsetof(Vertex, pos);
//
//		attributeDescriptions[1].binding = 0;
//		attributeDescriptions[1].location = 1;
//		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attributeDescriptions[1].offset = offsetof(Vertex, color);
//
//		return attributeDescriptions;
//	}
//};
//
//
//std::vector<Cube> cubes = { 0, 4, -4 };
//
//struct UniformBufferObject {
//	alignas(16)glm::mat4 model;
//	alignas(16)glm::mat4 view;
//	alignas(16)glm::mat4 proj;
//};
////
////Quad m_square(-0.1f, -0.1f);
////Quad m_square2(0.2f, -0.2f);
//
//class HelloTriangleApplication {            // Seperated into a better structure later
//public:
//	void run() {
//		initWindow();                       // Setup GLFW window and settings for Vulkan
//		initVulkan();                       // Setting up the information and checks needed for the drivers
//		mainLoop();                         // Loop
//		cleanup();                          // Clearing memory off of the heap
//	}
//	bool framebufferResized = false;
//private:
//
//	GLFWwindow* window;                     // Main window
//
//	VkInstance instance;                    // Instance of Vulkan, needed for everything.
//
//	VkSurfaceKHR surface;
//
//	VkSwapchainKHR swapChain;				// Controls the images to be set for viewing
//
//	std::vector<VkImage> swapChainImages;	// The images
//	std::vector<VkImageView> swapChainImageViews;
//
//	VkFormat swapChainImageFormat;			// Format settings
//	VkExtent2D swapChainExtent;				// Extent settings
//
//	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;       // Hold information about the GPU device, set to null
//
//	VkDevice device;						// Logical device
//
//	VkQueue graphicsQueue;
//	VkQueue presentQueue;
//
//	VkRenderPass renderPass;
//	VkPipelineLayout pipelineLayout;
//	VkPipeline graphicsPipeline;
//
//	std::vector<VkFramebuffer> swapChainFramebuffers;
//
//	VkCommandPool commandPool;
//	std::vector<VkCommandBuffer> commandBuffers;
//
//	std::vector<VkSemaphore> imageAvailableSemaphores;
//	std::vector<VkSemaphore> renderFinishedSemaphores;
//	std::vector<VkFence> imagesInFlight;
//	std::vector<VkFence> inFlightFences;
//	size_t currentFrame = 0;
//
//	std::vector<VkBuffer> vertexBuffers;
//	std::vector<VkDeviceMemory> vertexBufferMemorys;
//	std::vector<VkBuffer> indexBuffers;
//	std::vector<VkDeviceMemory> indexBufferMemorys;
//
//	VkBuffer vertexBuffer;
//	VkDeviceMemory vertexBufferMemory;
//	VkBuffer indexBuffer;
//	VkDeviceMemory indexBufferMemory;
//
//	VkBuffer vertexBuffer2;
//	VkDeviceMemory vertexBufferMemory2;
//	VkBuffer indexBuffer2;
//	VkDeviceMemory indexBufferMemory2;
//
//	VkDescriptorSetLayout descriptorSetLayout;
//
//	std::vector<VkBuffer> uniformBuffers;
//	std::vector<VkDeviceMemory> uniformBuffersMemory;
//
//	VkDescriptorPool descriptorPool;
//	std::vector<VkDescriptorSet> descriptorSets;
//
//
//	void initVulkan() {
//		createInstance();			 // Needed first
//		// setup validation layers here
//		createSurface();		     // creates a surface to interact with window
//		pickPhysicalDevice();		 // Pick a GPU, you can use many (I am not) this picks which one based on checks  
//		createLogicalDevice();		// Logic information for the instance
//		createSwapChain();			// Create a Swapchain to manage images for buffering
//		createImageViews();			// To Draw on
//		createRenderPass();			// Rendering settings for a pass on the GPU
//		createDescriptorSetLayout();
//		createGraphicsPipeline();	// Full pipeline for the rendering
//		createFramebuffers();		// Frame to draw to to be readied for the Swapchain
//		createCommandPool();		// set of Draw commands
//
//		creatBufferObjects();
//		createVertexBuffer(cubes.at(0), vertexBuffers.at(0), vertexBufferMemorys.at(0));
//		createVertexBuffer(cubes.at(1), vertexBuffers.at(1), vertexBufferMemorys.at(1));
//		createVertexBuffer(cubes.at(2), vertexBuffers.at(2), vertexBufferMemorys.at(2));
//
//		createIndexBuffer(cubes.at(0), indexBuffers.at(0), indexBufferMemorys.at(0));
//		createIndexBuffer(cubes.at(1), indexBuffers.at(1), indexBufferMemorys.at(1));
//		createIndexBuffer(cubes.at(2), indexBuffers.at(2), indexBufferMemorys.at(2));
//
//		createUniformBuffers();
//		createDescriptorPool();
//		createDescriptorSets();
//		createCommandBuffers();		// Different draw command groups set
//		createSyncObjects();		// For controlling the ordering of commands and draws ( Controls concurent actions )
//	}
//
//	void creatBufferObjects()
//	{
//		for (int i = 0; i < cubes.size(); i++)
//		{
//			VkBuffer buffer, index;
//			vertexBuffers.push_back(buffer);
//			indexBuffers.push_back(index);
//
//			VkDeviceMemory bufferMem, indexMem;
//			vertexBufferMemorys.push_back(bufferMem);
//			indexBufferMemorys.push_back(indexMem);
//		}
//	}
//
//	void createDescriptorPool()
//	{
//		VkDescriptorPoolSize poolSize{};
//		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());
//
//		VkDescriptorPoolCreateInfo poolInfo{};
//		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//		poolInfo.poolSizeCount = 1;
//		poolInfo.pPoolSizes = &poolSize;
//		poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
//
//		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create descriptor pool!");
//		}
//	}
//
//	void createDescriptorSets() 
//	{
//		std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
//		VkDescriptorSetAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		allocInfo.descriptorPool = descriptorPool;
//		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
//		allocInfo.pSetLayouts = layouts.data();
//
//		descriptorSets.resize(swapChainImages.size());
//		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate descriptor sets!");
//		}
//
//		for (size_t i = 0; i < swapChainImages.size(); i++) {
//			VkDescriptorBufferInfo bufferInfo{};
//			bufferInfo.buffer = uniformBuffers[i];
//			bufferInfo.offset = 0;
//			bufferInfo.range = sizeof(UniformBufferObject);
//
//			VkWriteDescriptorSet descriptorWrite{};
//			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			descriptorWrite.dstSet = descriptorSets[i];
//			descriptorWrite.dstBinding = 0;
//			descriptorWrite.dstArrayElement = 0;
//			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			descriptorWrite.descriptorCount = 1;
//			descriptorWrite.pBufferInfo = &bufferInfo;
//
//			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
//		}
//	}
//
//	void createDescriptorSetLayout() {
//		VkDescriptorSetLayoutBinding uboLayoutBinding{};
//		uboLayoutBinding.binding = 0;
//		uboLayoutBinding.descriptorCount = 1;
//		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		uboLayoutBinding.pImmutableSamplers = nullptr;
//		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//		VkDescriptorSetLayoutCreateInfo layoutInfo{};
//		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//		layoutInfo.bindingCount = 1;
//		layoutInfo.pBindings = &uboLayoutBinding;
//
//		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create descriptor set layout!");
//		}
//	}
//
//	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
//		VkBufferCreateInfo bufferInfo{};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = size;
//		bufferInfo.usage = usage;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create buffer!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(device, buffer, bufferMemory, 0);
//	}
//
//	void createVertexBuffer(Cube &t_cube, VkBuffer &t_vertexbuffer, VkDeviceMemory &t_vertexbuffermemory)
//	{
//		VkDeviceSize bufferSize = sizeof(t_cube.vertices[0]) * t_cube.vertices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//			memcpy(data, t_cube.vertices.data(), (size_t) bufferSize);
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, t_vertexbuffer, t_vertexbuffermemory);
//		copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createIndexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
//	{
//		VkDeviceSize bufferSize = sizeof(t_cube.indices[0]) * t_cube.indices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, t_cube.indices.data(), (size_t)bufferSize);
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, t_vertexbuffer, t_vertexbuffermemory);
//
//		copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createUniformBuffers() 
//	{
//		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//
//		uniformBuffers.resize(swapChainImages.size());
//		uniformBuffersMemory.resize(swapChainImages.size());
//
//		for (size_t i = 0; i < swapChainImages.size(); i++) 
//		{
//			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
//		}
//	}
//
//	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
//	{
//		VkCommandBufferAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandPool = commandPool;
//		allocInfo.commandBufferCount = 1;
//
//		VkCommandBuffer commandBuffer;
//		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//		VkCommandBufferBeginInfo beginInfo{};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//		VkBufferCopy copyRegion{};
//		copyRegion.srcOffset = 0; // Optional
//		copyRegion.dstOffset = 0; // Optional
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//		vkEndCommandBuffer(commandBuffer);
//
//		VkSubmitInfo submitInfo{};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(graphicsQueue);
//		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
//	}
//
//	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
//	{
//		VkPhysicalDeviceMemoryProperties memProperties;
//		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
//		{
//			if ((typeFilter & (1 << i)) &&
//				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
//			{
//				return i;
//			}
//		}
//		throw std::runtime_error("failed to find suitable memory type!");
//	}
//
//	void createInstance()
//	{
//		if (enableValidationLayers && !checkValidationLayerSupport())
//		{                         // If it's support and in Debug
//			throw std::runtime_error("validation layers requested, but not available!");
//		}
//
//		// To allow Vulkan to be generic a lot of the functionalisty is abstracted away into extentions
//		// Different GPUs can support some of these
//		uint32_t extensionCount = 0;                                                          // Create some memory to count them                                   
//		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);            // Layer, memory for count, memory for assignment (I use default layer for now)
//		std::vector<VkExtensionProperties> extensions(extensionCount);                        // Make a vector with the exact size of the count
//		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());  // Call again with container
//
//		// Outputting then to console
//		std::cout << "available extensions:" << std::endl;
//		for (const auto& extension : extensions) {
//			std::cout << "\t" << extension.extensionName << std::endl; // \t = tab
//		}
//
//		// These next 2 go hand in hand, app info first
//		VkApplicationInfo appInfo = {}; // Defalt init, leaves .pnext as nullptr, pnext tells the memory where to go next after use, nullptr deletes it
//		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;  // Most structs have this, like an ID
//		appInfo.pApplicationName = "Hello Triangle";
//		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//		appInfo.pEngineName = "No Engine";
//		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//		appInfo.apiVersion = VK_API_VERSION_1_0;
//
//		VkInstanceCreateInfo createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//		createInfo.pApplicationInfo = &appInfo; // Apllies info from above
//
//		// Come back to the layers
//		if (enableValidationLayers)
//		{
//			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//			createInfo.ppEnabledLayerNames = validationLayers.data();
//		}
//		else
//		{
//			createInfo.enabledLayerCount = 0;
//		}
//
//		uint32_t glfwExtensionCount = 0;    // How many, passed as memeory and counted
//		const char** glfwExtensions;        // Name list is returned
//
//		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);    // Returns extension list and creates count
//
//		createInfo.enabledExtensionCount = glfwExtensionCount;      // This info is added to the info struct
//		createInfo.ppEnabledExtensionNames = glfwExtensions;
//
//		// Global Validation Layer
//		// Global means on whole program not a specific device
//		createInfo.enabledLayerCount = 0;  // Empty for now
//
//		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) // Creates instance
//		{
//			throw std::runtime_error("failed to create instance!");
//		}
//	}
//
//	void createLogicalDevice()
//	{
//		// Queue families are just logical groupings of queues e.g graphics Qs, presentation Qs
//		//  Here we check our GPU for what it supports and sotres the info
//		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);	// Check for available queue families are store them
//
//		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;	// Creating more than 1 queue
//		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//		float queuePriority = 1.0f;		// Priority of the Queue used for ordering, 0.0 -1.0 scale
//		for (uint32_t queueFamily : uniqueQueueFamilies) 
//		{
//			VkDeviceQueueCreateInfo queueCreateInfo = {};
//			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//			queueCreateInfo.queueFamilyIndex = queueFamily;
//			queueCreateInfo.queueCount = 1;					// Can have more than 1 but 1 is simpiler for now
//			queueCreateInfo.pQueuePriorities = &queuePriority;
//			queueCreateInfos.push_back(queueCreateInfo);
//		}
//		VkPhysicalDeviceFeatures deviceFeatures = {};		// Features on device, none atm
//
//		VkDeviceCreateInfo createInfo = {};					// Logical device
//		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//		createInfo.pQueueCreateInfos = queueCreateInfos.data();		// Our created queues
//		createInfo.pEnabledFeatures = &deviceFeatures;				// Device features (none)
//		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // How many extentsions
//		createInfo.ppEnabledExtensionNames = deviceExtensions.data();	// The extensions
//
//		// Outdated
//		if (enableValidationLayers)
//		{
//			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//			createInfo.ppEnabledLayerNames = validationLayers.data();
//		}
//		else
//		{
//			createInfo.enabledLayerCount = 0;
//		}
//
//		//Creat the Logical device
//		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create logical device!");
//		}
//
//		// Store the Queues for use elsewhere
//		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//	}
//
//	void createSurface()
//	{
//		// Instance specific, window to map to and memory to hold it.
//		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create window surface!");
//		}
//	}
//
//	void createSwapChain() 
//	{
//		// Ask GPU what swapchain info it can do
//		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
//
//		//Chose the ones we want from what is available
//		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
//
//		//If we use the minimum, then we may have to wait for the driver to make another image, so 1 extra
//		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//		// Max count of 0 is a special value meaing there is no max
//		// Overwrite minimum
//		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
//		{
//			imageCount = swapChainSupport.capabilities.maxImageCount;
//		}
//
//		VkSwapchainCreateInfoKHR createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		createInfo.surface = surface;	// Link to window
//
//		createInfo.minImageCount = imageCount;		// Minimum images it should have
//		createInfo.imageFormat = surfaceFormat.format;		// Pixel detail info
//		createInfo.imageColorSpace = surfaceFormat.colorSpace;	// Type (non linear)
//		createInfo.imageExtent = extent;			// Window Size and res info
//		createInfo.imageArrayLayers = 1;			// Layers each image has, 1 unless Stereoscopic games
//		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	// If we wanted post processing, we would chnage this to transfer
//
//		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//		// Determine if the Graphics and Presentation families are the same or not
//		if (indices.graphicsFamily != indices.presentFamily) 
//		{		// If not
//			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//			createInfo.queueFamilyIndexCount = 2;		// how many
//			createInfo.pQueueFamilyIndices = queueFamilyIndices;	// The data set
//		}
//		else 
//		{
//			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//			createInfo.queueFamilyIndexCount = 0; // Optional
//			createInfo.pQueueFamilyIndices = nullptr; // Optional
//		}
//
//		// Such as rotating the image 90 degrees, here we do nothing
//		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
//
//		// aplha channel for blending
//		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//
//		createInfo.presentMode = presentMode;	 // Mailbox
//		createInfo.clipped = VK_TRUE;			// Use clipping
//
//		createInfo.oldSwapchain = VK_NULL_HANDLE;	// IF the chain becomes invalid ( we do nothing rn)
//
//		// Store these for use and checkls later
//		swapChainImageFormat = surfaceFormat.format;
//		swapChainExtent = extent;
//
//		// Create the bich
//		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create swap chain!");
//		}
//
//		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
//		swapChainImages.resize(imageCount);
//		// Get some images and store them for operations
//		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
//	}
//
//	void cleanupSwapChain() {
//		for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
//			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
//		}
//
//		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
//
//		vkDestroyPipeline(device, graphicsPipeline, nullptr);
//		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
//		vkDestroyRenderPass(device, renderPass, nullptr);
//
//		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
//			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
//		}
//
//		vkDestroySwapchainKHR(device, swapChain, nullptr);
//
//		for (size_t i = 0; i < swapChainImages.size(); i++) {
//			vkDestroyBuffer(device, uniformBuffers[i], nullptr);
//			vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
//		}
//		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
//	}
//
//	void recreateSwapChain() {
//		int width = 0, height = 0;
//		glfwGetFramebufferSize(window, &width, &height);
//
//		while (width == 0 || height == 0)
//		{
//			glfwGetFramebufferSize(window, &width, &height);
//			glfwWaitEvents();
//		}
//
//		vkDeviceWaitIdle(device);
//		cleanupSwapChain();
//
//		createSwapChain();
//		createImageViews();
//		createRenderPass();
//		createGraphicsPipeline();
//		createFramebuffers();
//		createUniformBuffers();
//		createDescriptorPool();
//		createDescriptorSets();
//		createCommandBuffers();
//	}
//
//	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
//	{
//		SwapChainSupportDetails details; // Details about the Swapchain that ask the GPU what it can do and stores it
//
//		// Min Max of images, num of images
//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
//
//		// Pixel formats, color space - Colour depth
//		uint32_t formatCount;
//		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
//
//		if (formatCount != 0)
//		{
//			details.formats.resize(formatCount);
//			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
//		}
//
//		// Presentation modes, Vsync, buffering etc
//		uint32_t presentModeCount;
//		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
//
//		if (presentModeCount != 0)
//		{
//			details.presentModes.resize(presentModeCount);
//			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
//		}
//
//
//		return details;
//	}
//
//	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
//	{
//		for (const auto& availableFormat : availableFormats)
//		{						// 32 bit color with SRGB (look this up again)
//			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
//			{
//				return availableFormat;
//			}
//		}
//		return availableFormats[0];	// Take the first if the one we want isn't there (it should be)
//	}
//
//	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
//	{
//		for (const auto& availablePresentMode : availablePresentModes)
//		{			// Mailbox allows triple buffering and doenst block images in queue, replaces them
//			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
//			{
//				return availablePresentMode;
//			}
//		}
//
//		return VK_PRESENT_MODE_FIFO_KHR;		// This will always be there
//	}
//
//	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
//	{
//		if (capabilities.currentExtent.width != UINT32_MAX) // Special case, if exists, you can change the window from default res
//		{
//			return capabilities.currentExtent;	// If not, take the default
//		}
//		else 
//		{
//			VkExtent2D actualExtent = { WIDTH, HEIGHT };
//			 // Make sure it's not more than the max capable, if possible set it to our defined window info
//			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
//			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
//
//			return actualExtent;
//		}
//	}
//
//	void createImageViews()
//	{
//		swapChainImageViews.resize(swapChainImages.size());
//
//		for (size_t i = 0; i < swapChainImages.size(); i++)
//		{
//			VkImageViewCreateInfo createInfo = {};
//			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//			createInfo.image = swapChainImages[i];
//			createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;  // 1D, 2D, 3D, cube maps
//			createInfo.format = swapChainImageFormat;
//
//			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // Can map all to Red if you want, or set value 0-1
//			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;	// This is default
//			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//
//			// What images purpose and which part of the image should be accessed
//			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;	// Color target
//			createInfo.subresourceRange.baseMipLevel = 0;
//			createInfo.subresourceRange.levelCount = 1;
//			createInfo.subresourceRange.baseArrayLayer = 0;
//			createInfo.subresourceRange.layerCount = 1;
//
//			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
//			{
//				throw std::runtime_error("failed to create image views!");
//			}
//		}
//
//	}
//
//	void createRenderPass()
//	{
//		VkAttachmentDescription colorAttachment = {};
//		colorAttachment.format = swapChainImageFormat;
//		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//
//		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	
//		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//		VkAttachmentReference colorAttachmentRef = {};
//		colorAttachmentRef.attachment = 0;
//		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//	
//		VkSubpassDescription subpass = {};
//		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		subpass.colorAttachmentCount = 1;
//		subpass.pColorAttachments = &colorAttachmentRef;
//
//		VkRenderPassCreateInfo renderPassInfo = {};
//		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		renderPassInfo.attachmentCount = 1;
//		renderPassInfo.pAttachments = &colorAttachment;
//		renderPassInfo.subpassCount = 1;
//		renderPassInfo.pSubpasses = &subpass;
//
//		VkSubpassDependency dependency = {};
//		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//		dependency.dstSubpass = 0;
//		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.srcAccessMask = 0;
//		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//
//		renderPassInfo.dependencyCount = 1;
//		renderPassInfo.pDependencies = &dependency;
//
//		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create render pass!");
//		}
//	}
//
//	void createGraphicsPipeline()
//	{
//		auto vertShaderCode = readFile("shaders/vert.spv");
//		auto fragShaderCode = readFile("shaders/frag.spv");
//
//		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
//
//		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
//		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//
//		vertShaderStageInfo.module = vertShaderModule;
//		vertShaderStageInfo.pName = "main";
//		vertShaderStageInfo.pSpecializationInfo = nullptr;
//
//		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
//		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//		fragShaderStageInfo.module = fragShaderModule;
//		fragShaderStageInfo.pName = "main";
//
//		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//
//		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
//		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//		auto bindingDescription = Vertex::getBindingDescription();
//		auto attributeDescriptions = Vertex::getAttributeDescriptions();
//
//		vertexInputInfo.vertexBindingDescriptionCount = 1;
//		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
//
//		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
//		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//		inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//		VkViewport viewport = {};
//		viewport.x = 0.0f;
//		viewport.y = 0.0f;
//		viewport.width = (float)swapChainExtent.width;
//		viewport.height = (float)swapChainExtent.height;
//		viewport.minDepth = 0.0f;
//		viewport.maxDepth = 1.0f;
//
//		VkRect2D scissor = {};
//		scissor.offset = { 0, 0 };
//		scissor.extent = swapChainExtent;
//
//		VkPipelineViewportStateCreateInfo viewportState = {};
//		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//		viewportState.viewportCount = 1;
//		viewportState.pViewports = &viewport;
//		viewportState.scissorCount = 1;
//		viewportState.pScissors = &scissor;
//
//		VkPipelineRasterizationStateCreateInfo rasterizer = {};
//		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//		rasterizer.depthClampEnable = VK_FALSE;
//		rasterizer.rasterizerDiscardEnable = VK_FALSE;
//		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//		rasterizer.lineWidth = 10.0f;
//		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//		rasterizer.depthBiasEnable = VK_FALSE;
//		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//		rasterizer.depthBiasClamp = 0.0f; // Optional
//		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
//
//		VkPipelineMultisampleStateCreateInfo multisampling = {};
//		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//		multisampling.sampleShadingEnable = VK_FALSE;
//		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//		multisampling.minSampleShading = 1.0f; // Optional
//		multisampling.pSampleMask = nullptr; // Optional
//		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
//		multisampling.alphaToOneEnable = VK_FALSE; // Optional
//
//		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
//		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//		colorBlendAttachment.blendEnable = VK_FALSE;
//		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
//		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
//
//		VkPipelineColorBlendStateCreateInfo colorBlending = {};
//		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//		colorBlending.logicOpEnable = VK_FALSE;
//		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
//		colorBlending.attachmentCount = 1;
//		colorBlending.pAttachments = &colorBlendAttachment;
//		colorBlending.blendConstants[0] = 0.0f; // Optional
//		colorBlending.blendConstants[1] = 0.0f; // Optional
//		colorBlending.blendConstants[2] = 0.0f; // Optional
//		colorBlending.blendConstants[3] = 0.0f; // Optional
//
//		VkDynamicState dynamicStates[] = 
//		{
//			VK_DYNAMIC_STATE_VIEWPORT,
//			VK_DYNAMIC_STATE_LINE_WIDTH
//		};
//
//		VkPipelineDynamicStateCreateInfo dynamicState = {};
//		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//		dynamicState.dynamicStateCount = 2;
//		dynamicState.pDynamicStates = dynamicStates;
//
//		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipelineLayoutInfo.setLayoutCount = 1;
//		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
//		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
//
//		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create pipeline layout!");
//		}
//
//		VkGraphicsPipelineCreateInfo pipelineInfo = {};
//		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//		pipelineInfo.stageCount = 2;
//		pipelineInfo.pStages = shaderStages;
//
//		pipelineInfo.pVertexInputState = &vertexInputInfo;
//		pipelineInfo.pInputAssemblyState = &inputAssembly;
//		pipelineInfo.pViewportState = &viewportState;
//		pipelineInfo.pRasterizationState = &rasterizer;
//		pipelineInfo.pMultisampleState = &multisampling;
//		pipelineInfo.pDepthStencilState = nullptr; // Optional
//		pipelineInfo.pColorBlendState = &colorBlending;
//		pipelineInfo.pDynamicState = nullptr; // Optional
//
//		pipelineInfo.layout = pipelineLayout;
//
//		pipelineInfo.renderPass = renderPass;
//		pipelineInfo.subpass = 0;
//
//		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//		pipelineInfo.basePipelineIndex = -1; // Optional
//
//		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create graphics pipeline!");
//		}
//
//		vkDestroyShaderModule(device, fragShaderModule, nullptr);
//		vkDestroyShaderModule(device, vertShaderModule, nullptr);
//	}
//
//	void createFramebuffers() 
//	{
//		swapChainFramebuffers.resize(swapChainImageViews.size());
//
//		for (size_t i = 0; i < swapChainImageViews.size(); i++) 
//		{
//			VkImageView attachments[] = { swapChainImageViews[i] };
//
//			VkFramebufferCreateInfo framebufferInfo = {};
//			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//			framebufferInfo.renderPass = renderPass;
//			framebufferInfo.attachmentCount = 1;
//			framebufferInfo.pAttachments = attachments;
//			framebufferInfo.width = swapChainExtent.width;
//			framebufferInfo.height = swapChainExtent.height;
//			framebufferInfo.layers = 1;
//
//			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) 
//			{
//				throw std::runtime_error("failed to create framebuffer!");
//			}
//		}
//	}
//
//	void createCommandPool() 
//	{
//		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
//
//		VkCommandPoolCreateInfo poolInfo = {};
//		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
//		poolInfo.flags = 0; // Optional
//
//		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create command pool!");
//		}
//	}
//
//	void createCommandBuffers() 
//	{
//		commandBuffers.resize(swapChainFramebuffers.size());
//
//		VkCommandBufferAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.commandPool = commandPool;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
//
//		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
//{
//			throw std::runtime_error("failed to allocate command buffers!");
//		}
//
//		for (size_t i = 0; i < commandBuffers.size(); i++) 
//		{
//			VkCommandBufferBeginInfo beginInfo = {};
//			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//			beginInfo.flags = 0; // Optional
//			beginInfo.pInheritanceInfo = nullptr; // Optional
//
//			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
//			{
//				throw std::runtime_error("failed to begin recording command buffer!");
//			}
//
//			VkRenderPassBeginInfo renderPassInfo = {};
//			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//			renderPassInfo.renderPass = renderPass;
//			renderPassInfo.framebuffer = swapChainFramebuffers[i];
//
//			renderPassInfo.renderArea.offset = { 0, 0 };
//			renderPassInfo.renderArea.extent = swapChainExtent;
//
//			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
//			renderPassInfo.clearValueCount = 1;
//			renderPassInfo.pClearValues = &clearColor;
//
//			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
//
//			for (int j = 0; j < cubes.size(); j++)
//			{
//				VkBuffer vertexBuffersObs[] = { vertexBuffers.at(j) };
//				VkDeviceSize offsets[] = { 0 };
//				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffersObs, offsets);
//				vkCmdBindIndexBuffer(commandBuffers[i], indexBuffers.at(j), 0, VK_INDEX_TYPE_UINT32);
//				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
//				vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(cubes[j].indices.size()), 1, 0, 0, 0);
//			}
//
//			vkCmdEndRenderPass(commandBuffers[i]);
//
//			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
//			{
//				throw std::runtime_error("failed to record command buffer!");
//			}
//		}
//	}
//
//	void createSyncObjects() {
//		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
//		imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
//		
//		VkSemaphoreCreateInfo semaphoreInfo{};
//		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//		VkFenceCreateInfo fenceInfo{};
//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
//				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
//				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
//
//				throw std::runtime_error("failed to create synchronization objects for a frame!");
//			}
//		}
//	}
//
//	VkShaderModule createShaderModule(const std::vector<char>& code) 
//	{
//		VkShaderModuleCreateInfo createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//		createInfo.codeSize = code.size();
//		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//		VkShaderModule shaderModule;
//		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to create shader module!");
//		}
//
//		return shaderModule;
//	}
//
//	void pickPhysicalDevice()
//	{
//		uint32_t deviceCount = 0;
//		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);    // Pass in memory for Number of GPU Devices
//		if (deviceCount == 0)
//		{
//			throw std::runtime_error("failed to find GPUs with Vulkan support!");   // If none we leave
//		}
//		std::vector<VkPhysicalDevice> devices(deviceCount);
//		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());     // Passes in container to hold all the devices
//
//		std::cout << "Number of Devives" << devices.size() << std::endl;
//		for (const auto& device : devices)
//		{        // Of our GPUs, find one that works
//			if (isDeviceSuitable(device))
//			{         // If we find one or more, use it (this picks which one too)
//				physicalDevice = device;
//				break;
//			}
//		}
//		if (physicalDevice == VK_NULL_HANDLE)
//		{
//			throw std::runtime_error("failed to find a suitable GPU!");
//		}
//	}
//
//	bool isDeviceSuitable(VkPhysicalDevice device) {
//		// Example
//		//VkPhysicalDeviceProperties deviceProperties;        // Name, type, Version
//		//vkGetPhysicalDeviceProperties(device, &deviceProperties);
//		//VkPhysicalDeviceFeatures deviceFeatures;            // Texture compression, 64 bit floats, multi viewport rendering(VR)
//		//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//		//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
//		//    deviceFeatures.geometryShader;
//
//		QueueFamilyIndices indices = findQueueFamilies(device);     // Returns a struct of Queues
//
//		bool extensionsSupported = checkDeviceExtensionSupport(device);
//
//		bool swapChainAdequate = false;
//		if (extensionsSupported)
//		{
//			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(); //If both are not empty
//		}
//		return indices.isComplete() && extensionsSupported && swapChainAdequate; // If Queue families exist, device extensions are supported and Swapchain details are adequate, they we can move on
//	}
//
//	bool checkDeviceExtensionSupport(VkPhysicalDevice device)
//	{
//		uint32_t extensionCount;
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//
//		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
//
//		// Outputting then to console
//		std::cout << "available device extensions:" << std::endl;
//		for (const auto& extension : availableExtensions) {
//			std::cout << "\t" << extension.extensionName << std::endl; // \t = tab
//		}
//
//		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//
//		// Go through available and delete them from the required
//		for (const auto& extension : availableExtensions)
//		{
//			requiredExtensions.erase(extension.extensionName);
//		}
//		// If all are deleted from required then they exist, if not returns false and we got a problem
//		return requiredExtensions.empty();
//	}
//
//	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
//	{
//		QueueFamilyIndices indices;
//
//		uint32_t queueFamilyCount = 0;
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); // Takes device, count, container
//		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//		int i = 0;
//		auto result = queueFamilies.at(0).queueFlags & VK_QUEUE_GRAPHICS_BIT;
//		for (const auto& queueFamily : queueFamilies)
//		{						// From all the Queue families
//			std::cout << "QueueFamily: " << i << std::endl;
//			std::cout << "Flag " << (queueFamily.queueFlags) << std::endl;
//			std::cout << "Graphics " << ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
//			std::cout << "Compute " << ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
//			std::cout << "Transfer " << ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
//			std::cout << "Protected " << ((queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) << std::endl;
//			std::cout << "Sparse " << ((queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
//			std::cout << "count: " << queueFamily.queueCount << std::endl;
//			std::cout << "" << std::endl;
//
//			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 )		// Bitwise calculation that results in 1 if correct (1 = true)
//			{			// If it has Queue graphics bit, one specific family grouping 
//				indices.graphicsFamily = i;			// The list is ordered and stored as ints, if it has this family, assign it index of Graphics bit so we know where it is
//			}
//			VkBool32 presentSupport = false;
//			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
//			if (presentSupport)
//			{
//				indices.presentFamily = i;
//			}
//
//			if (indices.isComplete())
//			{		// WE only need 1 so if sound, break;
//				break;
//			}
//			i++;
//		}
//		return indices;
//	}
//
//	bool checkValidationLayerSupport()
//	{
//		uint32_t layerCount;
//		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//		std::vector<VkLayerProperties> availableLayers(layerCount);
//		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
//
//		for (const char* layerName : validationLayers) 	// For every predtermined layer (we have 1)
//		{
//			bool layerFound = false;
//			for (const auto& layerProperties : availableLayers) {				//  For every available layer
//				if (strcmp(layerName, layerProperties.layerName) == 0) {		// If Available matches predetermiend
//					layerFound = true;											// WE got her
//					break;
//				}
//			}
//			if (!layerFound) {		// Not found, cant use
//				return false;
//			}
//		}
//		return true;
//	}
//
//	void initWindow()
//	{
//		glfwInit();  // Start it
//		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // Tell it its not OpenGL
//		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);     // Resize isn't straigth forward, set false
//		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // 4th is which monitor, 5th is GL(no use)
//		glfwSetWindowUserPointer(window, this);
//		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
//	}
//
//	void mainLoop()
//	{
//		while (!glfwWindowShouldClose(window)) {
//			glfwPollEvents();
//			drawFrame();
//		}
//	}
//
//	void drawFrame() 
//	{
//		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
//
//		uint32_t imageIndex;
//		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
//
//		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
//			recreateSwapChain();
//			return;
//		}
//		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
//			throw std::runtime_error("failed to acquire swap chain image!");
//		}
//
//		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
//		{
//			vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
//		}
//		imagesInFlight[imageIndex] = inFlightFences[currentFrame]; 
//		
//		updateUniformBuffer(imageIndex);
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
//		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//		submitInfo.waitSemaphoreCount = 1;
//		submitInfo.pWaitSemaphores = waitSemaphores;
//		submitInfo.pWaitDstStageMask = waitStages;
//
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
//
//		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
//		submitInfo.signalSemaphoreCount = 1;
//		submitInfo.pSignalSemaphores = signalSemaphores;
//
//		vkResetFences(device, 1, &inFlightFences[currentFrame]);
//
//		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
//		{
//			throw std::runtime_error("failed to submit draw command buffer!");
//		}
//		VkPresentInfoKHR presentInfo = {};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//		presentInfo.waitSemaphoreCount = 1;
//		presentInfo.pWaitSemaphores = signalSemaphores;
//
//		VkSwapchainKHR swapChains[] = { swapChain };
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = swapChains;
//		presentInfo.pImageIndices = &imageIndex;
//		presentInfo.pResults = nullptr; // Optional
//
//		result = vkQueuePresentKHR(presentQueue, &presentInfo);
//
//		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
//			framebufferResized = false;
//			recreateSwapChain();
//		}
//		else if (result != VK_SUCCESS) {
//			throw std::runtime_error("failed to present swap chain image!");
//		}
//
//		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//		vkQueueWaitIdle(presentQueue);
//	}
//
//	void updateUniformBuffer(uint32_t currentImage) 
//	{
//		static auto startTime = std::chrono::high_resolution_clock::now();
//
//		auto currentTime = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//
//		UniformBufferObject ubo{};
//		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		ubo.view = glm::lookAt(glm::vec3(5.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
//		ubo.proj[1][1] *= -1;
//
//		void* data;
//		vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
//	}
//
//	void cleanup()
//	{
//		cleanupSwapChain();
//
//		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
//
//		vkDestroyBuffer(device, indexBuffer, nullptr);
//		vkFreeMemory(device, indexBufferMemory, nullptr);
//
//		vkDestroyBuffer(device, vertexBuffer, nullptr);
//		vkFreeMemory(device, vertexBufferMemory, nullptr);
//
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
//			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
//			vkDestroyFence(device, inFlightFences[i], nullptr);
//		}
//
//		vkDestroyCommandPool(device, commandPool, nullptr);
//
//		vkDestroyDevice(device, nullptr);
//
//		vkDestroySurfaceKHR(instance, surface, nullptr);
//		vkDestroyInstance(instance, nullptr);
//
//		glfwDestroyWindow(window);
//
//		glfwTerminate();
//		glfwTerminate();
//	}
//};
//
//int main()
//{
//	HelloTriangleApplication app;
//
//	try {
//		app.run();
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//	return EXIT_SUCCESS;
//}
//
//static std::vector<char> readFile(const std::string& filename) {
//	std::ifstream file(filename, std::ios::ate | std::ios::binary);
//
//	if (!file.is_open()) {
//		throw std::runtime_error("failed to open file!");
//	}
//
//	size_t fileSize = (size_t)file.tellg();
//	std::vector<char> buffer(fileSize);
//
//	file.seekg(0);
//	file.read(buffer.data(), fileSize);
//
//	file.close();
//
//	return buffer;
//}
//
//static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
//	auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
//	app->framebufferResized = true;
//}
//
//
//
//// 4 quads
//
////const std::vector<Vertex> vertices = {
////	{{-0.75f, -0.75f}, {1.0f, 0.0f, 0.0f}},
////	{{-0.25f, -0.75f},  {0.0f, 1.0f, 0.0f}},
////	{{-0.25f, -0.25f}, {1.0f, 1.0f, 1.0f}},
////	{{-0.75f, -0.25f}, {0.0f, 0.0f, 1.0f}},
////
////	{{0.25f, 0.25f}, {1.0f, 0.0f, 0.0f}},
////	{{0.75f, 0.25f},  {0.0f, 1.0f, 0.0f}},
////	{{0.75f, 0.75f}, {1.0f, 1.0f, 1.0f}},
////	{{0.25f, 0.75f}, {0.0f, 0.0f, 1.0f}},
////
////	{{0.25f, -0.75f}, {1.0f, 0.0f, 0.0f}},
////	{{0.75f, -0.75f}, {0.0f, 1.0f, 0.0f}},
////	{{0.75f, -0.25f}, {1.0f, 1.0f, 1.0f}},
////	{{0.25f, -0.25f}, {0.0f, 0.0f, 1.0f}},
////
////	{{-0.75f, 0.25f}, {1.0f, 0.0f, 0.0f}},
////	{{-0.25f, 0.25f},  {0.0f, 1.0f, 0.0f}},
////	{{-0.25f, 0.75f}, {1.0f, 1.0f, 1.0f}},
////	{{-0.75f, 0.75f}, {0.0f, 0.0f, 1.0f}}
////
////};
//
////
////const std::vector<uint32_t> indices = {
////	0, 1, 2, 2, 3, 0, 
////	4, 5, 6, 6, 7, 4,
////	8, 9, 10, 10, 11, 8,
////	12, 13, 14, 14, 15, 12
////};
//
//
//// Cube
//
////std::vector<Vertex> vertices = {
////
////	// Front Face
////	{ { -1.00f, -1.00f, 1.00f }, {1.0f, 0.0f, 0.0f }},
////	{ { 1.00f, -1.00f , 1.00f } , { 0.0f, 1.0f, 0.0f}},
////	{ {	 1.00f,  1.00f, 1.00f  } , {1.0f, 1.0f, 1.0f}},
////	{ {	-1.00f,  1.00f, 1.00f } , { 0.0f, 0.0f, 1.0f}},
////
////	// Top Face
////{ {	-1.00f,  1.00f,  1.00f } , { 1.0f, 0.0f, 0.0f}},
////{ {	 1.00f,  1.00f, 1.00f } , { 0.0f, 1.0f, 0.0f}},
////{ {	 1.00f,  1.00f, -1.00f  } , { 1.0f, 1.0f, 1.0f}},
////{ {	-1.00f,  1.00f,  -1.00f } , { 0.0f, 0.0f, 1.0f}},
////
////// Back Face
////{ {	 1.00f, -1.00f, -1.00f  } , { 1.0f, 0.0f, 0.0f}},
////{ {	-1.00f, -1.00f,  -1.00f } , {0.0f, 1.0f, 0.0f}},
////{ {	-1.00f,  1.00f, -1.00f } , { 1.0f, 1.0f, 1.0f}},
////{ {	 1.00f,  1.00f , -1.00f } , {0.0f, 0.0f, 1.0f}},
////
////// Bottom Face
////{{-1.00f, -1.00f , -1.00f } , {1.0f, 0.0f, 0.0f}},
////{{ 1.00f, -1.00f, -1.00f  } , {0.0f, 1.0f, 0.0f}},
////{{ 1.00f, -1.00f, 1.00f  } , { 01.0f, 1.0f, 1.0f}},
////{{-1.00f, -1.00f , 1.00f } , {0.0f, 0.0f, 1.0f}},
////
////// Left Face
////{{-1.00f, -1.00f, -1.00f } , {1.0f, 0.0f, 0.0f}},
////{{-1.00f, -1.00f , 1.00f } , {0.0f, 1.0f, 0.0f }},
////{{-1.00f,  1.00f , 1.00f } , { 1.0f, 1.0f, 0.0f}},
////{{-1.00f,  1.00f, -1.00f  } , {0.0f, 0.0f, 1.0f }},
////
////// Right Face
////{ { 1.00f, -1.00f, 1.00f  } , { 1.0f, 0.0f, 0.0f}},
////{ { 1.00f, -1.00f, -1.00f  } , { 0.0f, 1.0f, 0.0f}},
////{ { 1.00f,  1.00f, -1.00f  } , { 1.0f, 1.0f, 1.0f}},
////{ { 1.00f,  1.00f , 1.00f } , { 0.0f, 0.0f, 1.0f}}
////
////
////
////
////
////};
////
////const std::vector<uint32_t>indices = {
////	// Front Face
////	0, 1, 2,
////	2, 3, 0,
////
////	// Top Face
////	4, 5, 6,
////	6, 7, 4,
////
////	// Back Face
////	8, 9, 10,
////	10, 11, 8,
////
////	// Bottom Face
////	12, 13, 14,
////	14, 15, 12,
////
////	// Left Face
////	16, 17, 18,
////	18, 19, 16,
////
////	// Right Face
////	20, 21, 22,
////	22, 23, 20
////};
//
//
