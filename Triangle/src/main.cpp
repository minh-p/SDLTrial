/*
** Following Hamdy Elzanqali's tutorial.
*/

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Creating the Triangle
// First, we're creating the vertex buffer.

// "Vertex input layout"
// Cool way C syntax works. As a programmer, you can organize it like this.
// The struct Vertex only houses float values.
struct Vertex {
  float x, y, z;    // vec3 position
  float r, g, b, a; // vec4 color
};

// Initialized only once "static" keyword.
// As you can see, the vertices' positions are in normalized device
// coordinates.
static Vertex vertices[]{
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // top vertex
    {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // bottom left vertex
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}   // bottom right vertex
};

SDL_Window *window;
SDL_GPUDevice *device;
SDL_GPUBuffer *vertexBuffer;
SDL_GPUTransferBuffer *transferBuffer;
SDL_GPUGraphicsPipeline *graphicsPipeline;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  window = SDL_CreateWindow("Hello, Triangle", 960, 540, SDL_WINDOW_RESIZABLE);

  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
  SDL_ClaimWindowForGPUDevice(device, window);

  // It's time to then send this data to a GPU buffer. It should not be mistaken
  // for the command buffer, which is the total buffer storage desgination.
  SDL_GPUBufferCreateInfo bufferInfo{};
  // Memory management in C be like.
  bufferInfo.size = sizeof(vertices);
  // Memory bufferes are temporary storage areas within a computer's memory.
  // A vertex buffer is a memory buffer that contain vertex data.
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

  // Creating buffers is an expensive operation. They should be only created
  // earlier in the app. Buffers also should be reused instead of creating them
  // every frame.
  vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

  // Now then, this vertexBuffer is a gpu buffer. But then we actually get the
  // information from the CPU.
  // And that's why, a gpu transfer buffer is needed.

  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = sizeof(vertices);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

  Vertex *data =
      (Vertex *)SDL_MapGPUTransferBuffer(device, transferBuffer, false);

  // Copying every vertex to data. e.g. data[0] = vertices[0]; data[1] =
  // vertices[1]...
  SDL_memcpy(data, vertices, sizeof(vertices));

  // Unmapping as we're donig updating the transfer buffer
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  // Shaders
  size_t vertexCodeSize;
  void *vertexCode = SDL_LoadFile("shaders/vertex.vert.spv", &vertexCodeSize);
  if (!vertexCode) {
    SDL_Log("Failed to load vertex shader.");
    return SDL_APP_FAILURE;
  }

  // Vertex Shader
  SDL_GPUShaderCreateInfo vertexInfo{};
  vertexInfo.code = (Uint8 *)vertexCode; // Converting to an array of bytes
  vertexInfo.code_size = vertexCodeSize;
  // entrypoint is the name of the function defined in the shader.
  vertexInfo.entrypoint = "main";
  vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  vertexInfo.num_samplers = 0;
  vertexInfo.num_storage_buffers = 0;
  vertexInfo.num_storage_textures = 0;
  vertexInfo.num_uniform_buffers = 0;
  SDL_GPUShader *vertexShader = SDL_CreateGPUShader(device, &vertexInfo);

  // Then free the shader file
  SDL_free(vertexCode);

  // create the fragment shader
  size_t fragmentCodeSize;
  void *fragmentCode =
      SDL_LoadFile("shaders/fragment.frag.spv", &fragmentCodeSize);

  if (!fragmentCode) {
    SDL_Log("Failed to load fragment shader.");
    return SDL_APP_FAILURE;
  }

  // create the fragment shader
  SDL_GPUShaderCreateInfo fragmentInfo{};
  fragmentInfo.code = (Uint8 *)fragmentCode;
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = "main";
  fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; // fragment shader
  fragmentInfo.num_samplers = 0;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_storage_textures = 0;
  fragmentInfo.num_uniform_buffers = 0;

  SDL_GPUShader *fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

  // free the file
  SDL_free(fragmentCode);

  // Graphics Pipeline
  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

  // bind shaders
  pipelineInfo.vertex_shader = vertexShader;
  pipelineInfo.fragment_shader = fragmentShader;

  // draw triangles
  // Primitive types tell the GPU what vertices mean. Right now, we're drawing
  // triangles.
  // Try changing it to draw points or lines later.
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  // Buffers are we expecting

  SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
  // assigned slot 0 will be used later when binding the buffer just before the
  // draw call.
  vertexBufferDescriptions[0].slot = 0;
  // input rate set to be per vertex and not per instance.
  // select data from the buffer will be changed to the next on every vertex.
  // e.g. first vertex gets vertices[0], second get vertices[0].
  vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDescriptions[0].instance_step_rate = 0;
  // pitch is how many bytes to jump after each cycle.
  vertexBufferDescriptions[0].pitch = sizeof(Vertex);

  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_buffer_descriptions =
      vertexBufferDescriptions;

  // Describing the vertex attribute

  SDL_GPUVertexAttribute vertexAttributes[2];

  // a_position
  vertexAttributes[0].buffer_slot = 0; // fetch data from the buffer at slot 0
  vertexAttributes[0].location = 0;    // layout (location = 0) in shader
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; // vec3
  vertexAttributes[0].offset =
      0; // start from the first byte from current buffer position

  // a_color
  vertexAttributes[1].buffer_slot = 0; // use buffer at slot 0
  vertexAttributes[1].location = 1;    // layout (location = 1) in shader
  vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; // vec4
  vertexAttributes[1].offset =
      sizeof(float) * 3; // 4th float from current buffer position

  pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

  // Now, what's the color target that this pipeline is created to work on?
  // describe the color target
  SDL_GPUColorTargetDescription colorTargetDescriptions[1];
  colorTargetDescriptions[0] = {};
  colorTargetDescriptions[0].format =
      SDL_GetGPUSwapchainTextureFormat(device, window);

  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

  // Officially creating the GPU graphics pipeline.
  graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

  // Releasing the shaders as we don't need the manymore.
  SDL_ReleaseGPUShader(device, vertexShader);
  SDL_ReleaseGPUShader(device, fragmentShader);

  // Updating the vertex buffer after creating the transfer buffer.
  // Transferring data from the transfer buffer to the vertex buffer is a
  // process known as a Copy pass.

  // The reason why we have a copy pass in the initialization portion of the
  // application is because the triangle won't change.

  // Initiating a copy pass
  SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

  // Source of the data.
  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  // Starting byte of the buffer data.
  // This gets really low level. Basically, we're starting from the beginning to
  // include the entirety of the transfer buffer.
  location.offset = 0;

  // Data destination
  SDL_GPUBufferRegion region{};
  region.buffer = vertexBuffer;
  region.size = sizeof(vertices);
  region.offset = 0;

  // uploading the data
  SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  // Ending copypass
  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);

  SDL_GPUTexture *swapchainTexture;
  Uint32 width, height;
  // The command buffer is the storage for all the commands, while the swapchain
  // is also a buffer.
  // A swap chain is a collection of buffers that are used for displaying frames
  // to the user.
  SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window,
                                        &swapchainTexture, &width, &height);

  if (swapchainTexture == NULL) {
    // end the frame early if a swapchain texture is not available
    // SDL_SubmitGPUCommandBuffer has to be called at the end of every frame.
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return SDL_APP_CONTINUE;
  }

  // The argument colorTargetInfo tells the GPU where to draw, what to do with
  // previous target's content, and how to deal with new data.

  // create the color target
  SDL_GPUColorTargetInfo colorTargetInfo{};
  // Still learning C. 255.0f is basically 255 in float. These arguments will be
  // from 0 to 1.
  colorTargetInfo.clear_color = {240 / 255.0f, 240 / 255.0f, 240 / 255.0f,
                                 255 / 255.0f};
  // What to do with previous target's content.
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  // We're going to store the content to the texture.
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  // The texture aforementioned.
  colorTargetInfo.texture = swapchainTexture;

  // begin a render pass
  SDL_GPURenderPass *renderPass =
      SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

  // Draw something

  // First, we are binding the pipeline.
  SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

  // Bind the vertex buffer
  SDL_GPUBufferBinding bufferBindings[1];
  // Know that index 0 is slot 0.
  bufferBindings[0].buffer = vertexBuffer;
  bufferBindings[0].offset = 0;

  // Binding one buffer starting from slot 0.
  SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

  // Finally, we're drawing the triangle!
  SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

  // end the render pass
  SDL_EndGPURenderPass(renderPass);

  // submit the command buffer
  SDL_SubmitGPUCommandBuffer(commandBuffer);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  SDL_ReleaseGPUBuffer(device, vertexBuffer);
  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
  SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
  SDL_DestroyGPUDevice(device);
  SDL_DestroyWindow(window);
}
