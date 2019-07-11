#include <cstdio>
#include <libretro.h>
#include <string>

#include "Common/CommonPaths.h"
#include "Common/FileUtil.h"
#include "Core/Boot/Boot.h"
#include "Core/BootManager.h"
#include "Core/Config/GraphicsSettings.h"
#include "Core/Config/SYSCONFSettings.h"
#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/HW/VideoInterface.h"
#include "Core/PowerPC/PowerPC.h"
#include "DolphinLibretro/Input.h"
#include "DolphinLibretro/Log.h"
#include "DolphinLibretro/Options.h"
#include "DolphinLibretro/Video.h"
#include "InputCommon/ControllerInterface/ControllerInterface.h"
//#include "UICommon/DiscordPresence.h"
#include "UICommon/UICommon.h"
#include "VideoCommon/AsyncRequests.h"
#include "VideoCommon/VideoBackendBase.h"
#include "VideoCommon/VideoConfig.h"

namespace Libretro
{
extern retro_environment_t environ_cb;
}

bool retro_load_game(const struct retro_game_info* game)
{
  const char* save_dir = NULL;
  const char* system_dir = NULL;
  const char* core_assets_dir = NULL;
  std::string user_dir;
  std::string sys_dir;

  Libretro::environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir);
  Libretro::environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir);
  Libretro::environ_cb(RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY, &core_assets_dir);

  if (save_dir && *save_dir)
    user_dir = std::string(save_dir) + DIR_SEP "User";
  else if (system_dir && *system_dir)
    user_dir = std::string(system_dir) + DIR_SEP "ishiiruka" DIR_SEP "User";

  if (system_dir && *system_dir)
    sys_dir = std::string(system_dir) + DIR_SEP "ishiiruka" DIR_SEP "Sys";
  else if (core_assets_dir && *core_assets_dir)
    sys_dir = std::string(core_assets_dir) + DIR_SEP "ishiiruka" DIR_SEP "Sys";
  else if (save_dir && *save_dir)
    sys_dir = std::string(save_dir) + DIR_SEP "Sys";
  else
    sys_dir = "ishiiruka" DIR_SEP "Sys";

  File::SetSysDirectory(sys_dir);
  UICommon::SetUserDirectory(user_dir);
  UICommon::CreateDirectories();
  UICommon::Init();
  Libretro::Log::Init();
#if 0
  Discord::SetDiscordPresenceEnabled(false);
#endif
  SetEnableAlert(false);

  INFO_LOG(COMMON, "User Directory set to '%s'", user_dir.c_str());
  INFO_LOG(COMMON, "System Directory set to '%s'", sys_dir.c_str());

  /* disable throttling emulation to match GetTargetRefreshRate() */
  Core::SetIsThrottlerTempDisabled(true);
  SConfig::GetInstance().m_EmulationSpeed = 0.0f;

  // Fastmem installs custom exception handlers
  // it needs to be disabled when running in a debugger.
  SConfig::GetInstance().bFastmem = Libretro::Options::fastmem;
  SConfig::GetInstance().bDSPHLE = Libretro::Options::DSPHLE;
  SConfig::GetInstance().m_DSPEnableJIT = Libretro::Options::DSPEnableJIT;
  SConfig::GetInstance().iCPUCore = Libretro::Options::cpu_core;
  SConfig::GetInstance().SelectedLanguage = (int)(DiscIO::Language)Libretro::Options::Language - 1;
  SConfig::GetInstance().bCPUThread = true;
  SConfig::GetInstance().bEMUThread = false;
  SConfig::GetInstance().bBootToPause = true;
  SConfig::GetInstance().m_enable_signature_checks = false;
  SConfig::GetInstance().m_OCFactor = Libretro::Options::cpuClockRate;
  SConfig::GetInstance().m_OCEnable = Libretro::Options::cpuClockRate != 1.0;
  SConfig::GetInstance().sBackend = BACKEND_NULLSOUND;
  SConfig::GetInstance().m_DumpAudio = false;
  SConfig::GetInstance().bDPL2Decoder = false;
  SConfig::GetInstance().iLatency = 0;
  SConfig::GetInstance().m_audio_stretch = false;

  Config::SetBase(Config::SYSCONF_LANGUAGE, (u32)(DiscIO::Language)Libretro::Options::Language);
  Config::SetBase(Config::SYSCONF_WIDESCREEN, (bool)Libretro::Options::Widescreen);
  Config::SetBase(Config::SYSCONF_PROGRESSIVE_SCAN, (bool)Libretro::Options::progressiveScan);
  Config::SetBase(Config::SYSCONF_PAL60, (bool)Libretro::Options::pal60);
  Config::SetBase(Config::SYSCONF_SENSOR_BAR_POSITION, (u32)Libretro::Options::sensorBarPosition);
  Config::SetBase(Config::GFX_WIDESCREEN_HACK, (bool)Libretro::Options::WidescreenHack);
#if 0
  Config::SetBase(Config::GFX_ASPECT_RATIO, AspectMode::Stretch);
#endif
  Config::SetBase(Config::GFX_EFB_SCALE, (int)Libretro::Options::efbScale);
  Config::SetBase(Config::GFX_BACKEND_MULTITHREADING, false);
#if 0
  Config::SetBase(Config::GFX_SHADER_COMPILER_THREADS, 1);
  Config::SetBase(Config::GFX_SHADER_PRECOMPILER_THREADS, 1);
  Config::SetBase(Config::GFX_SHADER_COMPILATION_MODE, Libretro::Options::shaderCompilationMode);
#endif

  Libretro::Video::Init();
  NOTICE_LOG(VIDEO, "Using GFX backend: %s", SConfig::GetInstance().m_strVideoBackend.c_str());

  if (!BootManager::BootCore(BootParameters::GenerateFromFile(game->path)))
  {
    ERROR_LOG(BOOT, "Could not boot %s\n", game->path);
    return false;
  }

  Libretro::Input::Init();

  return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info* info,
                             size_t num_info)
{
  return false;
}

void retro_unload_game(void)
{
  Core::Stop();
  Core::Shutdown();
  g_video_backend->ShutdownShared();
  Libretro::Input::Shutdown();
  Libretro::Log::Shutdown();
  UICommon::Shutdown();
}
