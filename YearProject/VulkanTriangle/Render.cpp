#include "Render.h"

float Render::speed{ 1 };
VkClearValue Render::backgroundColor{ 0.5f, 0.5f, 0.5f, 1.0f };		// Clear color - grey
float Render::zoom{ 400.0f };
float Render::timeTaken{ 0.0f };
bool Render::pathFind{ false };
Render::Render()
{
}

void Render::setupVulkan(GLFWwindow* t_window)
{
	// Function called my the main to setup all the initalization functions for the program
	createVulkanInstance();	
	creatVulkanSurface(t_window);	
	chooseGPUDevice();	
	createVulkanLogicalDevice();	
	createSwapChain();	
	createImageViews();		
	createVulkanRenderPass(); 
	createDescriptorSetLayout();	
	createVulkanGraphicsPipeline(); 
	createComputePipelineLayout();			
	createComputePipeline("shaders/astar.spv");	
	createFrameBuffers();		
	createCommandPool();
	createNodeBuffer(buffersNodes, 1, numOfAgents, 1);		
	createPathsBuffer(buffersPaths, 1, numOfAgents, 0);		

	createInstanceBuffer();
	//createVertexBuffer(*cubes->at(0), vertexBuffers.at(0), vertexBufferMemorys.at(0));

	for (int i = 0; i < cubes->size(); i++)
	{
		createVertexBuffer(*cubes->at(i), vertexBuffers.at(i), vertexBufferMemorys.at(i));
	}
	createIndexBuffer(*cubes->at(0), indexBuffers.at(0), indexBufferMemorys.at(0));			
	allocateBufferMemoryAndBind(buffersNodes, memoryNode, 0);	
	allocateBufferMemoryAndBind(buffersPaths, memoryPaths, 1);	
	createUniformBuffers();			
	createDescriptorPool();		
	m_buffers.push_back(buffersNodes);
	m_buffers.push_back(buffersPaths);
	createDescriptorSets();				
	allocateDescriptorSets(m_buffers);	
	createComputeCommandPoolAndBuffer();					
	createCommandBuffers();		
	createSyncObjects();		 

	// This sets up the movement of blocks through the maze after pathfinding
	nexts.resize(numOfAgents);
	for (int i = 0; i < numOfAgents; i++)
	{
		nexts.at(i) = 1;
	}



	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(device, &pool_info, nullptr, &guiPool);

	}

	{
		VkAttachmentDescription attachment = {};
		attachment.format = VK_FORMAT_B8G8R8A8_SRGB;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		if (vkCreateRenderPass(device, &info, nullptr, &imGuiRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("Could not create Dear ImGui's render pass");
		}
	}




	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);


	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = device;
	init_info.QueueFamily = indices.graphicsFamily.value();
	init_info.Queue = graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = guiPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = swapChainImages.size();
	init_info.ImageCount = swapChainImages.size();
	//init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);


	createCommandPool(&guiPools, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	guiCMDbuffers.resize(swapChainImageViews.size());
	createCommandBuffers(guiCMDbuffers.data(), static_cast<uint32_t>(guiCMDbuffers.size()), guiPools);
	// Upload Fonts
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;		// Only 1 call to this command


		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(guiCMDbuffers[0], &begin_info);

		ImGui_ImplVulkan_CreateFontsTexture(guiCMDbuffers[0]);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &guiCMDbuffers[0];
		vkEndCommandBuffer(guiCMDbuffers[0]);

		vkQueueSubmit(graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

		vkDeviceWaitIdle(device);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = imGuiRenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = swapChainExtent.width;
		info.height = swapChainExtent.height;
		info.layers = 1;
		guiFrameBuffers.resize(swapChainImageViews.size());
		for (uint32_t i = 0; i < swapChainImageViews.size(); i++)
		{
			attachment[0] = swapChainImageViews.at(i);
			vkCreateFramebuffer(device, &info, nullptr, &guiFrameBuffers.at(i));
		}

	}
}


void Render::createCommandPool(VkCommandPool* commandPool, VkCommandPoolCreateFlags flags) {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	commandPoolCreateInfo.flags = flags;

	if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Could not create graphics command pool");
	}
}

void Render::createCommandBuffers(VkCommandBuffer* commandBuffer, uint32_t commandBufferCount, VkCommandPool& commandPool) {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.commandBufferCount = commandBufferCount;
	vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffer);
}

void Render::createVulkanInstance()
{
	// To allow Vulkan to be generic a lot of the functionality is abstracted away into extentions
	// Different GPUs can support some of these
	uint32_t extensionCount = 0;                                                          // Create some memory to count them                                   
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);            // Layer, memory for count, memory for assignment (I use default layer for now)
	std::vector<VkExtensionProperties> extensions(extensionCount);                        // Make a vector with the exact size of the count
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());  // Call again with container

	// These next 2 go hand in hand, app info first
	VkApplicationInfo appInfo = {}; // Defalt init, leaves .pnext as nullptr, pnext tells the memory where to go next after use, nullptr deletes it
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;  // Most structs have this, like an ID
	appInfo.pApplicationName = "AStar";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t glfwExtensionCount = 0;    // How many, passed as memory and counted
	const char** glfwExtensions;        // Name list is returned
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);    // Vulkan instance extensions needed for GLFW windowing

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;						// Details info from above
	createInfo.enabledLayerCount = 0;
	createInfo.enabledExtensionCount = glfwExtensionCount;      // This info is added to the info struct
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	// Creat instance with info setup, returns error if it fails
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) // Creates instance
		throw std::runtime_error("failed to create instance!");
}

void Render::creatVulkanSurface(GLFWwindow* t_window)
{
	// Instance specific, window to map to and memory to hold it.
	if (glfwCreateWindowSurface(instance, t_window, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
}

void Render::chooseGPUDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);    // Pass in memory for Number of GPU Devices
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");   // If none we leave
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());     // Passes in container to hold all the devices

	for (const auto& device : devices)		// Check all devices
	{       
		if (isDeviceSuitable(device))	
		{        
			physicalDevice = device;	 // If we find a device that matches our criteria, we assign it
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

}

void Render::createVulkanLogicalDevice()
{
	// Queue families are just logical groupings of queues e.g graphics Qs, presentation Qs, compute Qs
	// Here we check our GPU for what it supports and sotres the info
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);		// Check for available queue families are store them

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;						
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };  // While these are likely the same, it's best to safe

	float queuePriority = 1.0f;							// Priority of the Queue used for ordering, 0.0 -> 1.0 scale for advanced control
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;							// Can have more than 1 but 1 is simpiler for now
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	VkPhysicalDeviceFeatures deviceFeatures = {};		// Features on device, none atm

	// General Logical device info, combining the queue created above with any device features that you might need
	VkDeviceCreateInfo createInfo = {};					// Logical device
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();		// Our created queues
	createInfo.pEnabledFeatures = &deviceFeatures;				// Device features (none)
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());	 // How many extentsions
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();		// The extensions, we check earlier if the were supported

	//Creat the Logical device
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	// Store the Queues for use elsewhere
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Render::createSwapChain()
{
	// Ask GPU what swapchain info it can do
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	// Choose the ones we want from what is available
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);		// SRBG and non linear color space
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);		// Mailbox buffer: triple buffering where the last image is replaced by new one's instead of discarded
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);						// Define extent to our window, if we can't take default.

	// If we use the minimum, then we may have to wait for the driver to make another image, so 1 extra
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	// Max count of 0 is a special value meaning there is no max
	// Overwrite minimum
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;								// Link to window
	createInfo.minImageCount = imageCount;						// Minimum images it should have
	createInfo.imageFormat = surfaceFormat.format;				// Pixel detail info
	createInfo.imageColorSpace = surfaceFormat.colorSpace;		// Type (non linear, better for screens)
	createInfo.imageExtent = extent;							// Window Size and res info
	createInfo.imageArrayLayers = 1;							// Layers each image has, 1 unless Stereoscopic games
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	// If we wanted post processing, we would chnage this to transfer but we dont

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)						// Determine if the Graphics and Presentation families are the same or not
	{		// If not
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;					// how many
		createInfo.pQueueFamilyIndices = queueFamilyIndices;	// The data set
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;		// Such as rotating the image 90 degrees, here we do nothing
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;					// aplha channel for blending
	createInfo.presentMode = presentMode;											// Mailbox
	createInfo.clipped = VK_TRUE;													// Use clipping
	createInfo.oldSwapchain = VK_NULL_HANDLE;										// IF the chain becomes invalid ( we do nothing rn)

	// Store these for use and checks later
	swapChainImageFormat = surfaceFormat.format;									
	swapChainExtent = extent;

	// Create the Chain
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	// Get some images and store them for operations
	overallImageCount = imageCount;
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}

void Render::createImageViews()
{
	// Each Image needs a view for the user to see it
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;				// 1D, 2D, 3D, cube maps
		createInfo.format = swapChainImageFormat;					// Created earlier

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;	// Can map all to Red if you want, or set value 0-1
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;	// This is default
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// What images purpose and which part of the image should be accessed
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		// Color target
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Render::createVulkanRenderPass()
{
	// We just need 1 color attachment
	VkAttachmentDescription colorAttachment = {};			
	colorAttachment.format = swapChainImageFormat;							// Same format as our images
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;						// Only 1 sample, no multisampling
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;					// What to do with the attachment data before rendering, you can preserve info but I want to clear
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// What to do with the attachment data after rendering, need to store it so it can bee seen on screen
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;		// Unused
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// Doesn't matter
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			// To see it

	VkAttachmentReference colorAttachmentRef = {};							
	colorAttachmentRef.attachment = 0;										// Index, we have one so its 0, (Layout = 0) in Fragment shader
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// Color buffer

	// There must be 1 subpass, but there can be many
	VkSubpassDescription subpass = {};					
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
	subpass.colorAttachmentCount = 1;									
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Combining the above
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void Render::createDescriptorSetLayout()
{
//	VS:
//	layout(set = 0, binding = 0) uniform UBOMatrices;
//	FS:
//	layout(set = 0, binding = 1) uniform sampler2D;
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;		// UBO, not image
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;		// Vertext shader, VS

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void Render::createVulkanGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");			// Load in shaders, which are in bytecode SPIR-V
	auto fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);			// Create the Shader module
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// Vertext shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;			// Type

	vertShaderStageInfo.module = vertShaderModule;					// Created above
	vertShaderStageInfo.pName = "main";								// Which function in shader to invoke
	vertShaderStageInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//auto bindingDescription = Vertex3::getBindingDescription();								// Binding
	//auto attributeDescriptions = Vertex3::getAttributeDescriptions();						// Layout positions
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;


	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding = 0;
	vInputBindDescription.stride = sizeof(Vertex3);
	vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputBindingDescription vInputBindDescription2{};
	vInputBindDescription2.binding = 1;
	vInputBindDescription2.stride = sizeof(InstanceData);
	vInputBindDescription2.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	bindingDescriptions = {
		// Binding point 0: Mesh vertex layout description at per-vertex rate
		vInputBindDescription,
		// Binding point 1: Instanced data at per-instance rate
		vInputBindDescription2 
	};

	VkVertexInputAttributeDescription vInputAttribDescription{};
	vInputAttribDescription.location = 0;
	vInputAttribDescription.binding = 0;
	vInputAttribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	vInputAttribDescription.offset = 0;

	VkVertexInputAttributeDescription vInputAttribDescription2{};
	vInputAttribDescription2.location = 1;
	vInputAttribDescription2.binding = 0;
	vInputAttribDescription2.format = VK_FORMAT_R32G32B32_SFLOAT;
	vInputAttribDescription2.offset = sizeof(float) * 3;

	// inst
	VkVertexInputAttributeDescription vInputAttribDescription3{};
	vInputAttribDescription3.location = 2;
	vInputAttribDescription3.binding = 1;
	vInputAttribDescription3.format = VK_FORMAT_R32G32B32_SFLOAT;
	vInputAttribDescription3.offset = 0;

	attributeDescriptions = {
		vInputAttribDescription, vInputAttribDescription2, vInputAttribDescription3	
	};

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;			
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;						// Triangles, not lines or points
	inputAssembly.primitiveRestartEnable = VK_FALSE;									// break up lines is a strip

	VkViewport viewport = {};									
	viewport.x = 0.0f;																// Top corner
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;									// Match width, height
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;														
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };														// Special view cliping
	scissor.extent = swapChainExtent;

	// Mixture of the 2 above
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;								// Clamp parts at clipping planes if true
	rasterizer.rasterizerDiscardEnable = VK_FALSE;						// Never gets rasterized
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;						// Can be lines or points
	rasterizer.lineWidth = 0.0f;										
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;						
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;			
	rasterizer.depthBiasEnable = VK_FALSE;

	// Mutlisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;			// Not used

	// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	// Layour is for uniform values in the shader, we will use it for the Camera
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;				// Setup in another function

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	// Combining all the structs above to create the Graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;		// created in the last function
	pipelineInfo.subpass = 0;					// Index

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = 2;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipelineNONinstanced) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void Render::createFrameBuffers()
{
	// For every image and image View, we need a frambuffer to display them
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;		// The render pass that will act on them
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;		// The image attachments declared before
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;						// Only 1 image in the image array

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Render::createCommandPool()
{
	// Command buffers set instructions for the draw/ actions fo the GPU
	// The are allocated from pools
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);	// Get the Queues needed

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();	// Graphics

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void Render::creatBufferObjects(int t_count)
{

	// Movable cubes
	for (int j = 0; j < t_count; j++)
	{
		VkBuffer buffer2; VkDeviceMemory bufferMem2;
		vertexBuffers.push_back(buffer2);
		vertexBufferMemorys.push_back(bufferMem2);
	}

	VkBuffer index; VkDeviceMemory indexMem;
	indexBuffers.push_back(index);
	indexBufferMemorys.push_back(indexMem);
}

void Render::addVBOs(std::vector<Cube*>* t_cubesMoveable/*, std::vector<Cube*>* t_cubesUnmoved*/)
{
	//cubesMoveable = t_cubesMoveable;
	cubes = t_cubesMoveable;
}

void Render::createVertexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
	// Size of the buffer is the size of 1 vertex * the amounf of them
	VkDeviceSize bufferSize = sizeof(t_cube.vertices[0]) * t_cube.vertices.size();

	// Create a staging buffer on Host visible memory, we can then transfer it to the Device local memory, which is fastest
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	// Copy the buffer infor into our allocated buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, t_cube.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	// Create a buffer for device memory
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, t_vertexbuffer, t_vertexbuffermemory);
	// Copy the staging buffer into the Device local buffer
	copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
}

void Render::updateBufferMemory(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
	// Updates the buffer memeory for a cube
	VkDeviceSize bufferSize = sizeof(t_cube.vertices[0]) * t_cube.vertices.size();

	// Stage the memory, the copy it to the device
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, t_cube.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
}

void Render::resetAgents()
{

	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_real_distribution<> dis(0.0, gridSizeTotal - 1);

	//std::vector<int> starts;
	//for (int i = 0; i < numOfAgents; i++)
	//{
	//	long int oop = dis(gen);
	//	while (nodes.at(oop)->passable == 0)
	//	{
	//		oop = dis(gen);
	//	}
	//	starts.push_back(oop);
	//	Cube* cub = new Cube(m_matchingPositions.at(oop).x, m_matchingPositions.at(oop).y, 2.0f);
	//	cub->updateColor(glm::vec3(0.0f, 0.0f, 0.0f));
	//	m_gameCubes.push_back(cub);
	//}
	//m_renderer.setStarts(starts);

	// Set new cubes positions
	// Update cubes to position
	// Reset node data
	// update storage buffers


	// reseting the Cubes to their starting places
	for (int i = 0; i < numOfAgents; i++)
	{
		float oop = matchingPos->at(backfinalPaths.at(i).at(0)).x - matchingPos->at(backfinalPaths.at(i).at(backfinalPaths.at(i).size() -1)).x;
		float oop2 = matchingPos->at(backfinalPaths.at(i).at(0)).y - matchingPos->at(backfinalPaths.at(i).at(backfinalPaths.at(i).size() - 1)).y;
		cubes->at(cubes->size() - ((numOfAgents + 1) - i))->updatePos(oop, oop2);
		updateBufferMemory(*cubes->at(cubes->size() - ((numOfAgents + 1) - i)), vertexBuffers.at(cubes->size() - ((numOfAgents + 1) - i)), vertexBufferMemorys.at(cubes->size() - ((numOfAgents + 1) - i)));
		
	}
	next = 1;
	last = 0;
}

void Render::updateCameraPosition(glm::vec3 t_changeInCameraPosition, int t_specify)
{
	if (t_specify == 0)
	{
		eye += t_changeInCameraPosition;
		lookAT += t_changeInCameraPosition;
	}
	else if (t_specify == 1)
	{
		eye += t_changeInCameraPosition;
	}
	else
	{
		lookAT += t_changeInCameraPosition;
	}
	
}

void Render::createInstanceBuffer()
{
	std::vector<InstanceData> instanceDatas;

	for (auto i = 0; i < nodes->size(); i++)
	{
		if (nodes->at(i)->passable == 0)
		{
			InstanceData inst;
			inst.pos.x = matchingPos->at(i).x;
			inst.pos.y = matchingPos->at(i).y;
			instanceDatas.push_back(inst);
		}
	}
	instanceBuffer.size = instanceDatas.size() * sizeof(InstanceData);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(instanceBuffer.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, instanceBuffer.size, 0, &data);
	memcpy(data, instanceDatas.data(), (size_t)instanceBuffer.size);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(instanceBuffer.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBuffer.buffer, instanceBuffer.memory);

	copyBuffer(stagingBuffer, instanceBuffer.buffer, instanceBuffer.size);

	instanceBuffer.descriptor.range = instanceBuffer.size;
	instanceBuffer.descriptor.buffer = instanceBuffer.buffer;
	instanceBuffer.descriptor.offset = 0;

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Render::createIndexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
	// Same as the vertex buffer creation
	VkDeviceSize bufferSize = sizeof(t_cube.indices[0]) * t_cube.indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, t_cube.indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, t_vertexbuffer, t_vertexbuffermemory);

	copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Render::createUniformBuffers()
{
	// Uniform buffer object
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	// One for each image
	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());	// Memory to access the buffer

	// Create each of the buffers for the frames
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void Render::createDescriptorPool()
{
	// Descriptors are allocated from Pools that tell the driver what types to use and how to use them
	VkDescriptorPoolSize poolSizeGraphics{};
	poolSizeGraphics.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;		// For a UBO
	poolSizeGraphics.descriptorCount = static_cast<uint32_t>(swapChainImages.size()); // One for each image

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSizeGraphics;
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = 1;

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;		// For the storage buffer
	poolSize.descriptorCount = static_cast<uint32_t>(2);

	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &poolSize;
	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPoolCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}

}

void Render::createDescriptorSets()
{
	// Needs an array of the same size from layouts
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;			// Pool declared earlier
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();		 // Descriptor set layout from before

	// Allocate a descriptor set for each image
	descriptorSets.resize(swapChainImages.size());
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		// Create writing object top tell the driver you are ovewriting the sata
		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;	// Uniform
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		// Update it
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Render::createCommandBuffers()
{
	// Command buffer for every image
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	// Allocate the buffers
	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	// For every buffer
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		// start the instructions
		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;						// Render pass to reference
		renderPassInfo.framebuffer = swapChainFramebuffers[i];		// Frame buffer to draw on

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = Render::backgroundColor;//{ 0.5f, 0.5f, 0.5f, 1.0f };		// Clear color - grey
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);		// start render pass

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);	// Bind the graphics pipeline
		


		VkDeviceSize offsetz[1] = { 0 };
			// Instanced rocks
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, NULL);
		// Binding point 0 : Mesh vertex buffer
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffers.at(0), offsetz);
		// Binding point 1 : Instance data buffer
		vkCmdBindVertexBuffers(commandBuffers[i], 1, 1, &instanceBuffer.buffer, offsetz);
		// Bind index buffer
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffers.at(0), 0, VK_INDEX_TYPE_UINT32);

		// Render instances
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>((*cubes)[0]->indices.size()), (*nodes).size(), 0, 0, 0);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineNONinstanced);	// Bind the graphics pipeline


		//// for every cube
		//int whatsLeft = cubes->size();
		for (int j = 0; j < cubes->size(); j++)
		{
			VkBuffer vertexBuffersObs[] = { vertexBuffers.at(j) };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffersObs, offsets);		// Bind the vertex data
			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffers.at(0), 0, VK_INDEX_TYPE_UINT32);		// Bind a cube index buffer
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);	// Bind the descriptor sets on how to interact with the vertex shader for this cube
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>((*cubes)[j]->indices.size()), 1, 0, 0, 0);		// Draw the cube
		}

		vkCmdEndRenderPass(commandBuffers[i]);		// end render pass

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)		// End buffer
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
	 // Compute command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(computeCommandBuffer, &beginInfo);
	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineCompute); // Bind compute pipeline
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayoutCompute, 0, 1, &descriptorSetCompute, 0, nullptr); // Bind compute descriptor sets
	vkCmdDispatch(computeCommandBuffer, numOfAgents, 1, 1);		// Bind dispatch the operation
	if (vkEndCommandBuffer(computeCommandBuffer) != VK_SUCCESS)			// end buffer
	{
		throw std::runtime_error("failed to end command buffer");
	}

	// Setting up node and path buffer initial values
	std::vector<Node> temp;
	for (auto& nod : *nodes)
	{
		temp.push_back(*nod);
	}
	std::vector<NodeData*> agents;
	agents.resize(numOfAgents);
	Path fillerData;
	for (int i = 0; i < pathMax; i++)
	{
		fillerData.pathList[i] = -1;
	}
	std::vector<Path*> praths;
	praths.resize(numOfAgents);


	// Node SSBO mapping =======================================
		// mapping data to the storage buffer
	NodeData* data = nullptr;
	if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}

	for (int i = 0; i < numOfAgents; i++)
	{
		agents.at(i) = data + i;
		agents.at(i)->start = starts.at(i);
		agents.at(i)->goal = goalID;
		std::copy(std::begin(temp), std::end(temp), std::begin(agents.at(i)->nodes));
	}
	vkUnmapMemory(device, memoryNode);
	// ============================================================


	// Paths SSBO mapping =========================================
	// mapping data to the storage buffer
	Path* dataPaths = nullptr;
	if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&dataPaths)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}
	for (int i = 0; i < numOfAgents; i++)
	{
		praths.at(i) = dataPaths + i;
		std::copy(std::begin(fillerData.pathList), std::end(fillerData.pathList), std::begin(praths.at(i)->pathList));
	}
	vkUnmapMemory(device, memoryPaths);
}

void Render::createSyncObjects()
{
	// Setting up Semaphores and Fences to manage the frames in the the triple buffer swapchain
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void Render::draw()
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	if (pathFind == true && update == false)
	{
		pathFind = false;
		update = true;

		VkSubmitInfo submitInfo2 = {};
		submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo2.commandBufferCount = 1;
		submitInfo2.pCommandBuffers = &computeCommandBuffer;

		// Submiting of the Compute shader after input, timed with Chrono  ======
		auto start = std::chrono::high_resolution_clock::now();
		vkQueueSubmit(graphicsQueue, 1, &submitInfo2, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		float seconds = duration.count();
		seconds /= 1000000;
		std::cout << "Time taken by GPU: "
			<< duration.count() << " microseconds" << " = " << seconds << " seconds" << std::endl;
		// =======================================
		timeTaken = seconds;

		//std::ofstream filestr;
		//std::string stg = "results250_200_20.txt";
		//filestr.open(stg, std::fstream::in | std::fstream::out | std::fstream::app);
		//stg.clear();
		//if (filestr.fail())
		//{
		//	std::cout << "Error" << std::endl;
		//}
		//filestr << seconds;
		//filestr << std::endl;
		//filestr.close();

		// Retrieving the data from the compute execution
		// The data from the Node storage buffer
		NodeData* data = nullptr;
		if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
			throw std::runtime_error("failed to map device memory");
		}
		backData.resize(numOfAgents);			
		for (int i = 0; i < numOfAgents; i++)
		{
			backData.at(i) = data + i;
		}
		vkUnmapMemory(device, memoryNode);

		// the Path data from the compute shader
		Path* pathsReturned = nullptr;
		if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&pathsReturned)) != VK_SUCCESS) {
			throw std::runtime_error("failed to map device memory");
		}
		backPaths.resize(numOfAgents);
		for (int i = 0; i < numOfAgents; i++)
		{
			backPaths.at(i) = pathsReturned + i;
		}
		vkUnmapMemory(device, memoryPaths);

		// Removing unused elements in the path buffer
		backfinalPaths.clear();
		for (int j = 0; j < numOfAgents; j++)
		{
			std::vector<int> p;
			p.push_back(goalID);
			for (int i = 0; i < pathMax; i++)
			{
				if (backPaths.at(j)->pathList[i] != -1)
				{
					p.push_back(backPaths.at(j)->pathList[i]);
				}
				else
				{
					break;
				}
			}
			// Revers the path as it starts at the goal
			std::reverse(p.begin(), p.end());
			backfinalPaths.push_back(p);		// One path for each agent
		}
	}
	pathFind = false;

	// The Rendering
	// Get the next image in the swapchain
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	currentIndex = imageIndex;
	// If an error recreate the swapchain
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// If the image isn't ready
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);	// Wait for it to declare that it is ready
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];		// Set the new fence


	// Updating the cubes after compute shader
	if (wait > 500 && update == true)
	{
		wait = 0;
		for (int i = 0; i < numOfAgents; i++)
		{
			if (next < backfinalPaths.at(i).size())
			{
				float oop = matchingPos->at(backfinalPaths.at(i).at(next)).x - matchingPos->at(backfinalPaths.at(i).at(last)).x;	// Distance to the next node from the last is the amount to move
				float oop2 = matchingPos->at(backfinalPaths.at(i).at(next)).y - matchingPos->at(backfinalPaths.at(i).at(last)).y;
				cubes->at(cubes->size() - ((numOfAgents + 1) - i))->updatePos(oop, oop2);	// Last cubes added to the array are the agent cubes, so start at the end

				// Update the memory
				updateBufferMemory(*cubes->at(cubes->size() - ((numOfAgents + 1) - i)), vertexBuffers.at(cubes->size() - ((numOfAgents + 1) - i)), vertexBufferMemorys.at(cubes->size() - ((numOfAgents + 1) - i)));
			}
		}
		last = next;		// manages the index of the paths to use
		next++;
	}
	wait += speed;
	

	// Update the UBO
	updateUniformBuffer(imageIndex);



	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::SetNextWindowSize(ImVec2(1000, 1000), ImGuiCond_FirstUseEver);
	ImGui::NewFrame();
	static bool what = true;
	//ImGui::ShowDemoWindow(&what);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("GPU Accelerated A* Pathfinding");    
		// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

		ImGui::Text("General Controls:");
		ImGui::Text("Pathfinding: Start: Space, Reset: R");
		ImGui::Text("Camera Movement: W,A,S,D");
		ImGui::Text("Camrera Zoom: Q,E");
		ImGui::Text("Camera Tilt: F,C");

		ImGui::Text("Pathfind:");            
		ImGui::SameLine();
		ImGui::Checkbox("", &pathFind);     

		ImGui::Text("Pathfind execution time: %.5f seconds", timeTaken);

		ImGui::Text("Change the Pathfinding speed:");             
		ImGui::SliderFloat("Pathfinding Speed", &speed, 0.0f, 100.0f);         
		//ImGui::ColorEdit3("Clear color", (float*)&clear_color); // Edit 3 floats representing a color

		ImGui::Text("Zoom:");      
		ImGui::SliderFloat("Zoom amount", &zoom, 10.0f, 2000.0f);            
		ImGui::Text("FPS:");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	const VkClearValue clearColor = { 1.0f, 0.0f, 0.5f, 1.0f };
	ImGui::Render();
	{
		vkResetCommandPool(device, guiPools, 0);
		VkCommandBufferBeginInfo info2 = {};
		info2.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info2.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(guiCMDbuffers[imageIndex], &info2);

		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = imGuiRenderPass;
		info.framebuffer = guiFrameBuffers[imageIndex];
		info.renderArea.extent.width = swapChainExtent.width;
		info.renderArea.extent.height = swapChainExtent.height;
		info.clearValueCount = 1;
		info.pClearValues = &clearColor;
		vkCmdBeginRenderPass(guiCMDbuffers[imageIndex], &info, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), guiCMDbuffers[imageIndex]);

		// Submit command buffer
		vkCmdEndRenderPass(guiCMDbuffers[imageIndex]);
		vkEndCommandBuffer(guiCMDbuffers[imageIndex]);
	}



	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	std::array<VkCommandBuffer, 2> submitCommandBuffers =
	{ commandBuffers[imageIndex], guiCMDbuffers[imageIndex] };

	// Create a semaphore to wait for an image to be available
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };	
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
	submitInfo.pCommandBuffers = submitCommandBuffers.data();

	// Semaphore to signal the end of rendering
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// reset fences for next frame
	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	// Submit the rendering command buffer
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	//Presenting
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;		// Wait for the end of rendering to present

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;		// Swapchain to draw from
	presentInfo.pImageIndices = &imageIndex;	// Which image to draw, since we know its ready

	// queue the presenting
	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	// If error, recreate swapchain
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	// Set the next frame, looped to max
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	// Wait for the presenting to finish before trying to draw again
	vkQueueWaitIdle(presentQueue);
}

void Render::cleanUp()
{
	cleanupSwapChain();

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	for (int i = 0; i < vertexBufferMemorys.size(); i++)
	{
		vkDestroyBuffer(device, indexBuffers.at(i), nullptr);
		vkFreeMemory(device, indexBufferMemorys.at(i), nullptr);
		vkDestroyBuffer(device, vertexBuffers.at(i), nullptr);
		vkFreeMemory(device, vertexBufferMemorys.at(i), nullptr);
	}
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

}

void Render::createComputePipelineLayout()
{
	// Similar to the graphics pipeline layout
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	
	// 2 sotrage buffers
	for (uint32_t i = 0; i < 2; i++) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = i;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;	// Storage
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;	// Compute
		layoutBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo{};
	setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	setLayoutCreateInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(device, &setLayoutCreateInfo, nullptr, &setLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &setLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayoutCompute)) {
		throw std::runtime_error("failed to create pipeline layout");
	}
}

void Render::createPathsBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position)
{
	// Same as the function below in terms fof functionality
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);	// Check for available queue families are store them

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;	// Creating more than 1 queue
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value() };


	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(Path) * buffer_size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &indices.graphicsFamily.value();

	if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffers) != VK_SUCCESS) {
		throw std::runtime_error("failed to create PAth buffers");
	}
}

void Render::createNodeBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position)
{
	// Needed queues
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);	// Check for available queue families are store them
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;	// Creating more than 1 queue
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value() }; // Grpahics queue also has compute capablitites

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(NodeData) * buffer_size;				// Size of the buffer * the amount of them
	bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;		// Storage
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &indices.graphicsFamily.value();
	if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffers) != VK_SUCCESS) {
		throw std::runtime_error("failed to create Node buffers");
	}
}

void Render::createComputePipeline(const std::string& shaderName)
{
	// Shortened version of the Grpahics pipeline functions
	VkShaderModuleCreateInfo shaderModuleCreateInfo{};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	auto shaderCode = readFile(shaderName);
	shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());
	shaderModuleCreateInfo.codeSize = shaderCode.size();

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}

	VkComputePipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;	// Compute stage
	pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;		// Compute
	pipelineCreateInfo.stage.module = shaderModule;

	pipelineCreateInfo.stage.pName = "main";
	pipelineCreateInfo.layout = pipelineLayoutCompute;		// Sperate layout for this pipeline created before

	if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipelineCompute) != VK_SUCCESS) {// TODO
		throw std::runtime_error("failed to create compute pipeline");
	}
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

void Render::allocateBufferMemoryAndBind(VkBuffer& buffers, VkDeviceMemory& bufferMemory, int t_offset)
{
	VkDeviceSize requiredMemorySize = 0;
	uint32_t typeFilter = 0;

	// Get memory requirements
	VkMemoryRequirements bufferMemoryRequirements;
	vkGetBufferMemoryRequirements(device, buffers, &bufferMemoryRequirements);
	requiredMemorySize += bufferMemoryRequirements.size;
	typeFilter |= bufferMemoryRequirements.memoryTypeBits;

	// Get the type of memory
	uint32_t memoryTypeIndex = findMemoryType(typeFilter, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	// Allocate the memory needed based on the size and type
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = requiredMemorySize;
	allocateInfo.memoryTypeIndex = memoryTypeIndex;
	if (vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}

	VkDeviceSize offset = 0;
	VkMemoryRequirements bufferMemoryRequirements2;
	vkGetBufferMemoryRequirements(device, buffers, &bufferMemoryRequirements2);
	requiredMemorySize += bufferMemoryRequirements2.size;

	if (vkBindBufferMemory(device, buffers, bufferMemory, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to bind buffer memory");
	}
}

void Render::allocateDescriptorSets(const std::vector<VkBuffer> &buffers)
{
	// Same as the other allocate discriptor sets but for the storage buffers
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPoolCompute;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &setLayout;

	if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSetCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	std::vector<VkWriteDescriptorSet> descriptorSetWrites(buffers.size());
	std::vector<VkDescriptorBufferInfo> bufferInfos(buffers.size());

	uint32_t i = 0;
	for (const VkBuffer& buff : buffers) {
		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = descriptorSetCompute;
		writeDescriptorSet.dstBinding = i;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;		// Storage

		VkDescriptorBufferInfo buffInfo{};
		buffInfo.buffer = buff;
		buffInfo.offset = 0;
		buffInfo.range = VK_WHOLE_SIZE;
		bufferInfos[i] = buffInfo;

		writeDescriptorSet.pBufferInfo = &bufferInfos[i];
		descriptorSetWrites[i] = writeDescriptorSet;
		i++;
	}

	vkUpdateDescriptorSets(device, descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);
}

void Render::createComputeCommandPoolAndBuffer()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType =
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &computeCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffer");
	}
}

void Render::addNodes(std::vector<Node*> *t_nodes)
{
	nodes = t_nodes;
}

void Render::setStarts(std::vector<int> t_starts)
{
	starts.clear();
	starts = t_starts;
}

void Render::setMatchingPositions(std::vector<glm::vec2>* t_pos)
{
	matchingPos = t_pos;
}

void Render::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 v{ eye.x, eye.y, zoom };
	ubo.view = glm::lookAt(v, lookAT, glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 5000.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

bool  Render::isDeviceSuitable(VkPhysicalDevice device) {
	
	QueueFamilyIndices indices = findQueueFamilies(device);     // Returns a struct of Queues

	bool extensionsSupported = checkDeviceExtensionSupport(device);		// Check for device level extensions we need are provided

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);							// We need the swap chain for rendering and double/triple buffering
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();	// If both are not empty
	}
	return indices.isComplete() && extensionsSupported && swapChainAdequate; // If Queue families exist, device extensions are supported and Swapchain details are adequate, they we can move on
}

bool Render::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	// Usual query style
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());   // Defined aboce, this would be used if I need special ones for the program, I don't really. All we need is the Swapchain

	// Go through available and delete them from the required
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	// If all are deleted from required then they exist, if not returns false and we got a problem
	return requiredExtensions.empty();
}

QueueFamilyIndices  Render::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); // Takes device, count, container
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)			// From all the Queue families
	{						
		if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)		// Bitwise calculation that results in 1 if correct (1 = true), we need both to be available
		{		
			indices.graphicsFamily = i;								// The list is ordered and stored as ints, if it has this family, assign it index of Graphics bit and Compute queue so we know where it is
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);			// Extension function to find if Presentation is available
		if (presentSupport)
		{
			indices.presentFamily = i;														// Keep track of it's index too, it's likely to be the same
		}

		if (indices.isComplete())					// Is complete if Grpahics, Compute and Present are available
		{		
			break;
		}
		i++;
	}
	return indices;
}

SwapChainSupportDetails  Render::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;	// Details about the Swapchain that ask the GPU what it can do and stores it

	// Min Max of images, num of images
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// Pixel formats, color space - Colour depth
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// Presentation modes, Vsync, buffering etc
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

VkQueue* Render::getGraphicsQueue()
{
	return &graphicsQueue;
}

VkQueue* Render::getPresentQueue()
{
	return &presentQueue;
}

VkSurfaceFormatKHR Render::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) // Of all the formats supported
	{					
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)  	// 32 bit color with SRGB, which is better for screens
		{
			return availableFormat;
		}
	}
	return availableFormats[0];	// Take the first if the one we want isn't there (it should be)
}

VkShaderModule Render::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void Render::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;		// Transfer is it's usage
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);		// Get the memory we need for the buffer

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;						// Size of memeory
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);		// Type of memory, host visible

	// Allocate it
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}
	// Bind the buffer to the allocated memory
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t Render::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);	// Get the memory properties on the device

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) // Bitwise calculation to find the memory
		{
			return i;		// return it if found
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Render::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Copy command needs a command buffer, from a command pool
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;		// Only 1 call to this command

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Copy
	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	// Then execute the command buffer we just declared and destroy it as we wont need it through the program
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Render::recreateSwapchain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);
	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createVulkanRenderPass();
	createVulkanGraphicsPipeline();
	createFrameBuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
}

void Render::cleanupSwapChain()
{
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

VkPresentModeKHR Render::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{			
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)		// Mailbox allows triple buffering and doesn't block images in queue, replaces them
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;		// This will always be there if the present queue is available
}

VkExtent2D Render::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)		// Special case, if exists, you can change the window from default res
	{
		return capabilities.currentExtent;					// If not, take the default
	}
	else
	{
		VkExtent2D actualExtent = { WIDTH, HEIGHT };
		// Make sure it's not more than the max capable, if possible set it to our defined window info
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

