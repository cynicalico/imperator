#include "imperator/module/audio_mgr.h"
#include "imperator/util/log.h"
#include "imperator/util/platform.h"

#include <windows.h>

namespace imp {
enum FormatType { Int16, Float, IMA4, MSADPCM };

const char *FormatName(ALenum format) {
    switch (format) {
    case AL_FORMAT_MONO8:                return "Mono, U8";
    case AL_FORMAT_MONO16:               return "Mono, S16";
    case AL_FORMAT_MONO_FLOAT32:         return "Mono, Float32";
    case AL_FORMAT_MONO_MULAW:           return "Mono, muLaw";
    case AL_FORMAT_MONO_ALAW_EXT:        return "Mono, aLaw";
    case AL_FORMAT_MONO_IMA4:            return "Mono, IMA4 ADPCM";
    case AL_FORMAT_MONO_MSADPCM_SOFT:    return "Mono, MS ADPCM";
    case AL_FORMAT_STEREO8:              return "Stereo, U8";
    case AL_FORMAT_STEREO16:             return "Stereo, S16";
    case AL_FORMAT_STEREO_FLOAT32:       return "Stereo, Float32";
    case AL_FORMAT_STEREO_MULAW:         return "Stereo, muLaw";
    case AL_FORMAT_STEREO_ALAW_EXT:      return "Stereo, aLaw";
    case AL_FORMAT_STEREO_IMA4:          return "Stereo, IMA4 ADPCM";
    case AL_FORMAT_STEREO_MSADPCM_SOFT:  return "Stereo, MS ADPCM";
    case AL_FORMAT_QUAD8:                return "Quadraphonic, U8";
    case AL_FORMAT_QUAD16:               return "Quadraphonic, S16";
    case AL_FORMAT_QUAD32:               return "Quadraphonic, Float32";
    case AL_FORMAT_QUAD_MULAW:           return "Quadraphonic, muLaw";
    case AL_FORMAT_51CHN8:               return "5.1 Surround, U8";
    case AL_FORMAT_51CHN16:              return "5.1 Surround, S16";
    case AL_FORMAT_51CHN32:              return "5.1 Surround, Float32";
    case AL_FORMAT_51CHN_MULAW:          return "5.1 Surround, muLaw";
    case AL_FORMAT_61CHN8:               return "6.1 Surround, U8";
    case AL_FORMAT_61CHN16:              return "6.1 Surround, S16";
    case AL_FORMAT_61CHN32:              return "6.1 Surround, Float32";
    case AL_FORMAT_61CHN_MULAW:          return "6.1 Surround, muLaw";
    case AL_FORMAT_71CHN8:               return "7.1 Surround, U8";
    case AL_FORMAT_71CHN16:              return "7.1 Surround, S16";
    case AL_FORMAT_71CHN32:              return "7.1 Surround, Float32";
    case AL_FORMAT_71CHN_MULAW:          return "7.1 Surround, muLaw";
    case AL_FORMAT_BFORMAT2D_8:          return "B-Format 2D, U8";
    case AL_FORMAT_BFORMAT2D_16:         return "B-Format 2D, S16";
    case AL_FORMAT_BFORMAT2D_FLOAT32:    return "B-Format 2D, Float32";
    case AL_FORMAT_BFORMAT2D_MULAW:      return "B-Format 2D, muLaw";
    case AL_FORMAT_BFORMAT3D_8:          return "B-Format 3D, U8";
    case AL_FORMAT_BFORMAT3D_16:         return "B-Format 3D, S16";
    case AL_FORMAT_BFORMAT3D_FLOAT32:    return "B-Format 3D, Float32";
    case AL_FORMAT_BFORMAT3D_MULAW:      return "B-Format 3D, muLaw";
    case AL_FORMAT_UHJ2CHN8_SOFT:        return "UHJ 2-channel, U8";
    case AL_FORMAT_UHJ2CHN16_SOFT:       return "UHJ 2-channel, S16";
    case AL_FORMAT_UHJ2CHN_FLOAT32_SOFT: return "UHJ 2-channel, Float32";
    case AL_FORMAT_UHJ3CHN8_SOFT:        return "UHJ 3-channel, U8";
    case AL_FORMAT_UHJ3CHN16_SOFT:       return "UHJ 3-channel, S16";
    case AL_FORMAT_UHJ3CHN_FLOAT32_SOFT: return "UHJ 3-channel, Float32";
    case AL_FORMAT_UHJ4CHN8_SOFT:        return "UHJ 4-channel, U8";
    case AL_FORMAT_UHJ4CHN16_SOFT:       return "UHJ 4-channel, S16";
    case AL_FORMAT_UHJ4CHN_FLOAT32_SOFT: return "UHJ 4-channel, Float32";
    default:                             return "Unknown Format";
    }
}

AudioMgr::AudioMgr(ModuleMgr &module_mgr) : Module(module_mgr) {
    event_bus = module_mgr_.get<EventBus>();

    event_bus->sub<E_Update>(module_name_, [&](const auto &p) { r_update_(p); });

    auto default_device_name = available_devices()[0];

    device_ = alcOpenDevice(default_device_name.c_str());
    if (!device_ || !check_alc_errors_(device_)) {
        IMPERATOR_LOG_ERROR("Failed to open OpenAL device!");
        return;
    }

    ctx_ = alcCreateContext(device_, nullptr);
    if (!ctx_ || !check_alc_errors_(device_)) {
        if (ctx_) { alcDestroyContext(ctx_); }
        IMPERATOR_LOG_ERROR("Failed to create OpenAL context");
        return;
    }

    bool context_current_ok = alcMakeContextCurrent(ctx_);
    if (!context_current_ok || !check_alc_errors_(device_)) {
        alcDestroyContext(ctx_);
        alcCloseDevice(device_);
        IMPERATOR_LOG_ERROR("Failed to make OpenAL context current");
        return;
    }

    const ALCchar *name{};
    if (alcIsExtensionPresent(device_, "ALC_ENUMERATE_ALL_EXT")) {
        name = alcGetString(device_, ALC_ALL_DEVICES_SPECIFIER);
    }
    if (!name || !check_alc_errors_(device_)) { name = alcGetString(device_, ALC_DEVICE_SPECIFIER); }
    device_specifier_ = name;

    IMPERATOR_LOG_DEBUG("Opened \"{}\"", device_specifier_);
}

AudioMgr::~AudioMgr() {
    for (const auto &[buffer, sources]: sources_) {
        for (const auto &source: sources) {
            alSourceStop(source);
            check_al_errors_();
        }

        if (!sources.empty()) {
            alDeleteSources(sources.size(), &sources[0]);
            check_al_errors_();
        }

        alDeleteBuffers(1, &buffer);
        check_al_errors_();
    }

    if (!alcMakeContextCurrent(nullptr) || !check_alc_errors_(device_)) {
        IMPERATOR_LOG_ERROR("Failed to clear OpenAL context!");
    }

    alcDestroyContext(ctx_);
    if (!check_alc_errors_(device_)) { IMPERATOR_LOG_ERROR("Failed to destroy OpenAL context!"); }

    alcCloseDevice(device_);
    if (!check_alc_errors_(device_)) { IMPERATOR_LOG_ERROR("Failed to close OpenAL device!"); }
}

Sound AudioMgr::load_sound(const std::filesystem::path &path) {
    auto sound = load_sound_(path);
    if (sound.buffer != 0) {
        sources_[sound.buffer] = {};

        sf_count_t frames = sound.sfinfo.frames;
        sf_count_t h = frames / (3600 * sound.sfinfo.samplerate);
        frames -= h * (3600 * sound.sfinfo.samplerate);
        sf_count_t m = frames / (60 * sound.sfinfo.samplerate);
        frames -= m * (60 * sound.sfinfo.samplerate);
        sf_count_t s = frames / sound.sfinfo.samplerate;
        frames -= s * sound.sfinfo.samplerate;

        IMPERATOR_LOG_DEBUG(
                "Loaded: {} ({}, {}hz, {})",
                sound.path,
                FormatName(sound.format),
                sound.sfinfo.samplerate,
                fmt::format("{:02}:{:02}:{:02}.{}", h, m, s, frames)
        );
    }
    return sound;
}

void AudioMgr::unload_sound(Sound &sound) {
    const auto it = sources_.find(sound.buffer);

    if (it != sources_.end()) {
        for (const auto &source: it->second) {
            alSourceStop(source);
            check_al_errors_();
        }

        if (!it->second.empty()) {
            alDeleteSources(it->second.size(), &it->second[0]);
            check_al_errors_();
        }

        alDeleteBuffers(1, &sound.buffer);
        check_al_errors_();

        sources_.erase(it);

        IMPERATOR_LOG_DEBUG("Unloaded: {}", sound.path);
        // Reset object so the user doesn't try to play a non-existent sound
        sound.buffer = 0;
        sound.path.clear();
        sound.format = AL_NONE;
        sound.sfinfo = {};
    }
}

void AudioMgr::play_sound(const Sound &sound, const PlayOptions &options) {
    ALuint source;
    alGenSources(1, &source);
    check_al_errors_();
    sources_[sound.buffer].emplace_back(source);

    alSourcef(sources_[sound.buffer].back(), AL_PITCH, options.pitch);
    alSourcef(sources_[sound.buffer].back(), AL_GAIN, options.volume);
    alSource3f(sources_[sound.buffer].back(), AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(sources_[sound.buffer].back(), AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(sources_[sound.buffer].back(), AL_LOOPING, options.looping ? AL_TRUE : AL_FALSE);
    alSourcei(sources_[sound.buffer].back(), AL_BUFFER, sound.buffer);

    alSourcePlay(sources_[sound.buffer].back());
    check_al_errors_();
}

std::vector<std::string> AudioMgr::available_devices() {
    std::vector<std::string> devices_v{};

    const ALCchar *devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
    if (!devices) {
        IMPERATOR_LOG_ERROR("Failed to get device list");
        return devices_v;
    }
    const char *ptr = devices;

    do {
        devices_v.emplace_back(ptr);
        ptr += devices_v.back().size() + 1;
    } while (*ptr != '\0');

    return devices_v;
}

void AudioMgr::r_update_(const E_Update &p) {
    for (auto s_it = sources_.begin(); s_it != sources_.end();) {
        auto &l = s_it->second;

        for (auto it = l.begin(); it != l.end();) {
            ALint state;
            alGetSourcei(*it, AL_SOURCE_STATE, &state);
            check_al_errors_();

            if (state == AL_STOPPED) {
                alDeleteSources(1, &(*it));
                check_al_errors_();
                it = l.erase(it);
                continue;
            }
            ++it;
        }

        ++s_it;
    }
}

Sound AudioMgr::load_sound_(const std::filesystem::path &path) {
    FormatType sample_format = Int16;
    ALint byteblockalign = 0;
    ALint splblockalign = 0;
    SF_INFO sfinfo;

    /* Open the audio file and check that it's usable. */
#if defined(IMPERATOR_PLATFORM_WINDOWS)
    SNDFILE *sndfile = sf_wchar_open(path.c_str(), SFM_READ, &sfinfo);
#else
    SNDFILE *sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
#endif
    if (!sndfile) {
        IMPERATOR_LOG_ERROR("Could not open audio file '{}'", path);
        IMPERATOR_LOG_ERROR("{}", sf_strerror(sndfile));
        return {};
    }
    if (sfinfo.frames < 1) {
        IMPERATOR_LOG_ERROR("Bad sample count in {} ({})", path, sfinfo.frames);
        sf_close(sndfile);
        return {};
    }

    /* Detect a suitable format to load. Formats like Vorbis and Opus use float
     * natively, so load as float to avoid clipping when possible. Formats
     * larger than 16-bit can also use float to preserve a bit more precision.
     */
    switch (sfinfo.format & SF_FORMAT_SUBMASK) {
    case SF_FORMAT_PCM_24:
    case SF_FORMAT_PCM_32:
    case SF_FORMAT_FLOAT:
    case SF_FORMAT_DOUBLE:
    case SF_FORMAT_VORBIS:
    case SF_FORMAT_OPUS:
    case SF_FORMAT_ALAC_20:
    case SF_FORMAT_ALAC_24:
    case SF_FORMAT_ALAC_32:
    case 0x0080 /*SF_FORMAT_MPEG_LAYER_I*/:
    case 0x0081 /*SF_FORMAT_MPEG_LAYER_II*/:
    case 0x0082 /*SF_FORMAT_MPEG_LAYER_III*/:
        if (alIsExtensionPresent("AL_EXT_FLOAT32")) { sample_format = Float; }
        break;
    case SF_FORMAT_IMA_ADPCM:
        /* ADPCM formats require setting a block alignment as specified in the
         * file, which needs to be read from the wave 'fmt ' chunk manually
         * since libsndfile doesn't provide it in a format-agnostic way.
         */
        if (sfinfo.channels <= 2 && (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV &&
            alIsExtensionPresent("AL_EXT_IMA4") && alIsExtensionPresent("AL_SOFT_block_alignment")) {
            sample_format = IMA4;
        }
        break;
    case SF_FORMAT_MS_ADPCM:
        if (sfinfo.channels <= 2 && (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV &&
            alIsExtensionPresent("AL_SOFT_MSADPCM") && alIsExtensionPresent("AL_SOFT_block_alignment")) {
            sample_format = MSADPCM;
        }
        break;
    }

    if (sample_format == IMA4 || sample_format == MSADPCM) {
        /* For ADPCM, lookup the wave file's "fmt " chunk, which is a
         * WAVEFORMATEX-based structure for the audio format.
         */
        SF_CHUNK_INFO inf = {"fmt ", 4, 0, nullptr};
        SF_CHUNK_ITERATOR *iter = sf_get_chunk_iterator(sndfile, &inf);

        /* If there's an issue getting the chunk or block alignment, load as
         * 16-bit and have libsndfile do the conversion.
         */
        if (!iter || sf_get_chunk_size(iter, &inf) != SF_ERR_NO_ERROR || inf.datalen < 14) {
            sample_format = Int16;
        } else {
            auto *fmtbuf = static_cast<ALubyte *>(calloc(inf.datalen, 1));
            inf.data = fmtbuf;
            if (sf_get_chunk_data(iter, &inf) != SF_ERR_NO_ERROR) {
                sample_format = Int16;
            } else {
                /* Read the nBlockAlign field, and convert from bytes- to
                 * samples-per-block (verifying it's valid by converting back
                 * and comparing to the original value).
                 */
                byteblockalign = fmtbuf[12] | (fmtbuf[13] << 8);
                if (sample_format == IMA4) {
                    splblockalign = (byteblockalign / sfinfo.channels - 4) / 4 * 8 + 1;
                    if (splblockalign < 1 || ((splblockalign - 1) / 2 + 4) * sfinfo.channels != byteblockalign) {
                        sample_format = Int16;
                    }
                } else {
                    splblockalign = (byteblockalign / sfinfo.channels - 7) * 2 + 2;
                    if (splblockalign < 2 || ((splblockalign - 2) / 2 + 7) * sfinfo.channels != byteblockalign) {
                        sample_format = Int16;
                    }
                }
            }
            free(fmtbuf);
        }
    }

    if (sample_format == Int16) {
        splblockalign = 1;
        byteblockalign = sfinfo.channels * 2;
    } else if (sample_format == Float) {
        splblockalign = 1;
        byteblockalign = sfinfo.channels * 4;
    }

    /* Figure out the OpenAL format from the file and desired sample type. */
    ALenum format = AL_NONE;
    if (sfinfo.channels == 1) {
        switch (sample_format) {
        case Int16:   format = AL_FORMAT_MONO16; break;
        case Float:   format = AL_FORMAT_MONO_FLOAT32; break;
        case IMA4:    format = AL_FORMAT_MONO_IMA4; break;
        case MSADPCM: format = AL_FORMAT_MONO_MSADPCM_SOFT; break;
        }
    } else if (sfinfo.channels == 2) {
        switch (sample_format) {
        case Int16:   format = AL_FORMAT_STEREO16; break;
        case Float:   format = AL_FORMAT_STEREO_FLOAT32; break;
        case IMA4:    format = AL_FORMAT_STEREO_IMA4; break;
        case MSADPCM: format = AL_FORMAT_STEREO_MSADPCM_SOFT; break;
        }
    } else if (sfinfo.channels == 3) {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT) {
            switch (sample_format) {
            case Int16: format = AL_FORMAT_BFORMAT2D_16; break;
            case Float: format = AL_FORMAT_BFORMAT2D_FLOAT32; break;
            default:    ; // no change
            }
        }
    } else if (sfinfo.channels == 4) {
        if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT) {
            switch (sample_format) {
            case Int16: format = AL_FORMAT_BFORMAT3D_16; break;
            case Float: format = AL_FORMAT_BFORMAT3D_FLOAT32; break;
            default:    ; // no change
            }
        }
    }
    if (!format) {
        IMPERATOR_LOG_ERROR("Unsupported channel count: {}", sfinfo.channels);
        sf_close(sndfile);
        return {};
    }

    if (sfinfo.frames / splblockalign > static_cast<sf_count_t>(INT_MAX / byteblockalign)) {
        IMPERATOR_LOG_ERROR("Too many samples in {} ({})", path, sfinfo.frames);
        sf_close(sndfile);
        return {};
    }

    /* Decode the whole audio file to a buffer. */
    void *membuf = malloc(static_cast<size_t>(sfinfo.frames / splblockalign * byteblockalign));
    sf_count_t num_frames;

    if (sample_format == Int16) {
        num_frames = sf_readf_short(sndfile, static_cast<short *>(membuf), sfinfo.frames);
    } else if (sample_format == Float) {
        num_frames = sf_readf_float(sndfile, static_cast<float *>(membuf), sfinfo.frames);
    } else {
        sf_count_t count = sfinfo.frames / splblockalign * byteblockalign;
        num_frames = sf_read_raw(sndfile, membuf, count);
        if (num_frames > 0) { num_frames = num_frames / byteblockalign * splblockalign; }
    }
    if (num_frames < 1) {
        free(membuf);
        sf_close(sndfile);
        IMPERATOR_LOG_ERROR("Failed to read samples in {} ({})", path, num_frames);
        return {};
    }
    const auto num_bytes = static_cast<ALsizei>(num_frames / splblockalign * byteblockalign);

    /* Buffer the audio data into a new buffer object, then free the data and
     * close the file.
     */
    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    if (splblockalign > 1) { alBufferi(buffer, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, splblockalign); }
    alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

    free(membuf);
    sf_close(sndfile);

    /* Check if an error occurred, and clean up if so. */
    if (!check_al_errors_()) {
        if (buffer && alIsBuffer(buffer)) { alDeleteBuffers(1, &buffer); }
        return {};
    }

    return {buffer, path, format, sfinfo};
}

bool AudioMgr::check_al_errors_() {
    if (ALenum error = alGetError(); error != AL_NO_ERROR) {
        IMPERATOR_LOG_ERROR(std::invoke([error] {
            switch (error) {
            case AL_INVALID_NAME:      return "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
            case AL_INVALID_ENUM:      return "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
            case AL_INVALID_VALUE:     return "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION: the requested operation is not valid";
            case AL_OUT_OF_MEMORY:
                return "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
            default: return fmt::format("UNKNOWN AL ERROR: {}", error).c_str();
            }
        }));
        return false;
    }
    return true;
}

bool AudioMgr::check_alc_errors_(ALCdevice *device) {
    if (ALCenum error = alcGetError(device); error != ALC_NO_ERROR) {
        IMPERATOR_LOG_ERROR(std::invoke([error] {
            switch (error) {
            case ALC_INVALID_VALUE:   return "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            case ALC_INVALID_DEVICE:  return "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
            case ALC_INVALID_CONTEXT: return "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
            case ALC_INVALID_ENUM:    return "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
            case ALC_OUT_OF_MEMORY:
                return "ALC_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
            default: return fmt::format("UNKNOWN ALC ERROR: {}", error).c_str();
            }
        }));
        return false;
    }
    return true;
}
} // namespace imp
