#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include "ModFilters.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/CrossOver2"
#include "BufferSize.h"

#define BUFFER_SIZE_FALLBACK 8192 /* when the host does not report bufsz:maxBlockLength */
enum {IN, OUT_1, OUT_2, FREQ, GAIN_1, GAIN_2, ORDER, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class CrossOver
{
public:
    CrossOver() {}
    ~CrossOver() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *in;
    float *out_1;
    float *out_2;
    float *freq;
    float *gain_1;
    float *gain_2;
    float *order;
    
    double *y1;
    double *y2;
    double *u;
    uint32_t capacity; /* samples per scratch buffer, see BufferSize.h */
    
    
    double T;
    double f;
    
    double y1_1;
    double y2_1;
    double u1_1;
    double u2_1;
    
    double y1_2;
    double y2_2;
    double u1_2;
    double u2_2;
    
    double y1_3;
    double y2_3;
    double u1_3;
    double u2_3;
    
    float g_1;
    float g_2;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    CrossOver::instantiate,
    CrossOver::connect_port,
    CrossOver::activate,
    CrossOver::run,
    CrossOver::deactivate,
    CrossOver::cleanup,
    CrossOver::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle CrossOver::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    CrossOver *plugin = new CrossOver();
    
    plugin->capacity = mod_block_capacity(features, BUFFER_SIZE_FALLBACK);
    plugin->y1 = (double*)malloc(plugin->capacity*sizeof(double));
    plugin->y2 = (double*)malloc(plugin->capacity*sizeof(double));
    plugin->u = (double*)malloc(plugin->capacity*sizeof(double));
    
    plugin->T = 1/samplerate;
    plugin->f = 1;
    
    plugin->y1_1 = 0;
    plugin->y2_1 = 0;
    plugin->u1_1 = 0;
    plugin->u2_1 = 0;
    
    plugin->y1_2 = 0;
    plugin->y2_2 = 0;
    plugin->u1_2 = 0;
    plugin->u2_2 = 0;
    
    plugin->y1_3 = 0;
    plugin->y2_3 = 0;
    plugin->u1_3 = 0;
    plugin->u2_3 = 0;
    
    plugin->g_1 = 1;
    plugin->g_2 = 1;
    
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void CrossOver::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void CrossOver::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void CrossOver::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    CrossOver *plugin;
    plugin = (CrossOver *) instance;

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
        case FREQ:
            plugin->freq = (float*) data;
            break;
        case GAIN_1:
            plugin->gain_1 = (float*) data;
            break;
        case GAIN_2:
            plugin->gain_2 = (float*) data;
            break;
        case ORDER:
            plugin->order = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

static void process(CrossOver *plugin, const float *in, float *out_1, float *out_2, uint32_t n_samples)
{
    double f_before = plugin->f;
    float Order;
    
    plugin->f = (float)(*(plugin->freq));
    Order = (float)(*(plugin->order));
    Order = round(Order)+1;
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->u[i] = in[i];
	}
	
	/*********************************************************/	
    
    switch (int(Order))
    {
        case 1:
			LPF1(plugin->u, plugin->y1, n_samples, f_before, plugin->f, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        case 2:
			LPF2(plugin->u, plugin->y1, n_samples, f_before, plugin->f, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        case 3:
			LPF3(plugin->u, plugin->y1, n_samples, f_before, plugin->f, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        default:
			LPF1(plugin->u, plugin->y1, n_samples, f_before, plugin->f, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		out_1[i] = plugin->y1[i];
	}
	
	/*********************************************************/
       
    double g_before_1 = plugin->g_1;
    plugin->g_1 = pow(10, (float)(*(plugin->gain_1))/20.0);
    
    for (uint32_t i=1; i<=n_samples; i++)
	{
		out_1[i-1] = (g_before_1 + ((plugin->g_1 - g_before_1)/(n_samples > 1 ? n_samples - 1 : 1))*(i-1) )*(float)out_1[i-1];
	}
	
	
	/*********************************************************//*********************************************************/
    
    switch (int(Order))
    {
        case 1:
			HPF1(plugin->u, plugin->y2, n_samples, f_before, plugin->f, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3  );
            break;
        case 2:
			HPF2(plugin->u, plugin->y2, n_samples, f_before, plugin->f, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3  );
            break;
        case 3:
			HPF3(plugin->u, plugin->y2, n_samples, f_before, plugin->f, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3  );
            break;
        default:
			HPF1(plugin->u, plugin->y2, n_samples, f_before, plugin->f, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		out_2[i] = plugin->y2[i];
	}
	
	/*********************************************************/
       
    double g_before_2 = plugin->g_2;
    plugin->g_2 = pow(10, (float)(*(plugin->gain_2))/20.0);
    
    for (uint32_t i=1; i<=n_samples; i++)
	{
		out_2[i-1] = (g_before_2 + ((plugin->g_2 - g_before_2)/(n_samples > 1 ? n_samples - 1 : 1))*(i-1) )*(float)out_2[i-1];
	} 
	

}

void CrossOver::run(LV2_Handle instance, uint32_t n_samples)
{
    CrossOver *plugin;
    plugin = (CrossOver *) instance;

    /* the scratch buffers hold `capacity` samples; larger blocks are processed in chunks */
    for (uint32_t offset = 0; offset < n_samples; )
    {
        const uint32_t n = mod_next_chunk(n_samples - offset, plugin->capacity);
        process(plugin, plugin->in + offset, plugin->out_1 + offset, plugin->out_2 + offset, n);
        offset += n;
    }
}

/**********************************************************************************************************************************************************/

void CrossOver::cleanup(LV2_Handle instance)
{
	CrossOver *plugin;
	plugin = (CrossOver *) instance;
	
	free(plugin->u);
	free(plugin->y1);
	free(plugin->y2);
	
    delete ((CrossOver *) instance);
}

/**********************************************************************************************************************************************************/

const void* CrossOver::extension_data(const char* uri)
{
    return NULL;
}
