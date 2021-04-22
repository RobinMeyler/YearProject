#include "Render.h"


Render::Render()
{
}

void Render::setupVulkan(GLFWwindow* t_window)
{
	createVulkanInstance();
	creatVulkanSurface(t_window);
	chooseGPUDevice();
	createVulkanLogicalDevice();
	createSwapChain();
	createImageViews();
	createVulkanRenderPass();
	createDescriptorSetLayout();
	createVulkanGraphicsPipeline();

	createComputePipelineLayout();				// New
	createComputePipeline("shaders/astar.spv");	// new

	createFrameBuffers();
	createCommandPool();

	createNodeBuffer(buffersNodes, 1, 3, 1);	// new
	createPathsBuffer(buffersPaths, 1, 3, 0);	 // new

	for (int i = 0; i < cubes->size(); i++)
	{
		createVertexBuffer(*cubes->at(i), vertexBuffers.at(i), vertexBufferMemorys.at(i));
		createIndexBuffer(*cubes->at(i), indexBuffers.at(i), indexBufferMemorys.at(i));
	}

	allocateBufferMemoryAndBind(buffersNodes, memoryNode, 0);	// new
	allocateBufferMemoryAndBind(buffersPaths, memoryPaths, 1);	// new

	createUniformBuffers();
	createDescriptorPool();
	
	m_buffers.push_back(buffersNodes);
	m_buffers.push_back(buffersPaths);

	createDescriptorSets();
	allocateDescriptorSets(m_buffers);
	createComputeCommandPoolAndBuffer();						// new
	createCommandBuffers();		// Different draw command groups set
	createSyncObjects();		// For controlling the ordering of commands and draws ( Controls concurent actions 
}

void Render::createVulkanInstance()
{
	// To allow Vulkan to be generic a lot of the functionalisty is abstracted away into extentions
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

	std::cout << "Number of Devices" << devices.size() << std::endl;
	for (const auto& device : devices)
	{        // Of our GPUs, find one that works
		if (isDeviceSuitable(device))
		{         // If we find one or more, use it (this picks which one too)
			physicalDevice = device;
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
	// Queue families are just logical groupings of queues e.g graphics Qs, presentation Qs
		//  Here we check our GPU for what it supports and sotres the info
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);	// Check for available queue families are store them

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;	// Creating more than 1 queue
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;		// Priority of the Queue used for ordering, 0.0 -1.0 scale
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;					// Can have more than 1 but 1 is simpiler for now
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	VkPhysicalDeviceFeatures deviceFeatures = {};		// Features on device, none atm

	VkDeviceCreateInfo createInfo = {};					// Logical device
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();		// Our created queues
	createInfo.pEnabledFeatures = &deviceFeatures;				// Device features (none)
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // How many extentsions
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();	// The extensions

	

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

	//Chose the ones we want from what is available
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//If we use the minimum, then we may have to wait for the driver to make another image, so 1 extra
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	// Max count of 0 is a special value meaing there is no max
	// Overwrite minimum
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;	// Link to window

	createInfo.minImageCount = imageCount;		// Minimum images it should have
	createInfo.imageFormat = surfaceFormat.format;		// Pixel detail info
	createInfo.imageColorSpace = surfaceFormat.colorSpace;	// Type (non linear)
	createInfo.imageExtent = extent;			// Window Size and res info
	createInfo.imageArrayLayers = 1;			// Layers each image has, 1 unless Stereoscopic games
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	// If we wanted post processing, we would chnage this to transfer

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// Determine if the Graphics and Presentation families are the same or not
	if (indices.graphicsFamily != indices.presentFamily)
	{		// If not
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;		// how many
		createInfo.pQueueFamilyIndices = queueFamilyIndices;	// The data set
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	// Such as rotating the image 90 degrees, here we do nothing
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// aplha channel for blending
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;	 // Mailbox
	createInfo.clipped = VK_TRUE;			// Use clipping

	createInfo.oldSwapchain = VK_NULL_HANDLE;	// IF the chain becomes invalid ( we do nothing rn)

	// Store these for use and checkls later
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	// Create the bich
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	// Get some images and store them for operations
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}

void Render::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;  // 1D, 2D, 3D, cube maps
		createInfo.format = swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // Can map all to Red if you want, or set value 0-1
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;	// This is default
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// What images purpose and which part of the image should be accessed
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;	// Color target
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
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

// change here
void Render::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

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
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = Vertex2::getBindingDescription();
	auto attributeDescriptions = Vertex2::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 0.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkDynamicState dynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional

	pipelineInfo.layout = pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void Render::createFrameBuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Render::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void Render::creatBufferObjects(int t_count)
{
	for (int i = 0; i < t_count; i++)
	{
		VkBuffer buffer, index;
		vertexBuffers.push_back(buffer);
		indexBuffers.push_back(index);

		VkDeviceMemory bufferMem, indexMem;
		vertexBufferMemorys.push_back(bufferMem);
		indexBufferMemorys.push_back(indexMem);
	}
}

void Render::addVBOs(std::vector<Cube*>* t_cubes)
{
	cubes = t_cubes;
}

void Render::createVertexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
	VkDeviceSize bufferSize = sizeof(t_cube.vertices[0]) * t_cube.vertices.size();

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, t_cube.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, t_vertexbuffer, t_vertexbuffermemory);
	copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
}

void Render::updateBufferMemory(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
	VkDeviceSize bufferSize = sizeof(t_cube.vertices[0]) * t_cube.vertices.size();

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, t_cube.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	copyBuffer(stagingBuffer, t_vertexbuffer, bufferSize);
}

void Render::createIndexBuffer(Cube& t_cube, VkBuffer& t_vertexbuffer, VkDeviceMemory& t_vertexbuffermemory)
{
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
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void Render::createDescriptorPool()
{
	VkDescriptorPoolSize poolSizeGraphics{};
	poolSizeGraphics.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizeGraphics.descriptorCount = static_cast<uint32_t>(swapChainImages.size());

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
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(2);

	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &poolSize;
	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPoolCompute) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}

}

void Render::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainImages.size());
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Render::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		for (int j = 0; j < cubes->size(); j++)
		{
			VkBuffer vertexBuffersObs[] = { vertexBuffers.at(j) };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffersObs, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffers.at(j), 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>((*cubes)[j]->indices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(computeCommandBuffer, &beginInfo);
		vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineCompute);
		vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayoutCompute, 0, 1, &descriptorSetCompute, 0, nullptr);
		vkCmdDispatch(computeCommandBuffer, 3, 1, 1);
	if (vkEndCommandBuffer(computeCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to end command buffer");
	}
	// Node SSBO mapping =======================================
	NodeData* data = nullptr;
	if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}
	/*Node nodesTemp[5625];
	for (int i = 0; i < 625; i++)
	{
		nodesTemp[i] = *nodes->at(i);
	}*/
	std::vector<Node> temp;

	for (auto& nod : *nodes)
	{
		temp.push_back(*nod);
	}

	NodeData* data1;
	NodeData* data2;
	NodeData* data3;

	data1 = data;
	data2 = data + 1;
	data3 = data + 2;

	data1->start = 341;
	data1->goal = 4066;

	data2->start = 1536;
	data2->goal = 4066;

	data3->start = 129;
	data3->goal = 4066;

	std::copy(std::begin(temp), std::end(temp), std::begin(data1->nodes));
	std::copy(std::begin(temp), std::end(temp), std::begin(data2->nodes));
	std::copy(std::begin(temp), std::end(temp), std::begin(data3->nodes));
	vkUnmapMemory(device, memoryNode);
	// ============================================================

	// Paths SSBO mapping =========================================
	Path* dataPaths = nullptr;
	if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&dataPaths)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}
	Path fillerData;
	for (int i = 0; i < 625; i++)
	{
		fillerData.pathList[i] = -1;
	}
	Path* populatingPaths;
	Path* populatingPaths2;
	Path* populatingPaths3;

	populatingPaths = dataPaths;
	populatingPaths2 = dataPaths + 1;
	populatingPaths3 = dataPaths + 2;

	std::copy(std::begin(fillerData.pathList), std::end(fillerData.pathList), std::begin(populatingPaths->pathList));
	std::copy(std::begin(fillerData.pathList), std::end(fillerData.pathList), std::begin(populatingPaths2->pathList));
	std::copy(std::begin(fillerData.pathList), std::end(fillerData.pathList), std::begin(populatingPaths3->pathList));

	vkUnmapMemory(device, memoryPaths);

	VkSubmitInfo submitInfo2 = {};
	submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo2.commandBufferCount = 1;
	submitInfo2.pCommandBuffers = &computeCommandBuffer;
	vkQueueSubmit(graphicsQueue, 1, &submitInfo2, VK_NULL_HANDLE);
	// but we can simply wait for all the work to be done
	vkQueueWaitIdle(graphicsQueue);

	data = nullptr;
	if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}
	dataR = data;
	dataR2 = data + 1;
	dataR3 = data + 2;
	vkUnmapMemory(device, memoryNode);

	Path* pathsReturned = nullptr;
	if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&pathsReturned)) != VK_SUCCESS) {
		throw std::runtime_error("failed to map device memory");
	}
	returnPaths = pathsReturned;
	returnPaths2 = pathsReturned + 1;
	returnPaths3 = pathsReturned + 2;
	vkUnmapMemory(device, memoryPaths);

	finalPath.push_back(4066);
	for (int i = 0; i < 625; i++)
	{
		if (returnPaths->pathList[i] != -1)
		{
			finalPath.push_back(returnPaths->pathList[i]);
		}
		else
		{
			break;
		}
	}
	std::reverse(finalPath.begin(), finalPath.end());

	finalPath2.push_back(4066);
	for (int i = 0; i < 625; i++)
	{
		if (returnPaths2->pathList[i] != -1)
		{
			finalPath2.push_back(returnPaths2->pathList[i]);
		}
		else
		{
			break;
		}
	}
	std::reverse(finalPath2.begin(), finalPath2.end());

	finalPath3.push_back(4066);
	for (int i = 0; i < 625; i++)
	{
		if (returnPaths3->pathList[i] != -1)
		{
			finalPath3.push_back(returnPaths3->pathList[i]);
		}
		else
		{
			break;
		}
	}
	std::reverse(finalPath3.begin(), finalPath3.end());
}

void Render::createSyncObjects()
{
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

	//VkSubmitInfo submitInfo2 = {};
	//submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//submitInfo2.commandBufferCount = 1;
	//submitInfo2.pCommandBuffers = &computeCommandBuffer;
	//vkQueueSubmit(graphicsQueue, 1, &submitInfo2, VK_NULL_HANDLE);
	//// but we can simply wait for all the work to be done
	//vkQueueWaitIdle(graphicsQueue);

	//NodeData* data = nullptr;
	//if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to map device memory");
	//}
	//dataR = data;
	//dataR2 = data + 1;
	//dataR3 = data + 2;
	//vkUnmapMemory(device, memoryNode);

	//Path* pathsReturned = nullptr;
	//if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&pathsReturned)) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to map device memory");
	//}
	//returnPaths = pathsReturned;
	//returnPaths2 = pathsReturned + 1;
	//returnPaths3 = pathsReturned + 2;
	//vkUnmapMemory(device, memoryPaths);





	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	// Update Start Block
	if (wait > 501 && next < finalPath.size())
	{
		float oop = nodes->at(finalPath.at(next))->position.x - nodes->at(finalPath.at(last))->position.x;
		float oop2 = nodes->at(finalPath.at(next))->position.y - nodes->at(finalPath.at(last))->position.y;
		cubes->at(cubes->size() - 4)->updatePos(oop, oop2);
		updateBufferMemory(*cubes->at(cubes->size() - 4), vertexBuffers.at(cubes->size() - 4), vertexBufferMemorys.at(cubes->size() - 4));
	}
	// Update Start Block
	if (wait > 501 && next < finalPath2.size())
	{
		float oop = nodes->at(finalPath2.at(next))->position.x - nodes->at(finalPath2.at(last))->position.x;
		float oop2 = nodes->at(finalPath2.at(next))->position.y - nodes->at(finalPath2.at(last))->position.y;
		cubes->at(cubes->size() - 3)->updatePos(oop, oop2);
		updateBufferMemory(*cubes->at(cubes->size() - 3), vertexBuffers.at(cubes->size() - 3), vertexBufferMemorys.at(cubes->size() - 3));
	}
	// Update Start Block
	if (wait > 501 && next < finalPath3.size())
	{
		wait = 0;
		float oop = nodes->at(finalPath3.at(next))->position.x - nodes->at(finalPath3.at(last))->position.x;
		float oop2 = nodes->at(finalPath3.at(next))->position.y - nodes->at(finalPath3.at(last))->position.y;
		cubes->at(cubes->size() - 2)->updatePos(oop, oop2);
		updateBufferMemory(*cubes->at(cubes->size() - 2), vertexBuffers.at(cubes->size() - 2), vertexBufferMemorys.at(cubes->size() - 2));
		last = next;
		next++;
	}
	wait++;
	std::cout << wait << std::endl;
	updateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	for (uint32_t i = 0; i < 2; i++) {
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = i;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
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
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);	// Check for available queue families are store them

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;	// Creating more than 1 queue
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value() };


	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(NodeData) * buffer_size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &indices.graphicsFamily.value();

	if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffers) != VK_SUCCESS) {
		throw std::runtime_error("failed to create Node buffers");
	}
}

void Render::createComputePipeline(const std::string& shaderName)
{
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
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	pipelineCreateInfo.stage.module = shaderModule;

	pipelineCreateInfo.stage.pName = "main";
	pipelineCreateInfo.layout = pipelineLayoutCompute;

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
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

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

void Render::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(75.0f, 75.0f, 200.0f), glm::vec3(75.0f, 75.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

bool  Render::isDeviceSuitable(VkPhysicalDevice device) {
	
	QueueFamilyIndices indices = findQueueFamilies(device);     // Returns a struct of Queues

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty(); //If both are not empty
	}
	return indices.isComplete() && extensionsSupported && swapChainAdequate; // If Queue families exist, device extensions are supported and Swapchain details are adequate, they we can move on
}

bool  Render::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Outputting then to console
	std::cout << "available device extensions:" << std::endl;
	for (const auto& extension : availableExtensions) {
		std::cout << "\t" << extension.extensionName << std::endl; // \t = tab
	}

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

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
	auto result = queueFamilies.at(0).queueFlags & VK_QUEUE_GRAPHICS_BIT;
	for (const auto& queueFamily : queueFamilies)
	{						// From all the Queue families
		std::cout << "QueueFamily: " << i << std::endl;
		std::cout << "Flag " << (queueFamily.queueFlags) << std::endl;
		std::cout << "Graphics " << ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
		std::cout << "Compute " << ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
		std::cout << "Transfer " << ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
		std::cout << "Protected " << ((queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) << std::endl;
		std::cout << "Sparse " << ((queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
		std::cout << "count: " << queueFamily.queueCount << std::endl;
		std::cout << "" << std::endl;

		if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)		// Bitwise calculation that results in 1 if correct (1 = true)
		{			// If it has Queue graphics bit, one specific family grouping 
			indices.graphicsFamily = i;			// The list is ordered and stored as ints, if it has this family, assign it index of Graphics bit so we know where it is
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{		// WE only need 1 so if sound, break;
			break;
		}
		i++;
	}
	return indices;
}

SwapChainSupportDetails  Render::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details; // Details about the Swapchain that ask the GPU what it can do and stores it

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
	for (const auto& availableFormat : availableFormats)
	{						// 32 bit color with SRGB (look this up again)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t Render::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Render::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

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
	{			// Mailbox allows triple buffering and doenst block images in queue, replaces them
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;		// This will always be there
}

VkExtent2D Render::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) // Special case, if exists, you can change the window from default res
	{
		return capabilities.currentExtent;	// If not, take the default
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

