#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <lv2.h>

#ifdef USE_NE10
#include <NE10.h>
#endif

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Gain2x2"
#define TAMANHO_DO_BUFFER 1024
enum {IN_1, IN_2, OUT_1, OUT_2, GAIN, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class GainControl
{
public:
    GainControl() {}
    ~GainControl() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    float *in_1;
    float *in_2;
    float *out_1;
    float *out_2;
    float *gain;

    float g;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    GainControl::instantiate,
    GainControl::connect_port,
    NULL,
    GainControl::run,
    NULL,
    GainControl::cleanup,
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

LV2_Handle GainControl::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    GainControl *plugin = new GainControl();

    // default gain value is 3.0f
    plugin->g = powf(10.0f, 3.0f/20.0f);

    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void GainControl::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    GainControl *plugin;
    plugin = (GainControl *) instance;

    switch (port)
    {
        case IN_1:
            plugin->in_1 = (float*) data;
            break;
        case IN_2:
            plugin->in_2 = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case OUT_2:
            plugin->out_2 = (float*) data;
            break;
        case GAIN:
            plugin->gain = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void GainControl::run(LV2_Handle instance, uint32_t n_samples)
{
    GainControl *plugin = (GainControl *) instance;

    const float g_before = plugin->g;
    plugin->g = powf(10.0f, (*plugin->gain)/20.0f);

    if (fabsf(g_before - plugin->g) < FLT_EPSILON)
    {
#ifdef USE_NE10
        ne10_mulc_float_neon(plugin->out_1, plugin->in_1, g_before, n_samples);
        ne10_mulc_float_neon(plugin->out_2, plugin->in_2, g_before, n_samples);
#else
        for (uint32_t i=0; i<n_samples; i++)
        {
            plugin->out_1[i] = g_before * plugin->in_1[i];
            plugin->out_2[i] = g_before * plugin->in_2[i];
        }
#endif
    }
    else
    {
        const float g2 = ((plugin->g - g_before)/n_samples);

        for (uint32_t i=0; i<n_samples; i++)
        {
            plugin->out_1[i] = (g_before + g2 * i ) * plugin->in_1[i];
            plugin->out_2[i] = (g_before + g2 * i ) * plugin->in_2[i];
        }
    }
}

/**********************************************************************************************************************************************************/

void GainControl::cleanup(LV2_Handle instance)
{
    delete ((GainControl *) instance);
}
