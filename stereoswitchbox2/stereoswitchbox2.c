#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/StereoSwitchBox3"
typedef enum {
	IN_L = 0,
	IN_R,
	OUT_1L,
	OUT_1R,
	OUT_2L,
	OUT_2R,
	CHANNEL
}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
	float *in_L;
	float *in_R;
	float *out_1L;
	float *out_1R;
	float *out_2L;
	float *out_2R;
	float *channel;

	float time_rate;
	float gain;
} Switchbox;
/**********************************************************************************************************************************************************/
static LV2_Handle
instantiate(const LV2_Descriptor*   descriptor,
double                              samplerate,
const char*                         bundle_path,
const LV2_Feature* const* features)
{
	Switchbox* switchbox = (Switchbox*)malloc(sizeof(Switchbox));

	float seconds  = 0.01;
	switchbox->time_rate = 1.0 / seconds / (float)samplerate;
	switchbox->gain = 1.0;

	return (LV2_Handle)switchbox;
}
/**********************************************************************************************************************************************************/
static void connect_port(LV2_Handle instance, uint32_t port, void *data)
{
	Switchbox* switchbox = (Switchbox*)instance;

	switch ((PortIndex)port)
	{
		case IN_L:
			switchbox->in_L = (float*) data;
			break;
		case IN_R:
			switchbox->in_R = (float*) data;
			break;
		case OUT_1L:
			switchbox->out_1L = (float*) data;
			break;
		case OUT_1R:
			switchbox->out_1R = (float*) data;
			break;
		case OUT_2L:
			switchbox->out_2L = (float*) data;
			break;
		case OUT_2R:
			switchbox->out_2R = (float*) data;
			break;
		case CHANNEL:
			switchbox->channel = (float*) data;
			break;
	}
}
/**********************************************************************************************************************************************************/
void activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/
void run(LV2_Handle instance, uint32_t n_samples)
{
	Switchbox* switchbox = (Switchbox*)instance;

	float *in_L = switchbox->in_L;
	float *in_R = switchbox->in_R;
	float *out_1L = switchbox->out_1L;
	float *out_1R = switchbox->out_1R;
	float *out_2L = switchbox->out_2L;
	float *out_2R = switchbox->out_2R;


	for ( uint32_t i = 0; i < n_samples; i++)
	{

		switch ((int)(*switchbox->channel)) {
			case 0:
				switchbox->gain = (switchbox->gain < 1.0) ? switchbox->gain + switchbox->time_rate : 1.0;
				break;
			case 1:
				switchbox->gain = (switchbox->gain > 0.0) ? switchbox->gain - switchbox->time_rate : 0.0;
				break;
		}

		float inv_gain = (switchbox->gain * -1) + 1;

		out_1L[i] = (in_L[i] * switchbox->gain);
		out_1R[i] = (in_R[i] * switchbox->gain);
		out_2L[i] = (in_L[i] * inv_gain);
		out_2R[i] = (in_R[i] * inv_gain);
	}
}

/**********************************************************************************************************************************************************/
void deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}
/**********************************************************************************************************************************************************/
void cleanup(LV2_Handle instance)
{
    free(instance);
}
/**********************************************************************************************************************************************************/
const void* extension_data(const char* uri)
{
    return NULL;
}
/**********************************************************************************************************************************************************/
static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};
/**********************************************************************************************************************************************************/
LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}
/**********************************************************************************************************************************************************/
