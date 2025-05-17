/*
** Following Hamdy Elzanqali's tutorial.
*/

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *window;
SDL_GPUDevice *device;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  window = SDL_CreateWindow("Hello, Triangle", 960, 540, SDL_WINDOW_RESIZABLE);

  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
  SDL_ClaimWindowForGPUDevice(device, window);

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

  // draw something

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
  SDL_DestroyGPUDevice(device);
  SDL_DestroyWindow(window);
}
