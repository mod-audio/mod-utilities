#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lv2.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/LoopSwitch4"
enum {IN, RET1, RET2, RET3, RET4, OUT, SND1, SND2, SND3, SND4, LOOP1, LOOP2, LOOP3, LOOP4};
#define N_LOOPS 4

/**********************************************************************************************************************************************************/

class LoopSwitch
{
public:
    LoopSwitch() {}
    ~LoopSwitch() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *ret1;
    float *ret2;
    float *ret3;
    float *ret4;
    float *out;
    float *snd1;
    float *snd2;
    float *snd3;
    float *snd4;
    float *loop1;
    float *loop2;
    float *loop3;
    float *loop4;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    LoopSwitch::instantiate,
    LoopSwitch::connect_port,
    LoopSwitch::activate,
    LoopSwitch::run,
    LoopSwitch::deactivate,
    LoopSwitch::cleanup,
    LoopSwitch::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle LoopSwitch::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    LoopSwitch *plugin = new LoopSwitch();
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void LoopSwitch::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void LoopSwitch::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void LoopSwitch::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    LoopSwitch *plugin;
    plugin = (LoopSwitch *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case RET1:
            plugin->ret1 = (float*) data;
            break;
        case RET2:
            plugin->ret2 = (float*) data;
            break;
        case RET3:
            plugin->ret3 = (float*) data;
            break;
        case RET4:
            plugin->ret4 = (float*) data;
            break;
        case OUT:
            plugin->out = (float*) data;
            break;
        case SND1:
            plugin->snd1 = (float*) data;
            break;
        case SND2:
            plugin->snd2 = (float*) data;
            break;
        case SND3:
            plugin->snd3 = (float*) data;
            break;
        case SND4:
            plugin->snd4 = (float*) data;
            break;
        case LOOP1:
            plugin->loop1 = (float*) data;
            break;
        case LOOP2:
            plugin->loop2 = (float*) data;
            break;
        case LOOP3:
            plugin->loop3 = (float*) data;
            break;
        case LOOP4:
            plugin->loop4 = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void LoopSwitch::run(LV2_Handle instance, uint32_t n_samples)
{
    LoopSwitch *plugin;
    plugin = (LoopSwitch *) instance;

    float *in   = plugin-> in;
    float *ret1 = plugin-> ret1;
    float *ret2 = plugin-> ret2;
    float *ret3 = plugin-> ret3;
    float *ret4 = plugin-> ret4;
    float *out  = plugin-> out;
    float *snd1 = plugin-> snd1;
    float *snd2 = plugin-> snd2;
    float *snd3 = plugin-> snd3;
    float *snd4 = plugin-> snd4;

    bool loop1  = *plugin-> loop1 >0.5f;
    bool loop2  = *plugin-> loop2 >0.5f;
    bool loop3  = *plugin-> loop3 >0.5f;
    bool loop4  = *plugin-> loop4 >0.5f;

	bool mults[N_LOOPS + 1][N_LOOPS + 1] = {
		{loop1} ,
		{loop1*loop2, !loop1*loop2} ,
		{loop2*loop3, loop1*!loop2*loop3, !loop1*!loop2*loop3} ,
		{loop3*loop4, loop2*!loop3*loop4, loop1*!loop2*!loop3*loop4, !loop1*!loop2*!loop3*loop4} ,
		{loop4, loop3*!loop4, loop2*!loop3*!loop4, loop1*!loop2*!loop3*!loop4, !loop1*!loop2*!loop3*!loop4}
	};    

	for (uint32_t i=0; i < n_samples; i++)
	{
		snd1[i] = (mults[0][0]*in[i]);
        snd2[i] = (mults[1][0]*ret1[i]) + (mults[1][1]*in[i]);
   		snd3[i] = (mults[2][0]*ret2[i]) + (mults[2][1]*ret1[i]) + (mults[2][2]*in[i]);
    	snd4[i] = (mults[3][0]*ret3[i]) + (mults[3][1]*ret2[i]) + (mults[3][2]*ret1[i]) + (mults[3][3]*in[i]);
        out[i]  = (mults[4][0]*ret4[i]) + (mults[4][1]*ret3[i]) + (mults[4][2]*ret2[i]) + (mults[4][3]*ret1[i]) + (mults[4][4]*in[i]);
	}
	
}

/**********************************************************************************************************************************************************/

void LoopSwitch::cleanup(LV2_Handle instance)
{
    delete ((LoopSwitch *) instance);
}

/**********************************************************************************************************************************************************/

const void* LoopSwitch::extension_data(const char* uri)
{
    return NULL;
}
