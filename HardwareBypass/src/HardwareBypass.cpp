
#include <lv2.h>
#include <alsa/asoundlib.h>

/**********************************************************************************************************************************************************/

#ifdef __aarch64__
 #define ALSA_SOUNDCARD_ID        "hw:DUOX"
#else
 #define ALSA_SOUNDCARD_ID        "hw:MODDUO"
#endif
#define ALSA_CONTROL_BYPASS_LEFT  "Left True-Bypass"
#define ALSA_CONTROL_BYPASS_RIGHT "Right True-Bypass"

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/HardwareBypass"

enum {ENABLED, MODE, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

struct AlsaControls {
    snd_mixer_t* mixer;
    snd_mixer_elem_t* left;
    snd_mixer_elem_t* right;

    AlsaControls()
        : mixer(NULL), left(NULL), right(NULL) {}
};

class HardwareBypassPlugin
{
public:
    HardwareBypassPlugin() {}
    ~HardwareBypassPlugin() {}
    static LV2_Handle instantiate(const LV2_Descriptor*, double, const char*, const LV2_Feature* const*);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    const float *enabled;
    const float *mode;

    AlsaControls alsa;
    bool first_run;
    bool last_enabled;
    int last_mode;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    HardwareBypassPlugin::instantiate,
    HardwareBypassPlugin::connect_port,
    NULL,
    HardwareBypassPlugin::run,
    NULL,
    HardwareBypassPlugin::cleanup,
    NULL
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle HardwareBypassPlugin::instantiate(const LV2_Descriptor*, double, const char*, const LV2_Feature* const*)
{
    AlsaControls alsa;

    if (snd_mixer_open(&alsa.mixer, SND_MIXER_ELEM_SIMPLE) != 0)
        return NULL;

    snd_mixer_selem_id_t* sid;

    if (snd_mixer_attach(alsa.mixer, ALSA_SOUNDCARD_ID) == 0 &&
        snd_mixer_selem_register(alsa.mixer, NULL, NULL) == 0 &&
        snd_mixer_load(alsa.mixer) == 0 &&
        snd_mixer_selem_id_malloc(&sid) == 0)
    {
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, ALSA_CONTROL_BYPASS_LEFT);
        alsa.left = snd_mixer_find_selem(alsa.mixer, sid);

        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, ALSA_CONTROL_BYPASS_RIGHT);
        alsa.right = snd_mixer_find_selem(alsa.mixer, sid);

        snd_mixer_selem_id_free(sid);
    }
    else
    {
        snd_mixer_close(alsa.mixer);
        return NULL;
    }

    if (alsa.left == NULL || alsa.right == NULL)
    {
        snd_mixer_close(alsa.mixer);
        return NULL;
    }

    HardwareBypassPlugin *plugin = new HardwareBypassPlugin();

    plugin->alsa = alsa;
    plugin->first_run = true;
    plugin->last_enabled = false;
    plugin->last_mode = 0;

    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void HardwareBypassPlugin::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    HardwareBypassPlugin *plugin;
    plugin = (HardwareBypassPlugin *) instance;

    switch (port)
    {
        case ENABLED:
            plugin->enabled = (const float*) data;
            break;
        case MODE:
            plugin->mode = (const float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

static void set_alsa_state(const AlsaControls& alsa, const bool enabled, const int mode)
{
    int left = 0, right = 0;

    if (enabled)
    {
        switch (mode)
        {
        case 0:
            left = 1;
            right = 1;
            break;
        case 1:
            left = 1;
            break;
        case 2:
            right = 1;
            break;
        }
    }

    snd_mixer_selem_set_playback_switch_all(alsa.left, left);
    snd_mixer_selem_set_playback_switch_all(alsa.right, right);
}

void HardwareBypassPlugin::run(LV2_Handle instance, uint32_t n_samples)
{
    HardwareBypassPlugin *plugin;
    plugin = (HardwareBypassPlugin *) instance;

    const bool enabled = *plugin->enabled > 0.5f;
    const int mode = (int)(*plugin->mode + 0.5f);

    if (plugin->first_run)
    {
        plugin->first_run = false;
        plugin->last_enabled = enabled;
        plugin->last_mode = mode;
        set_alsa_state(plugin->alsa, enabled, mode);
    }
    else if (plugin->last_enabled != enabled || plugin->last_mode != mode)
    {
        plugin->last_enabled = enabled;
        plugin->last_mode = mode;
        set_alsa_state(plugin->alsa, enabled, mode);
    }
}

/**********************************************************************************************************************************************************/

void HardwareBypassPlugin::cleanup(LV2_Handle instance)
{
    delete ((HardwareBypassPlugin *) instance);
}
