#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/StereoSwitchBox3"
typedef enum {INL, INR, OUT_1L, OUT_1R, OUT_2L, OUT_2R, CHANNEL}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
    float *inL;
	float *inR;
    float *out_1L;
    float *out_1R;
	float *out_2L;
    float *out_2R;
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
        case INL:
            switchbox->inL = (float*) data;
            break;
		case INR:
			switchbox->inR = (float*) data;
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
    const Switchbox* switchbox = (const Switchbox*)instance;    

    int canal;
    canal = (int)(*(switchbox->channel));
    float *inL = switchbox->inL;
	float *inR = switchbox->inR;
    float *out_1L = switchbox->out_1L;
	float *out_1R = switchbox->out_1R;
    float *out_2L = switchbox->out_2L;
	float *out_2R = switchbox->out_2R;

 switch (canal) {
  default:
   case 0:
    for ( uint32_t i = 0; i < n_samples; i++)
{
    *out_1L = *inL;
	*out_1R = *inR;
    *out_2L=0;
	*out_2R=0;
    inL++;
	inR++;
    out_1L++;
	out_1R++;
    out_2L++;
	out_2R++;
}



  break;

  case 1:

    for (uint32_t i = 0; i < n_samples; i++)
{
    *out_2L = *inL;
	*out_2R = *inR;
    *out_1L=0;
	*out_1R=0;
    inL++;
	inR++;
    out_1L++;
	out_1R++;
    out_2L++;
	out_2R++;
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
