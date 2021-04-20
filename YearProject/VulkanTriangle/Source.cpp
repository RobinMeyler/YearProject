//
//// Author: Robin Meyler
//// Date: 2020/2021
//
//
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/glm.hpp>
//#include <vulkan/vulkan.h>
//#include <cstdio>
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <random>
//#include <array>
//#include <string>
//#include <map>
//
//const std::vector<const char*> validationLayers = {
//        "VK_LAYER_KHRONOS_validation"
//};
//
//#ifdef NDEBUG
//const bool enableValidationLayers = false;
//#else
//const bool enableValidationLayers = true;
//#endif
//
//VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//    VkDebugUtilsMessageTypeFlagsEXT messageType,
//    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//    void* pUserData) {
//
//    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//
//    return VK_FALSE;
//}
//
///// GLOBAL VARIABLES
//VkInstance instance;
//VkDebugUtilsMessengerEXT debugMessenger;
//VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//uint32_t queueFamilyIndex = 0;
//VkDevice device = VK_NULL_HANDLE;
//VkQueue queue;
//VkDescriptorSetLayout setLayout;
//VkPipelineLayout pipelineLayout;
//VkPipeline pipeline;
//VkDescriptorPool descriptorPool;
//VkDescriptorSet descriptorSet;
//VkCommandPool commandPool;
//VkCommandBuffer commandBuffer;
//
//VkDeviceMemory memoryNode;
//VkDeviceMemory memoryPaths;
//
//struct Node 
//{
//    float costToGoal;
//    float totalCostFromStart;
//    float totalCostAccumlative;
//    int marked;		// Has been reached yet or not
//    int passable;
//    int ID;
//    int previousID;		// Previous id for finidng path
//    int arcIDs[4];		// 4 neighbours in grid by ID
//    glm::vec2 position;
//    int padding;        // Aligning the memoryw with the base/ largest data -> Vec2
//};
//
//
//struct NodeData 
//{
//    int start;		// Start ID
//    int goal;		// Goal ID
//    Node nodes[20];
//};
//
//struct Path
//{
//    int pathList[20];
//};
//
//
//bool checkValidationLayerSupport() {
//    uint32_t layerCount;
//    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//    std::vector<VkLayerProperties> availableLayers(layerCount);
//
//    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
//
//    for (const char* layerName : validationLayers) {
//        bool found = false;
//        for (const auto& layerProperties : availableLayers) {
//            if (strcmp(layerName, layerProperties.layerName) == 0) {
//                found = true;
//                break;
//            }
//        }
//
//        if (!found) {
//            return false;
//        }
//    }
//    return true;
//}
//
//std::vector<const char*> getRequiredExtensions() {
//
//    std::vector<const char*> extensions;
//
//    if (enableValidationLayers) {
//        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//    }
//
//    return extensions;
//}
//
//void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
//    createInfo = {};
//    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//    createInfo.messageSeverity =
//        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
//        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//    createInfo.messageType =
//        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
//        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//    createInfo.pfnUserCallback = debugCallback;
//}
//
//void createInstance() {
//    if (enableValidationLayers && !checkValidationLayerSupport()) {
//        throw std::runtime_error("validation layers requested, but not available!");
//    }
//
//    VkApplicationInfo appInfo{};
//    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//    appInfo.pApplicationName = "Astar";
//    appInfo.engineVersion = VK_API_VERSION_1_0;
//    appInfo.pEngineName = "No Engine";
//    appInfo.engineVersion = VK_API_VERSION_1_0;
//    appInfo.apiVersion = VK_API_VERSION_1_0;
//
//    VkInstanceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//    createInfo.pApplicationInfo = &appInfo;
//
//    auto extensions = getRequiredExtensions();
//    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//    createInfo.ppEnabledExtensionNames = extensions.data();
//
//    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
//    if (enableValidationLayers) {
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//
//        populateDebugMessengerCreateInfo(debugCreateInfo);
//        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
//    }
//    else {
//        createInfo.enabledLayerCount = 0;
//
//        createInfo.pNext = nullptr;
//    }
//
//    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create instance!");
//    }
//}
//
//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
//    const VkAllocationCallbacks* pAllocator,
//    VkDebugUtilsMessengerEXT* pDebugMessenger) {
//    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
//        "vkCreateDebugUtilsMessengerEXT");
//    if (func != nullptr) {
//        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//    }
//    else {
//        return VK_ERROR_EXTENSION_NOT_PRESENT;
//    }
//}
//
//void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
//    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//    if (func != nullptr) {
//        func(instance, debugMessenger, pAllocator);
//    }
//}
//
//void setupDebugMessenger() {
//    if (!enableValidationLayers) return;
//    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
//    populateDebugMessengerCreateInfo(createInfo);
//
//    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
//        throw std::runtime_error("failed to set up debug messenger!");
//    }
//}
//
//bool isDeviceSuitable(VkPhysicalDevice device) {
//    uint32_t queueFamilyCount = 0;
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//    int i = 0;
//    for (const auto& queueFamily : queueFamilies) {
//        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
//            queueFamilyIndex = i;
//            return true;
//        }
//        i++;
//    }
//
//    return false;
//}
//
//void pickPhysicalDevice() {
//    uint32_t deviceCount = 0;
//    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//
//    if (deviceCount == 0) {
//        throw std::runtime_error("failed to find GPUs with Vulkan support!");
//    }
//
//    std::vector<VkPhysicalDevice> devices(deviceCount);
//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//
//    for (const auto& device : devices) {
//        if (isDeviceSuitable(device)) {
//            physicalDevice = device;
//            break;
//        }
//    }
//
//    if (physicalDevice == VK_NULL_HANDLE) {
//        throw std::runtime_error("failed to find a suitable GPU!");
//    }
//
//    VkPhysicalDeviceProperties gpuProperties;
//    vkGetPhysicalDeviceProperties(physicalDevice, &gpuProperties);
//
//    std::cout << "Using device: " << gpuProperties.deviceName << '\n';
//}
//
//void createLogicalDeviceAndQueue() {
//    VkDeviceQueueCreateInfo queueCreateInfo{};
//    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//    queueCreateInfo.queueCount = 1;
//    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
//    float queuePriority = 1.0f;
//    queueCreateInfo.pQueuePriorities = &queuePriority;
//
//    VkDeviceCreateInfo deviceCreateInfo{};
//    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//    deviceCreateInfo.queueCreateInfoCount = 1;
//    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
//
//    if (enableValidationLayers) {
//        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
//    }
//    else {
//        deviceCreateInfo.enabledLayerCount = 0;
//    }
//
//    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create logical device!");
//    }
//
//    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
//}
//
//// change here
//void createBindingsAndPipelineLayout(uint32_t bindingsCount) {
//    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
//
//    for (uint32_t i = 0; i < bindingsCount; i++) {
//        VkDescriptorSetLayoutBinding layoutBinding{};
//        layoutBinding.binding = i;
//        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//        layoutBinding.descriptorCount = 1;
//        layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
//        layoutBindings.push_back(layoutBinding);
//    }
//
//    VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo{};
//    setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//    setLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
//    setLayoutCreateInfo.pBindings = layoutBindings.data();
//
//    if (vkCreateDescriptorSetLayout(device, &setLayoutCreateInfo, nullptr, &setLayout) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create descriptor set layout!");
//    }
//
//    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
//    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipelineLayoutCreateInfo.setLayoutCount = 1;
//    pipelineLayoutCreateInfo.pSetLayouts = &setLayout;
//
//    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout)) {
//        throw std::runtime_error("failed to create pipeline layout");
//    }
//}
//
//std::vector<char> readFile(const std::string& filename) {
//    std::ifstream file(filename, std::ios::ate | std::ios::binary);
//
//    if (!file.is_open()) {
//        throw std::runtime_error("failed to open file!");
//    }
//
//    size_t fileSize = (size_t)file.tellg();
//    std::vector<char> buffer(fileSize);
//    file.seekg(0);
//    file.read(buffer.data(), fileSize);
//    file.close();
//
//    return buffer;
//}
//
//void createComputePipeline(const std::string& shaderName) {
//    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
//    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//
//    auto shaderCode = readFile(shaderName);
//    shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());
//    shaderModuleCreateInfo.codeSize = shaderCode.size();
//
//    VkShaderModule shaderModule = VK_NULL_HANDLE;
//    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create shader module");
//    }
//
//    VkComputePipelineCreateInfo pipelineCreateInfo{};
//    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
//    pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    pipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
//    pipelineCreateInfo.stage.module = shaderModule;
//
//    pipelineCreateInfo.stage.pName = "main";
//    pipelineCreateInfo.layout = pipelineLayout;
//
//    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS) {// TODO
//        throw std::runtime_error("failed to create compute pipeline");
//    }
//    vkDestroyShaderModule(device, shaderModule, nullptr);
//}
//
//void createNodeBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position) {
//    VkBufferCreateInfo bufferCreateInfo{};
//    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferCreateInfo.size = sizeof(NodeData) * buffer_size;
//    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
//    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//    bufferCreateInfo.queueFamilyIndexCount = 1;
//    bufferCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
//
//    if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffers) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create buffers");
//    }
//    
//   
//}
//
//void createPathsBuffer(VkBuffer& buffers, uint32_t num_buffers, uint64_t buffer_size, uint64_t position) {
//    VkBufferCreateInfo bufferCreateInfo{};
//    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferCreateInfo.size = sizeof(Path) * buffer_size;
//    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
//    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//    bufferCreateInfo.queueFamilyIndexCount = 1;
//    bufferCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
//
//    if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffers) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create buffers");
//    }
//}
//
//
//uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
//        {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
//
//void allocateBufferMemoryAndBind(VkBuffer &buffers, VkDeviceMemory &bufferMemory, int t_offset) {
//    VkDeviceSize requiredMemorySize = 0;
//    uint32_t typeFilter = 0;
//
//    // Get memory requirements
//    VkMemoryRequirements bufferMemoryRequirements;
//    vkGetBufferMemoryRequirements(device, buffers, &bufferMemoryRequirements);
//    requiredMemorySize += bufferMemoryRequirements.size;
//    typeFilter |= bufferMemoryRequirements.memoryTypeBits;
//    
//    // Get the type of memory
//    uint32_t memoryTypeIndex = findMemoryType(typeFilter, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//    // Allocate the memory needed based on the size and type
//    VkMemoryAllocateInfo allocateInfo = {};
//    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocateInfo.allocationSize = requiredMemorySize;
//    allocateInfo.memoryTypeIndex = memoryTypeIndex;
//    if (vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate buffer memory");
//    }
//
//    VkDeviceSize offset = 0;
//    VkMemoryRequirements bufferMemoryRequirements2;
//    vkGetBufferMemoryRequirements(device, buffers, &bufferMemoryRequirements2);
//    requiredMemorySize += bufferMemoryRequirements2.size;
//
//    if (vkBindBufferMemory(device, buffers, bufferMemory, 0) != VK_SUCCESS) {
//        throw std::runtime_error("failed to bind buffer memory");
//    }   
//}
//
//void allocateDescriptorSets(const std::vector<VkBuffer> buffers) {
//    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
//    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    descriptorPoolCreateInfo.maxSets = 1;
//
//    VkDescriptorPoolSize poolSize{};
//    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//    poolSize.descriptorCount = static_cast<uint32_t>(buffers.size());
//
//    descriptorPoolCreateInfo.poolSizeCount = 1;
//    descriptorPoolCreateInfo.pPoolSizes = &poolSize;
//    if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create descriptor pool");
//    }
//
//    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
//    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
//    descriptorSetAllocateInfo.descriptorSetCount = 1;
//    descriptorSetAllocateInfo.pSetLayouts = &setLayout;
//
//    if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate descriptor sets");
//    }
//
//
//    std::vector<VkWriteDescriptorSet> descriptorSetWrites(buffers.size());
//    std::vector<VkDescriptorBufferInfo> bufferInfos(buffers.size());
//
//    uint32_t i = 0;
//    for (const VkBuffer& buff : buffers) {
//        VkWriteDescriptorSet writeDescriptorSet{};
//        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        writeDescriptorSet.dstSet = descriptorSet;
//        writeDescriptorSet.dstBinding = i;
//        writeDescriptorSet.dstArrayElement = 0;
//        writeDescriptorSet.descriptorCount = 1;
//        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//
//        VkDescriptorBufferInfo buffInfo{};
//        buffInfo.buffer = buff;
//        buffInfo.offset = 0;
//        buffInfo.range = VK_WHOLE_SIZE;
//        bufferInfos[i] = buffInfo;
//
//        writeDescriptorSet.pBufferInfo = &bufferInfos[i];
//        descriptorSetWrites[i] = writeDescriptorSet;
//        i++;
//    }
//
//    vkUpdateDescriptorSets(device, descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);
//}
//
//void createCommandPoolAndBuffer() {
//    VkCommandPoolCreateInfo commandPoolCreateInfo{};
//    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
//    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
//
//    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create command pool");
//    }
//
//    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
//    commandBufferAllocateInfo.sType =
//        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    commandBufferAllocateInfo.commandPool = commandPool;
//    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    commandBufferAllocateInfo.commandBufferCount = 1;
//
//    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate command buffer");
//    }
//}
//
//int main2() {
//    createInstance();
//    setupDebugMessenger();
//    pickPhysicalDevice();
//    createLogicalDeviceAndQueue();
//
//    uint32_t bindingsCount = 2;
//    createBindingsAndPipelineLayout(bindingsCount); // done
//
//    createComputePipeline("shaders/astar.spv"); // done
//
//    const uint32_t elements = 3;
//    VkBuffer buffersNodes;
//    VkBuffer buffersPaths;
//
//    createPathsBuffer(buffersPaths, 1, elements, 0); // done
//    createNodeBuffer(buffersNodes, 1, elements, 1); // done
//
//
//    allocateBufferMemoryAndBind(buffersNodes, memoryNode, 0);
//    allocateBufferMemoryAndBind(buffersPaths, memoryPaths, 1);
//
//    std::vector<VkBuffer> buffers;
//    buffers.push_back(buffersNodes);
//    buffers.push_back(buffersPaths);
//    allocateDescriptorSets(buffers);
//
//    createCommandPoolAndBuffer();
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
//
//    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
//    vkCmdDispatch(commandBuffer, elements, 1, 1);
//
//    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//        throw std::runtime_error("failed to end command buffer");
//    }
//
//    // Node SSBO mapping =======================================
//    NodeData* data = nullptr;
//    if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
//        throw std::runtime_error("failed to map device memory");
//    }
//    Node nodes[20];
//    Node nodes2[20];
//    int xpos = 0;
//    int ypos = 0;
//    for (int i = 0; i < 20; i++)
//    {
//        nodes[i].costToGoal = 1;
//        nodes[i].totalCostFromStart = 0;
//        nodes[i].totalCostAccumlative = 100;
//        nodes[i].marked = 0;
//        nodes[i].passable = 1;
//        nodes[i].ID = i;
//        nodes[i].previousID = -1;
//        nodes2[i].costToGoal = 1;
//        nodes2[i].totalCostFromStart = 0;
//        nodes2[i].totalCostAccumlative = 100;
//        nodes2[i].marked = 0;
//        nodes2[i].passable = 1;
//        nodes2[i].ID = i;
//        nodes2[i].previousID = -1;
//
//        // Left
//        if (i - 1 >= 0 && ((i != 5) && (i != 10) && (i != 15)))
//        {
//            nodes[i].arcIDs[0] = (i - 1);
//            nodes2[i].arcIDs[0] = (i - 1);
//        }
//        else
//        {
//            nodes[i].arcIDs[0] = -1;
//            nodes2[i].arcIDs[0] = -1;
//        }
//        
//        // Right
//        if (i + 1 < 20 && ((i != 4) && (i != 9) && (i != 14)))
//        {
//            nodes[i].arcIDs[1] = (i + 1);
//            nodes2[i].arcIDs[1] = (i + 1);
//        }
//        else
//        {
//            nodes[i].arcIDs[1] = -1;
//            nodes2[i].arcIDs[1] = -1;
//        }
//        
//        // Up
//        if (i - 5 >= 0)
//        {
//            nodes[i].arcIDs[2] = (i - 5);
//            nodes2[i].arcIDs[2] = (i - 5);
//        }
//        else
//        {
//            nodes[i].arcIDs[2] = -1;
//            nodes2[i].arcIDs[2] = -1;
//        }
//
//        // Down
//        if (i + 5 < 20)
//        {
//            nodes[i].arcIDs[3] = (i + 5);
//            nodes2[i].arcIDs[3] = (i + 5);
//        }
//        else
//        {
//            nodes[i].arcIDs[3] = -1;
//            nodes2[i].arcIDs[3] = -1;
//        }
//
//        nodes[i].position.x = xpos;
//        nodes[i].position.y = ypos;
//        nodes2[i].position.x = xpos;
//        nodes2[i].position.y = ypos;
//        xpos++;
//
//        if (i % 5 == 4)
//        {
//            ypos++;
//            xpos = 0;
//        }
//
//    }
//    // 18 is test goal
//    for (int i = 0; i < 20; i++)
//    {
//        nodes[i].costToGoal = abs(nodes[18].position.x - nodes[i].position.x) + abs(nodes[18].position.y - nodes[i].position.y);
//        nodes2[i].costToGoal = abs(nodes2[18].position.x - nodes2[i].position.x) + abs(nodes2[18].position.y - nodes2[i].position.y);
//
//    }
//    NodeData* data1;
//    NodeData* data2;
//    NodeData* data3;
//
//    NodeData* dataR;
//    NodeData* dataR2;
//    NodeData* dataR3;
//
//    data1 = data;
//    data2 = data + 1;
//    data3 = data + 2;
//
//    data1->start = 5;
//    data1->goal = 18;
//    data2->start = 4;
//    data2->goal = 18;
//    data3->start = 10;
//    data3->goal = 18;
//
//
//    std::copy(std::begin(nodes), std::end(nodes), std::begin(data1->nodes));
//    std::copy(std::begin(nodes), std::end(nodes), std::begin(data2->nodes));
//    std::copy(std::begin(nodes), std::end(nodes), std::begin(data3->nodes));
//    vkUnmapMemory(device, memoryNode);
//    // ============================================================
//
//
//    // Paths SSBO mapping =========================================
//    Path* dataPaths = nullptr;
//    if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&dataPaths)) != VK_SUCCESS) {
//        throw std::runtime_error("failed to map device memory");
//    }
//    Path fillerData;
//    Path fillerData2;
//    for (int i = 0; i < 20; i++)
//    {
//        fillerData.pathList[i] = -1;
//        fillerData2.pathList[i] = -1;
//    }
//    Path* populatingPaths;
//    Path* populatingPaths2;
//    Path* populatingPaths3;
//
//    Path* returnPaths;
//    Path* returnPaths2;
//    Path* returnPaths3;
//
//    populatingPaths = dataPaths;
//    populatingPaths2 = dataPaths + 1;
//    populatingPaths3 = dataPaths + 2;
//
//    std::copy(std::begin(fillerData.pathList), std::end(fillerData.pathList), std::begin(populatingPaths->pathList));
//    std::copy(std::begin(fillerData2.pathList), std::end(fillerData2.pathList), std::begin(populatingPaths2->pathList));
//    std::copy(std::begin(fillerData2.pathList), std::end(fillerData2.pathList), std::begin(populatingPaths3->pathList));
//
//    vkUnmapMemory(device, memoryPaths);
//    // =============================================================
//
//    VkSubmitInfo submitInfo = {};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &commandBuffer;
//    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
//    // but we can simply wait for all the work to be done
//    vkQueueWaitIdle(queue);
//
//
//    data = nullptr;
//    if (vkMapMemory(device, memoryNode, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&data)) != VK_SUCCESS) {
//        throw std::runtime_error("failed to map device memory");
//    }
//    dataR = data;
//    dataR2 = data + 1;
//    dataR3 = data + 2;
// 
//    Path* pathsReturned = nullptr;
//    if (vkMapMemory(device, memoryPaths, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&pathsReturned)) != VK_SUCCESS) {
//        throw std::runtime_error("failed to map device memory");
//    }
//    returnPaths = pathsReturned;
//    returnPaths2 = pathsReturned + 1;
//    returnPaths3 = pathsReturned + 2;
//    vkUnmapMemory(device, memoryPaths);
//
//    vkDestroyCommandPool(device, commandPool, nullptr);
//    vkFreeMemory(device, memoryNode, nullptr);
//    vkFreeMemory(device, memoryPaths, nullptr);
//    vkDestroyBuffer(device, buffersNodes, nullptr);
//    vkDestroyBuffer(device, buffersPaths, nullptr);
//
//    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
//    vkDestroyPipeline(device, pipeline, nullptr);
//    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
//    vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
//    vkDestroyDevice(device, nullptr);
//
//    if (enableValidationLayers) {
//        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
//    }
//
//    vkDestroyInstance(instance, nullptr);
//
//    system("PAUSE");
//    return 0;
//}