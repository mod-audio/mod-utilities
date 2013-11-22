#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include "ModFilters.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/HighPassFilter"
#define TAMANHO_DO_BUFFER 1024
enum {IN, OUT_1, FREQ, ORDER, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class HighPassFilter
{
public:
    HighPassFilter() {}
    ~HighPassFilter() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out_1;
    float *freq;
    float *order;
    
    double *y;
    double *u;
    
    double T;
    double f;
    double y_1;
    double u_1;
    double y_2;
    double u_2;
    double y_3;
    double u_3;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    HighPassFilter::instantiate,
    HighPassFilter::connect_port,
    HighPassFilter::activate,
    HighPassFilter::run,
    HighPassFilter::deactivate,
    HighPassFilter::cleanup,
    HighPassFilter::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle HighPassFilter::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    HighPassFilter *plugin = new HighPassFilter();
    
    plugin->y = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    plugin->u = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    
    plugin->T = 1/samplerate;
    plugin->f = 1;
    plugin->y_1 = 0;
    plugin->u_1 = 0;
    plugin->y_2 = 0;
    plugin->u_2 = 0;
    plugin->y_3 = 0;
    plugin->u_3 = 0;
    	
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void HighPassFilter::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void HighPassFilter::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void HighPassFilter::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    HighPassFilter *plugin;
    plugin = (HighPassFilter *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case FREQ:
            plugin->freq = (float*) data;
            break;
        case ORDER:
            plugin->order = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void HighPassFilter::run(LV2_Handle instance, uint32_t n_samples)
{
    HighPassFilter *plugin;
    plugin = (HighPassFilter *) instance;    
    double f_before = plugin->f;
    float Order;
    
    plugin->f = (float)(*(plugin->freq));
    Order = (float)(*(plugin->order));
    Order = round(Order)+1;
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->u[i] = plugin->in[i];
	}
    
    switch (int(Order))
    {
        case 1:
			HPF1(plugin->u, plugin->y, n_samples, f_before, plugin->f, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3  );
            break;
        case 2:
			HPF2(plugin->u, plugin->y, n_samples, f_before, plugin->f, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3  );
            break;
        case 3:
			HPF3(plugin->u, plugin->y, n_samples, f_before, plugin->f, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3  );
            break;
        default:
			HPF1(plugin->u, plugin->y, n_samples, f_before, plugin->f, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->out_1[i] = plugin->y[i];
	}
    
}

/**********************************************************************************************************************************************************/

void HighPassFilter::cleanup(LV2_Handle instance)
{
	HighPassFilter *plugin;
	plugin = (HighPassFilter *) instance;
	
	free(plugin->u);
	free(plugin->y);
	
    delete ((HighPassFilter *) instance);
}

/**********************************************************************************************************************************************************/

const void* HighPassFilter::extension_data(const char* uri)
{
    return NULL;
}
