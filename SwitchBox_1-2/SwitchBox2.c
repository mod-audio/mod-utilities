#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/switchbox_1-2"
typedef enum {
	IN = 0,
	OUT_1,
	OUT_2,
	CHANNEL
}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
	float *in;
	float *out_1;
	float *out_2;
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
		case IN:
			switchbox->in = (float*) data;
			break;
		case OUT_1:
			switchbox->out_1 = (float*) data;
			break;
		case OUT_2:
			switchbox->out_2 = (float*) data;
			break;
		case CHANNEL:
			switchbox->channel = (float*) data;
			break;
	}
}
/**********************************************************************************************************************************************************/
void activate(LV2_Handle instance)
{
}

/**********************************************************************************************************************************************************/
void run(LV2_Handle instance, uint32_t n_samples)
{
	Switchbox* switchbox = (Switchbox*)instance;

	float *in = switchbox->in;
	float *out_1 = switchbox->out_1;
	float *out_2 = switchbox->out_2;


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

		out_1[i] = (in[i] * switchbox->gain);
		out_2[i] = (in[i] * inv_gain);
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
