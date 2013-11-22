#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include "ModFilters.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/BandPassFilter"
#define TAMANHO_DO_BUFFER 1024
enum {IN, OUT_1, FREQ, Q, ORDER, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class BandPassFilter
{
public:
    BandPassFilter() {}
    ~BandPassFilter() {}
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
    float *q_factor;
    float *order;
    
    double *y;
    double *u;
    
    double T;
    double f;
    double q;
    double bw;
    double y_1;
    double u_1;
    double y_2;
    double u_2;
    double y_3;
    double u_3;
    double y_4;
    double u_4;
    double y_5;
    double u_5;
    double y_6;
    double u_6;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    BandPassFilter::instantiate,
    BandPassFilter::connect_port,
    BandPassFilter::activate,
    BandPassFilter::run,
    BandPassFilter::deactivate,
    BandPassFilter::cleanup,
    BandPassFilter::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle BandPassFilter::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    BandPassFilter *plugin = new BandPassFilter();
    
    plugin->y = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    plugin->u = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    
    plugin->T = 1/samplerate;
    plugin->f = 1;
    plugin->q = 0.5;
    plugin->bw = 2;
    plugin->y_1 = 0;
    plugin->u_1 = 0;
    plugin->y_2 = 0;
    plugin->u_2 = 0;
    plugin->y_3 = 0;
    plugin->u_3 = 0;
    plugin->y_4 = 0;
    plugin->u_4 = 0;
    plugin->y_5 = 0;
    plugin->u_5 = 0;
    plugin->y_6 = 0;
    plugin->u_6 = 0;
    	
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void BandPassFilter::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void BandPassFilter::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void BandPassFilter::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    BandPassFilter *plugin;
    plugin = (BandPassFilter *) instance;

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
        case Q:
            plugin->q_factor = (float*) data;
            break;
        case ORDER:
            plugin->order = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void BandPassFilter::run(LV2_Handle instance, uint32_t n_samples)
{
    BandPassFilter *plugin;
    plugin = (BandPassFilter *) instance;    
    double f_before = plugin->f;
    double bw_before = plugin->bw;
    float Order;
    
    plugin->f = (float)(*(plugin->freq));
    plugin->q = (float)(*(plugin->q_factor));
    plugin->bw = plugin->f/plugin->q;
    Order = (float)(*(plugin->order));
    Order = round(Order)+1;
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->u[i] = plugin->in[i];
	}
    
    switch (int(Order))
    {
        case 1:
			BPF1(plugin->u, plugin->y, n_samples, f_before, plugin->f, bw_before, plugin->bw, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3, &plugin->y_4, &plugin->u_4, &plugin->y_5, &plugin->u_5, &plugin->y_6, &plugin->u_6  );
            break;
        case 2:
			BPF2(plugin->u, plugin->y, n_samples, f_before, plugin->f, bw_before, plugin->bw, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3, &plugin->y_4, &plugin->u_4, &plugin->y_5, &plugin->u_5, &plugin->y_6, &plugin->u_6  );
            break;
        case 3:
			BPF3(plugin->u, plugin->y, n_samples, f_before, plugin->f, bw_before, plugin->bw, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3, &plugin->y_4, &plugin->u_4, &plugin->y_5, &plugin->u_5, &plugin->y_6, &plugin->u_6  );
            break;
        default:
			BPF1(plugin->u, plugin->y, n_samples, f_before, plugin->f, bw_before, plugin->bw, plugin->T, &plugin->y_1, &plugin->u_1, &plugin->y_2, &plugin->u_2, &plugin->y_3, &plugin->u_3, &plugin->y_4, &plugin->u_4, &plugin->y_5, &plugin->u_5, &plugin->y_6, &plugin->u_6  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->out_1[i] = plugin->y[i];
	}
}

/**********************************************************************************************************************************************************/

void BandPassFilter::cleanup(LV2_Handle instance)
{
	BandPassFilter *plugin;
	plugin = (BandPassFilter *) instance;
	
	free(plugin->u);
	free(plugin->y);
	
    delete ((BandPassFilter *) instance);
}

/**********************************************************************************************************************************************************/

const void* BandPassFilter::extension_data(const char* uri)
{
    return NULL;
}
