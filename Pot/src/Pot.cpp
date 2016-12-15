#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Pot"
enum {IN, OUT, LEVEL};

/**********************************************************************************************************************************************************/

class Pot
{
public:
    Pot() {}
    ~Pot() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out;
    float *level;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    Pot::instantiate,
    Pot::connect_port,
    Pot::activate,
    Pot::run,
    Pot::deactivate,
    Pot::cleanup,
    Pot::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Pot::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    Pot *plugin = new Pot();
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Pot::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void Pot::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void Pot::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Pot *plugin;
    plugin = (Pot *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT:
            plugin->out = (float*) data;
            break;
        case LEVEL:
            plugin->level = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void Pot::run(LV2_Handle instance, uint32_t n_samples)
{
    Pot *plugin;
    plugin = (Pot *) instance;


    float *in = plugin->in;
    float *out = plugin->out;
    float *level = plugin->level;
    
    //linear
    float factor = level;

    for(uint32_t i = 0; i < n_samples; i++){
        out[i] = in[i] * factor;
    }
}

/**********************************************************************************************************************************************************/

void Pot::cleanup(LV2_Handle instance)
{
    delete ((Pot *) instance);
}

/**********************************************************************************************************************************************************/

const void* Pot::extension_data(const char* uri)
{
    return NULL;
}
