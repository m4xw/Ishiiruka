
#pragma once

#include <libretro.h>
//#include "VideoBackends/Null/Render.h"
#include "VideoBackends/Software/SWRenderer.h"
#include "VideoBackends/Software/SWTexture.h"
#include "VideoCommon/VideoConfig.h"
#include "Core/HW/Memmap.h"
#ifndef __APPLE__
#include "VideoBackends/Vulkan/VulkanLoader.h"
#endif
#ifdef _WIN32
#include "VideoBackends/DX11/D3DBase.h"
#include "VideoBackends/DX11/D3DState.h"
#include "VideoBackends/DX11/D3DUtil.h"
#include "VideoBackends/DX11/D3DShader.h"
#include "VideoBackends/DX11/DXTexture.h"
#include "VideoBackends/DX11/FramebufferManager.h"
#include "VideoBackends/DX11/PixelShaderCache.h"
#include "VideoBackends/DX11/Render.h"
#include "VideoBackends/DX11/TextureCache.h"
#endif

namespace Libretro
{
namespace Video
{
void Init(void);
extern retro_video_refresh_t video_cb;
extern struct retro_hw_render_callback hw_render;

#ifndef __APPLE__
namespace Vk
{
void Init(VkInstance instance, VkPhysicalDevice gpu, VkSurfaceKHR surface,
          PFN_vkGetInstanceProcAddr get_instance_proc_addr, const char** required_device_extensions,
          unsigned num_required_device_extensions, const char** required_device_layers,
          unsigned num_required_device_layers, const VkPhysicalDeviceFeatures* required_features);
void SetSurfaceSize(uint32_t width, uint32_t height);
void SetHWRenderInterface(retro_hw_render_interface* hw_render_interface);
void Shutdown();
void WaitForPresentation();
}  // namespace Vk
#endif

class SWRenderer : public ::SWRenderer
{
public:
  void SwapImpl(u32 xfbAddr, u32 fbWidth, u32 fbStride, u32 fbHeight, const EFBRectangle& rc, u64 ticks, float Gamma)
  {
     if (g_ActiveConfig.bUseXFB)
     {
        EfbInterface::yuv422_packed* xfb = (EfbInterface::yuv422_packed*) Memory::GetPointer(xfbAddr);
        UpdateColorTexture(xfb, fbWidth, fbHeight);
     }
     else
     {
        EfbInterface::BypassXFB(GetCurrentColorTexture(), fbWidth, fbHeight, rc, Gamma);
     }

     video_cb(GetCurrentColorTexture(), fbWidth, fbHeight, fbWidth * 4);

     UpdateActiveConfig();
  }
};

#if 0
class NullRenderer : public Null::Renderer
{
public:
  void SwapImpl(u32 xfbAddr, u32 fbWidth, u32 fbStride, u32 fbHeight, const EFBRectangle& rc, u64 ticks, float Gamma)
  {
    video_cb(NULL, 512, 512, 512 * 4);
    UpdateActiveConfig();
  }
};
#endif
#ifdef _WIN32
class DX11Renderer : public DX11::Renderer
{
public:
   /* TODO/FIXME */
#if 0
  DX11Renderer() : DX11::Renderer(1,1) {}
#endif
  void SwapImpl(u32 xfbAddr, u32 fbWidth, u32 fbStride, u32 fbHeight, const EFBRectangle& rc, u64 ticks, float Gamma)
  {
     /* TODO/FIXME */
#if 0
    DX11::D3DTexture2D* xfb_texture = static_cast<DX11::DXTexture*>(texture)->GetRawTexIdentifier();

    ID3D11RenderTargetView* nullView = nullptr;
    DX11::D3D::context->OMSetRenderTargets(1, &nullView, nullptr);
    DX11::D3D::context->PSSetShaderResources(0, 1, &xfb_texture->GetSRV());
    Libretro::Video::video_cb(RETRO_HW_FRAME_BUFFER_VALID, rc.GetWidth(), rc.GetHeight(), 0);

    ResetAPIState();
    g_texture_cache->Cleanup(frameCount);

    // Enable configuration changes
    UpdateActiveConfig();
    g_texture_cache->OnConfigChanged(g_ActiveConfig);

    // Resize the back buffers NOW to avoid flickering
    if (CalculateTargetSize())
    {
      UpdateDrawRectangle();
      g_framebuffer_manager.reset();
      g_framebuffer_manager =
          std::make_unique<DX11::FramebufferManager>(m_target_width, m_target_height);
      static constexpr std::array<float, 4> clear_color{{0.f, 0.f, 0.f, 1.f}};
      DX11::D3D::context->ClearRenderTargetView(
          DX11::FramebufferManager::GetEFBColorTexture()->GetRTV(), clear_color.data());
      DX11::D3D::context->ClearDepthStencilView(
          DX11::FramebufferManager::GetEFBDepthTexture()->GetDSV(), D3D11_CLEAR_DEPTH, 0.f, 0);
    }

    CheckForHostConfigChanges();
    // begin next frame
    RestoreAPIState();
    DX11::D3D::stateman->Restore();
#endif
  }
};
#endif
}  // namespace Video
}  // namespace Libretro
