#ifndef IMPERATOR_MODULE_AUDIO_MGR_H
#define IMPERATOR_MODULE_AUDIO_MGR_H

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "imperator/module/event_bus.h"
#include "imperator/module/module_mgr.h"

#include <unordered_map>

namespace imp {
struct Sound {
    ALuint buffer; // 0 if failed to load
};

struct PlayOptions {
    float pitch{1.0f};  // valid range: 0.5 - 2.0
    float volume{1.0f}; // valid range: 0.0 - 1.0
    bool looping{false};
};

class AudioMgr : public Module<AudioMgr> {
public:
    std::shared_ptr<EventBus> event_bus;

    AudioMgr(ModuleMgr &module_mgr);
    ~AudioMgr();

    Sound load_sound(const std::filesystem::path &path);
    void unload_sound(Sound &sound);

    void play_sound(const Sound &sound, const PlayOptions &options = {});

    static std::vector<std::string> available_devices();

private:
    std::string device_specifier_{};
    ALCdevice *device_{nullptr};
    ALCcontext *ctx_{nullptr};

    std::unordered_map<ALuint, std::vector<ALuint>> sources_{};

    void r_update_(const E_Update &p);

    static ALuint load_sound_(const std::filesystem::path &path);

    static bool check_al_errors_();
    static bool check_alc_errors_(ALCdevice *device);
};
} // namespace imp

IMPERATOR_DECLARE_MODULE(imp::AudioMgr);

#endif //IMPERATOR_MODULE_AUDIO_MGR_H
