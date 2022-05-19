#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <lv2.h>
#include<lv2/lv2plug.in/ns/ext/urid/urid.h>
#include<lv2/lv2plug.in/ns/ext/atom/util.h>
#include<lv2/lv2plug.in/ns/ext/atom/atom.h>
#include<lv2/lv2plug.in/ns/ext/state/state.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/SwitchTrigger4"
#define CHANNEL_URI "http://moddevices.com/plugins/mod-devel/SwitchTrigger4#channel"

enum {IN, OUT_1, OUT_2, OUT_3, OUT_4, CHANNEL1, CHANNEL2, CHANNEL3, CHANNEL4, CHANNEL_OUTPUT};

/**********************************************************************************************************************************************************/

static LV2_State_Status channel_save(LV2_Handle handle, LV2_State_Store_Function store, LV2_State_Handle state_handle, uint32_t flags, const LV2_Feature* const* features);
static LV2_State_Status channel_restore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve, LV2_State_Handle state_handle, uint32_t flags, const LV2_Feature* const* features);

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
    static LV2_State_Status channel_save(LV2_Handle handle, LV2_State_Store_Function store, LV2_State_Handle state_handle, uint32_t flags, const LV2_Feature* const* features);
    static LV2_State_Status channel_restore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve, LV2_State_Handle state_handle, uint32_t flags, const LV2_Feature* const* features);
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
    float *channel_output;
    int channel;

    LV2_URID_Map *urid_map;

    struct urids
    {
        LV2_URID    atom_Int;
        LV2_URID    switch_channel;
    } URIDs;

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

    for(int i=0; features[i]; i++)
    {
        if(!strcmp(features[i]->URI,LV2_URID__map))
        {
            plugin->urid_map = (LV2_URID_Map *) features[i]->data;
            if(plugin->urid_map)
            {
                plugin->URIDs.atom_Int = plugin->urid_map->map(plugin->urid_map->handle,LV2_ATOM__Int);
                plugin->URIDs.switch_channel = plugin->urid_map->map(plugin->urid_map->handle,CHANNEL_URI);
                break;
            }
        }
    }

    plugin->channel = 0;

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
        case CHANNEL_OUTPUT:
            plugin->channel_output = (float*) data;
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
        memcpy(out_1, in, sizeof(float)*n_samples);
        memset(out_2, 0, sizeof(float)*n_samples);
        memset(out_3, 0, sizeof(float)*n_samples);
        memset(out_4, 0, sizeof(float)*n_samples);
	    break;
	    
	case 1:
        memcpy(out_2, in, sizeof(float)*n_samples);
        memset(out_1, 0, sizeof(float)*n_samples);
        memset(out_3, 0, sizeof(float)*n_samples);
        memset(out_4, 0, sizeof(float)*n_samples);
	    break;
  
	case 2:
        memcpy(out_3, in, sizeof(float)*n_samples);
        memset(out_1, 0, sizeof(float)*n_samples);
        memset(out_2, 0, sizeof(float)*n_samples);
        memset(out_4, 0, sizeof(float)*n_samples);
	    break;

	case 3:
        memcpy(out_4, in, sizeof(float)*n_samples);
        memset(out_1, 0, sizeof(float)*n_samples);
        memset(out_2, 0, sizeof(float)*n_samples);
        memset(out_3, 0, sizeof(float)*n_samples);
	    break;
	}

    plugin->channel_output = (float*)&channel;
}

/**********************************************************************************************************************************************************/

void SwitchTrigger::cleanup(LV2_Handle instance)
{
    delete ((SwitchTrigger *) instance);
}

/**********************************************************************************************************************************************************/

LV2_State_Status SwitchTrigger::channel_save(LV2_Handle handle, LV2_State_Store_Function store, LV2_State_Handle state_handle,
        uint32_t flags, const LV2_Feature* const* features)
{
    SwitchTrigger *plugin = (SwitchTrigger*)handle;

    void *body = &plugin->channel;
    store(state_handle, plugin->URIDs.switch_channel, body, sizeof(int),
           plugin->URIDs.atom_Int, LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE);

    return LV2_STATE_SUCCESS;
}

/**********************************************************************************************************************************************************/

LV2_State_Status SwitchTrigger::channel_restore(LV2_Handle handle, LV2_State_Retrieve_Function retrieve, LV2_State_Handle state_handle,
        uint32_t flags, const LV2_Feature* const* features)
{
    SwitchTrigger *plugin = (SwitchTrigger*)handle;

    size_t   size;
    uint32_t type;
    uint32_t valflags;

    const void* value = retrieve( state_handle, plugin->URIDs.switch_channel, &size, &type, &valflags);

    if ((value) && (size == sizeof(int)) && (type == plugin->URIDs.switch_channel))
    {
        plugin->channel = *((int*)(&value));
    }

    return LV2_STATE_SUCCESS;
}

/**********************************************************************************************************************************************************/

const void* SwitchTrigger::extension_data(const char* uri)
{
    static const LV2_State_Interface state = { channel_save, channel_restore };

    if (!strcmp(uri, LV2_STATE__interface))
    {
        return &state;
    }

    return NULL;
}
