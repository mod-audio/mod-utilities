#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/inverted-Switchbox"
typedef enum {IN_1, IN_2, OUT, CHANNEL}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
    float *in_1;
    float *in_2;
	float *out;
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
        case IN_1:
            switchbox->in_1 = (float*) data;
            break;
        case IN_2:
            switchbox->in_2 = (float*) data;
            break;
		case OUT:
            switchbox->out = (float*) data;
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
    float *in_1 = switchbox->in_1;
    float *in_2 = switchbox->in_2;
    float *out = switchbox->out;

 switch (canal) {
  default:
   case 0:
    for ( uint32_t i = 0; i < n_samples; i++)
{
    *out = *in_1;
    *in_2=0;
    in_1++;
    in_2++;
    out++;
}



  break;

  case 1:

    for (uint32_t i = 0; i < n_samples; i++)
{
    *out = *in_2;
    *in_1=0;
    in_1++;
    in_2++;
    out++;
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
