#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/SwitchTrigger4"
enum {IN, OUT_1, OUT_2, OUT_3, OUT_4, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4};

/**********************************************************************************************************************************************************/

class SwitchTrigger
{
public:
    SwitchTrigger() {}
    ~SwitchTrigger() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    int select_channel();
    float *in;
    float *out_1;
    float *out_2;
    float *out_3;
    float *out_4;
    float *channel1;
    float *channel2;
    float *channel3;
    float *channel4;
    int channel;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    SwitchTrigger::instantiate,
    SwitchTrigger::connect_port,
    SwitchTrigger::activate,
    SwitchTrigger::run,
    SwitchTrigger::deactivate,
    SwitchTrigger::cleanup,
    SwitchTrigger::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle SwitchTrigger::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    SwitchTrigger *plugin = new SwitchTrigger();
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void SwitchTrigger::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void SwitchTrigger::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void SwitchTrigger::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    SwitchTrigger *plugin;
    plugin = (SwitchTrigger *) instance;

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

int SwitchTrigger::select_channel() {
    if (*channel1 > 0) {
	channel = 0;
	*channel1 = 0;
    }
    if (*channel2 > 0) {
	channel = 1;
	*channel2 = 0;
    }
    if (*channel3 > 0) {
	channel = 2;
	*channel3 = 0;
    }
    if (*channel4 > 0) {
	channel = 3;
	*channel4 = 0;
    }
    return channel;
}

void SwitchTrigger::run(LV2_Handle instance, uint32_t n_samples)
{
    SwitchTrigger *plugin;
    plugin = (SwitchTrigger *) instance;
    /* float *pfOutput; */
    float *in = plugin->in;
    float *out_1 = plugin->out_1;
    float *out_2 = plugin->out_2;
    float *out_3 = plugin->out_3;
    float *out_4 = plugin->out_4;

    int channel = plugin->select_channel();

    switch (channel) 
	{
	case 0:
	    for ( uint32_t i = 0; i < n_samples; i++)
		{
		    *out_1 = *in;
		    *out_2=0;
		    *out_3=0;
		    *out_4=0;
		    in++;
		    out_1++;
		    out_2++;
		    out_3++;
		    out_4++;
		}
	    break;
	    
	case 1:
	    for (uint32_t i = 0; i < n_samples; i++)
		{
		    *out_1=0;
		    *out_2 = *in;
		    *out_3=0;
		    *out_4=0;
		    in++;
		    out_1++;
		    out_2++;
		    out_3++;
		    out_4++;
		}
	    break;
  
	case 2:
	    for (uint32_t i = 0; i < n_samples; i++)
		{
		    *out_1=0;
		    *out_2=0;
		    *out_3 = *in;
		    *out_4=0;
		    in++;
		    out_1++;
		    out_2++;
		    out_3++;
		    out_4++;
		}
	    break;

	case 3:
	    for (uint32_t i = 0; i < n_samples; i++)
		{
		    *out_1=0;
		    *out_2=0;
		    *out_3=0;
		    *out_4 = *in;
		    in++;
		    out_1++;
		    out_2++;
		    out_3++;
		    out_4++;
		}
	    break;

	}
}

/**********************************************************************************************************************************************************/

void SwitchTrigger::cleanup(LV2_Handle instance)
{
    delete ((SwitchTrigger *) instance);
}

/**********************************************************************************************************************************************************/

const void* SwitchTrigger::extension_data(const char* uri)
{
    return NULL;
}
