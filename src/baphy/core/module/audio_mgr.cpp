#include <utility>

#include "baphy/core/module/audio_mgr.hpp"

#include "baphy/util/rnd.hpp"

namespace baphy {

Sound::Sound(std::shared_ptr<AudioMgr> mgr, std::string tag, ALuint buffer)
    : mgr(std::move(mgr)), tag_(std::move(tag)), buffer_(buffer) {}

void Sound::play(const PlayOptions &options) {
  mgr->play_(buffer_, options);
}

bool AudioMgr::open_context() {
  ctx_ = alcCreateContext(device_, nullptr);
  bool ok = check_alc_errors_(device_);
  if (!ok)
    BAPHY_LOG_ERROR("Failed to create OpenAL context");
  return ok;
}

bool AudioMgr::make_current() {
  bool ok1 = alcMakeContextCurrent(ctx_);
  bool ok2 = check_alc_errors_(device_);
  if (!ok1 || !ok2)
    BAPHY_LOG_ERROR("Failed to make OpenAL context current");
  return ok1 && ok2;
}

std::vector<std::string> AudioMgr::available_devices() {
  std::vector<std::string> devices_v{};

  const ALCchar *devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
  if (!devices) {
    BAPHY_LOG_ERROR("Failed to get device list");
    return devices_v;
  }
  const char *ptr = devices;

  do {
    devices_v.emplace_back(ptr);
    ptr += devices_v.back().size() + 1;
  } while (*ptr != '\0');

  return devices_v;
}

bool AudioMgr::open_device(const std::string &device_name) {
  const char *device_cp = nullptr;
  if (!device_name.empty())
    device_cp = device_name.c_str();

  device_ = alcOpenDevice(device_cp);
  if (!device_) {
    BAPHY_LOG_ERROR("Failed to open audio device: '{}'", device_cp ? device_name : "default");
    return false;
  } else
    BAPHY_LOG_DEBUG("Opened audio device: '{}'", device_cp ? device_name : "default");

  if (alcIsExtensionPresent(device_, "ALC_SOFT_reopen_device"))
    alcReopenDeviceSOFT_ = (ALCboolean (ALC_APIENTRY*)(ALCdevice *device, const ALCchar *name, const ALCint *attribs))alcGetProcAddress(device_, "alcReopenDeviceSOFT");
  else
    BAPHY_LOG_WARN("Reopen extension not supported on this system; change in audio device will require reloading all audio buffers");

  return true;
}

bool AudioMgr::reopen_device(const std::string &device_name) {
  if (!alcReopenDeviceSOFT_) {
    const char *device_cp = nullptr;
    if (!device_name.empty())
      device_cp = device_name.c_str();

    bool ok = alcReopenDeviceSOFT_(device_, device_cp, nullptr);
    check_alc_errors_(device_);
    if (!ok) {
      BAPHY_LOG_ERROR("Failed to reopen audio device: '{}'", device_cp ? device_name : "default");
      return false;
    } else
      BAPHY_LOG_DEBUG("Reopened audio device: '{}'", device_cp ? device_name : "default");

  } else {
    BAPHY_LOG_ERROR("Failed to reopen audio device: '{}'; extension not supported on this system");
    return false;
  }

  return true;
}

std::shared_ptr<Sound> AudioMgr::load(const std::filesystem::path &path) {
  std::unique_ptr<nqr::AudioData> file_data = std::make_unique<nqr::AudioData>();
  audio_loader_.Load(file_data.get(), path.string());

  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_errors_();

  ALenum format;
  if (file_data->channelCount == 1)
    format = AL_FORMAT_MONO_FLOAT32;
  else if (file_data->channelCount == 2)
    format = AL_FORMAT_STEREO_FLOAT32;
  else {
    spdlog::error("Unrecognized audio format: {} channels", file_data->channelCount);
    return nullptr;
  }

  alBufferData(
      buffer, format,
      &file_data->samples[0],
      file_data->samples.size() * sizeof(float),
      file_data->sampleRate
  );
  bool ok = check_al_errors_();
  if (!ok) {
    BAPHY_LOG_ERROR("Failed to load audio: '{}'", path.string());
    return nullptr;
  } else
    BAPHY_LOG_DEBUG("Loaded audio: '{}'", path.string());

  std::string tag = rnd::base58(11);
  sounds_[tag] = std::make_shared<Sound>(shared_from_this(), tag, buffer);
  return sounds_[tag];
}

void AudioMgr::play_(ALuint buffer, const PlayOptions &options) {
  ALuint source;
  alGenSources(1, &source);
  check_al_errors_();
  sources_[buffer].emplace_back(source);

  alSourcef(sources_[buffer].back(), AL_PITCH, options.pitch);
  alSourcef(sources_[buffer].back(), AL_GAIN, options.volume);
  alSource3f(sources_[buffer].back(), AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(sources_[buffer].back(), AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei(sources_[buffer].back(), AL_LOOPING, options.looping ? AL_TRUE : AL_FALSE);
  alSourcei(sources_[buffer].back(), AL_BUFFER, buffer);

  alSourcePlay(sources_[buffer].back());
  check_al_errors_();
}

bool AudioMgr::check_al_errors_() {
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    switch (error) {
      case AL_INVALID_NAME:
        BAPHY_LOG_ERROR("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
        break;
      case AL_INVALID_ENUM:
        BAPHY_LOG_ERROR("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
        break;
      case AL_INVALID_VALUE:
        BAPHY_LOG_ERROR("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
      case AL_INVALID_OPERATION:
        BAPHY_LOG_ERROR("AL_INVALID_OPERATION: the requested operation is not valid");
        break;
      case AL_OUT_OF_MEMORY:
        BAPHY_LOG_ERROR("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
        break;
      default:
        BAPHY_LOG_ERROR("UNKNOWN AL ERROR: {}", error);
    }
    return false;
  }
  return true;
}

bool AudioMgr::check_alc_errors_(ALCdevice *device) {
  ALCenum error = alcGetError(device);
  if (error != ALC_NO_ERROR) {
    switch (error) {
      case ALC_INVALID_VALUE:
        BAPHY_LOG_ERROR("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
      case ALC_INVALID_DEVICE:
        BAPHY_LOG_ERROR("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
        break;
      case ALC_INVALID_CONTEXT:
        BAPHY_LOG_ERROR("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
        break;
      case ALC_INVALID_ENUM:
        BAPHY_LOG_ERROR("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
        break;
      case ALC_OUT_OF_MEMORY:
        BAPHY_LOG_ERROR("ALC_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
        break;
      default:
        BAPHY_LOG_ERROR("UNKNOWN ALC ERROR: {}", error);
    }
    return false;
  }
  return true;
}

bool AudioMgr::check_alc_errors_() {
  return check_alc_errors_(device_);
}

void AudioMgr::e_initialize_(const EInitialize &e) {
  EventBus::sub<EUpdate>(module_name, [&](const auto &e) { e_update_(e); });

  Module::e_initialize_(e);
}

void AudioMgr::e_shutdown_(const EShutdown &e) {
  Module::e_shutdown_(e);
}

void AudioMgr::e_update_(const EUpdate &e) {
  for (auto s_it = sources_.begin(); s_it != sources_.end(); ) {
    auto &l = s_it->second;

    for (auto it = l.begin(); it != l.end(); ) {
      ALint state;
      alGetSourcei(*it, AL_SOURCE_STATE, &state);
      check_al_errors_();

      if (state == AL_STOPPED) {
        alDeleteSources(1, &(*it));
        check_al_errors_();
        it = l.erase(it);
        continue;
      }
      it++;
    }

    if (s_it->second.empty()) {
      s_it = sources_.erase(s_it);
      continue;
    }
    s_it++;
  }
}

} // namespace baphy
