#ifndef BAPHY_CORE_MODULE_AUDIO_MGR_HPP
#define BAPHY_CORE_MODULE_AUDIO_MGR_HPP

#include "baphy/core/module_mgr.hpp"
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "libnyquist/Decoders.h"
#include <string>
#include <vector>

namespace baphy {

struct PlayOptions {
  float pitch{1.0f};    // valid range: 0.5 - 2.0
  float volume{1.0f};   // valid range: 0.0 - 1.0
  bool looping{false};
};

class AudioMgr;

class Sound {
public:
  std::shared_ptr<AudioMgr> mgr{nullptr};

  Sound(std::shared_ptr<AudioMgr> mgr, std::string tag, ALuint buffer);

  void play(const PlayOptions &options = {});

private:
  std::string tag_;
  ALuint buffer_;
};

class AudioMgr : public Module<AudioMgr>, public std::enable_shared_from_this<AudioMgr> {
  friend class Sound;

public:
  AudioMgr() : Module<AudioMgr>() {}
  ~AudioMgr() override = default;

  bool open_context();
  bool make_current();

  std::vector<std::string> available_devices();
  bool open_device(const std::string &device_name = "");
  bool reopen_device(const std::string &device_name = "");

  std::shared_ptr<Sound> load(const std::filesystem::path &path);

private:
  ALCcontext *ctx_{nullptr};
  ALCdevice *device_{nullptr};

  nqr::NyquistIO audio_loader_{};
  std::unordered_map<std::string, std::shared_ptr<Sound>> sounds_{};
  std::unordered_map<ALuint, std::vector<ALuint>> sources_{};

  void play_(ALuint buffer, const PlayOptions &options = {});

  bool check_al_errors_();
  bool check_alc_errors_(ALCdevice *device);
  bool check_alc_errors_();

  ALCboolean (ALC_APIENTRY *alcReopenDeviceSOFT_)(ALCdevice *device, const ALCchar *name, const ALCint *attribs){nullptr};

  void e_initialize_(const EInitialize &e) override;
  void e_shutdown_(const EShutdown &e) override;

  void e_update_(const EUpdate &e);
};

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::AudioMgr);

#endif//BAPHY_CORE_MODULE_AUDIO_MGR_HPP
