#pragma once

#include "device.h"

#include <string>
#include <vector>

namespace fengine {
	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class FPipeline {
		public:
			FPipeline(
				Device& device, 
				const PipelineConfigInfo& configInfo, 
				const std::string& vertFilePath, 
				const std::string& fragFilePath);
			~FPipeline();

			FPipeline(const FPipeline&) = delete;
			FPipeline& operator= (const FPipeline&) = delete;

			void bind(VkCommandBuffer commandBuffer);

			static void defaultPipeLineConfigInfo(PipelineConfigInfo& configInfo);

		private:
			static std::vector<char> readFile(const std::string& filePath);

			void createGraphicsPipeline(
				const PipelineConfigInfo& configInfo, 
				const std::string& vertFilePath, 
				const std::string& fragFilePath);

			void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

			Device& m_device;
			VkPipeline m_graphicsPipeline;
			VkShaderModule m_vertShaderModule;
			VkShaderModule m_fragShaderModule;
	};
}