#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/StereoSwitchBox2"
typedef enum {IN_1L, IN_1R, IN_2L, IN_2R, OUTL, OUTR, CHANNEL}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
    float *in_1L;
	float *in_1R;
    float *in_2L;
    float *in_2R;
	float *outL;
    float *outR;
    float *channel;
} Switchbox;
/**********************************************************************************************************************************************************/
static LV2_Handle
instantiate(const LV2_Descriptor*   descriptor,
double                              samplerate,
const char*                         bundle_path,
const LV2_Feature* const* features)
{
    Switchbox* switchbox = (Switchbox*)malloc(sizeof(Switchbox));
    return (LV2_Handle)switchbox;
}
/**********************************************************************************************************************************************************/
static void connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Switchbox* switchbox = (Switchbox*)instance;

    switch ((PortIndex)port)
    {
        case IN_1L:
            switchbox->in_1L = (float*) data;
            break;
		case IN_1R:
			switchbox->in_1R = (float*) data;
			break;
        case IN_2L:
            switchbox->in_2L = (float*) data;
            break;
		case IN_2R:
            switchbox->in_2R = (float*) data;
            break;
        case OUTL:
            switchbox->outL = (float*) data;
            break;
		case OUTR:
            switchbox->outR = (float*) data;
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
    const Switchbox* switchbox = (const Switchbox*)instance;    

    int canal;
    canal = (int)(*(switchbox->channel));
    float *in_1L = switchbox->in_1L;
	float *in_1R = switchbox->in_1R;
    float *in_2L = switchbox->in_2L;
	float *in_2R = switchbox->in_2R;
    float *outL = switchbox->outL;
	float *outR = switchbox->outR;

 switch (canal) {
  default:
   case 0:
    for ( uint32_t i = 0; i < n_samples; i++)
{
    *outL = *in_1L;
	*outR = *in_1R;
    *in_2L=0;
	*in_2R=0;
    in_1L++;
	in_1R++;
    in_2L++;
	in_2R++;
    outL++;
	outR++;
}



  break;

  case 1:

    for (uint32_t i = 0; i < n_samples; i++)
{
    *outL = *in_2L;
	*outR = *in_2R;
    *in_1L=0;
	*in_1R=0;
    in_1L++;
	in_1R++;
    in_2L++;
	in_2R++;
    outL++;
	outR++;
}


break;

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
