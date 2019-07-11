#include <libretro.h>

#include "RetroGLContext.h"
#include "Video.h"
#include "Options.h"

namespace Libretro
{
namespace Video
{

void RetroGLContext::Swap()
{
  video_cb(RETRO_HW_FRAME_BUFFER_VALID, m_backbuffer_width, m_backbuffer_height, 0);
}

bool RetroGLContext::Initialize(void* display_handle,
		void* window_handle, bool stereo, bool core)
{
  m_backbuffer_width = EFB_WIDTH * Libretro::Options::efbScale;
  m_backbuffer_height = EFB_HEIGHT * Libretro::Options::efbScale;
  switch (hw_render.context_type)
  {
  case RETRO_HW_CONTEXT_OPENGLES2:
    return false;
  case RETRO_HW_CONTEXT_OPENGLES3:
    m_opengl_mode = Mode::OpenGLES;
    break;
  default:
  case RETRO_HW_CONTEXT_OPENGL_CORE:
  case RETRO_HW_CONTEXT_OPENGL:
    m_opengl_mode = Mode::OpenGL;
    break;
  }
  return true;
}

void* RetroGLContext::GetFuncAddress(const std::string& name)
{
  return (void*)(hw_render.get_proc_address(name.c_str()));
}

}
}
