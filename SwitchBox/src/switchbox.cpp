#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/SwitchBox2"
enum {IN, OUT_1, OUT_2, CHANNEL, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class SwitchBox
{
public:
    SwitchBox() {}
    ~SwitchBox() {}
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
    float *channel;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    SwitchBox::instantiate,
    SwitchBox::connect_port,
    SwitchBox::activate,
    SwitchBox::run,
    SwitchBox::deactivate,
    SwitchBox::cleanup,
    SwitchBox::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle SwitchBox::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    SwitchBox *plugin = new SwitchBox();
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void SwitchBox::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void SwitchBox::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void SwitchBox::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    SwitchBox *plugin;
    plugin = (SwitchBox *) instance;

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
        case CHANNEL:
            plugin->channel = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void SwitchBox::run(LV2_Handle instance, uint32_t n_samples)
{
    SwitchBox *plugin;
    plugin = (SwitchBox *) instance;
    /* float *pfOutput; */
    int canal;
    canal = (int)(*(plugin->channel));
    float *in = plugin->in;
    float *out_1 = plugin->out_1;
    float *out_2 = plugin->out_2;

 switch (canal) {
  default:
   case 0:
    for ( uint32_t i = 0; i < n_samples; i++)
{
    *out_1 = *in;
    *out_2=0;
    in++;
    out_1++;
    out_2++;
}



  break;

  case 1:

    for (uint32_t i = 0; i < n_samples; i++)
{
    *out_2 = *in;
    *out_1=0;
    in++;
    out_1++;
    out_2++;

}


break;

  }

}

/**********************************************************************************************************************************************************/

void SwitchBox::cleanup(LV2_Handle instance)
{
    delete ((SwitchBox *) instance);
}

/**********************************************************************************************************************************************************/

const void* SwitchBox::extension_data(const char* uri)
{
    return NULL;
}
