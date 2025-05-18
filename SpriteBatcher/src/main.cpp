#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *window;
SDL_GPUDevice *device;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  window = SDL_CreateWindow("SpriteBatcher", 960, 540, SDL_WINDOW_RESIZABLE);
  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

  SDL_ClaimWindowForGPUDevice(device, window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUTexture *swapchainTexture;

  Uint32 width, height;

  SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window,
                                        &swapchainTexture, &width, &height);

  if (swapchainTexture == NULL) {
    // swapchain texture has not finished updating. It's overloaded for example.
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return SDL_APP_CONTINUE;
  }

  // Color target - where gpu draws
  SDL_GPUColorTargetInfo colorTargetInfo{};
  colorTargetInfo.clear_color = {60 / 255.0f, 60 / 255.0f, 60 / 255.0f,
                                 255 / 255.0f};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapchainTexture;

  SDL_GPURenderPass *renderPass =
      SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

  SDL_EndGPURenderPass(renderPass);

  SDL_SubmitGPUCommandBuffer(commandBuffer);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
    return SDL_APP_SUCCESS;
  };
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
