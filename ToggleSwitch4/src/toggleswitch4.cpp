#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/ToggleSwitch4"
enum {IN, OUT_1, OUT_2, OUT_3, OUT_4, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4};

/**********************************************************************************************************************************************************/

class ToggleSwitch
{
public:
    ToggleSwitch() {}
    ~ToggleSwitch() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out_1;
    float *out_2;
    float *out_3;
    float *out_4;
    float *channel1;
    float *channel2;
    float *channel3;
    float *channel4;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    ToggleSwitch::instantiate,
    ToggleSwitch::connect_port,
    ToggleSwitch::activate,
    ToggleSwitch::run,
    ToggleSwitch::deactivate,
    ToggleSwitch::cleanup,
    ToggleSwitch::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle ToggleSwitch::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    ToggleSwitch *plugin = new ToggleSwitch();
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void ToggleSwitch::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void ToggleSwitch::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void ToggleSwitch::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    ToggleSwitch *plugin;
    plugin = (ToggleSwitch *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case OUT_2:
            plugin->out_2 = (float*) data;
            break;
        case OUT_3:
            plugin->out_3 = (float*) data;
            break;
        case OUT_4:
            plugin->out_4 = (float*) data;
            break;
        case CHANNEL1:
            plugin->channel1 = (float*) data;
            break;
        case CHANNEL2:
            plugin->channel2 = (float*) data;
            break;
        case CHANNEL3:
            plugin->channel3 = (float*) data;
            break;
        case CHANNEL4:
            plugin->channel4 = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void ToggleSwitch::run(LV2_Handle instance, uint32_t n_samples)
{
    ToggleSwitch *plugin;
    plugin = (ToggleSwitch *) instance;


    float *in = plugin->in;
    float *out_1 = plugin->out_1;
    float *out_2 = plugin->out_2;
    float *out_3 = plugin->out_3;
    float *out_4 = plugin->out_4;

    for(uint32_t i = 0; i < n_samples; i++){
        out_1[i] = in[i]*(*(plugin->channel1));
        out_2[i] = in[i]*(*(plugin->channel2));
        out_3[i] = in[i]*(*(plugin->channel3));
        out_4[i] = in[i]*(*(plugin->channel4));
    }
}

/**********************************************************************************************************************************************************/

void ToggleSwitch::cleanup(LV2_Handle instance)
{
    delete ((ToggleSwitch *) instance);
}

/**********************************************************************************************************************************************************/

const void* ToggleSwitch::extension_data(const char* uri)
{
    return NULL;
}
