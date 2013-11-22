#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include "ModFilters.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/CrossOver2"
#define TAMANHO_DO_BUFFER 1024
enum {IN, OUT_1, OUT_2, OUT_3, FREQ_1, FREQ_2, GAIN_1, GAIN_2, GAIN_3, ORDER, PLUGIN_PORT_COUNT};

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
    float *out_3;
    float *freq_1;
    float *freq_2;
    float *gain_1;
    float *gain_2;
    float *gain_3;
    float *order;
    
    double *y1;
    double *y2;
    double *y3;
    double *u;
    
    
    double T;
    double f_1;
    double f_2;
    
    double y1_1;
    double y2_1;
    double y3_1;
    double u1_1;
    double u2_1;
    double u3_1;
    
    double y1_2;
    double y2_2;
    double y3_2;
    double u1_2;
    double u2_2;
    double u3_2;
    
    double y1_3;
    double y2_3;
    double y3_3;
    double u1_3;
    double u2_3;
    double u3_3;
    
    double y2_4;
    double u2_4;
    
    double y2_5;
    double u2_5;
    
    double y2_6;
    double u2_6;
    
    
    float g_1;
    float g_2;
    float g_3;
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
    
    plugin->y1 = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    plugin->y2 = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    plugin->y3 = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    plugin->u = (double*)malloc(TAMANHO_DO_BUFFER*sizeof(double));
    
    plugin->T = 1/samplerate;
    plugin->f_1 = 1;
    plugin->f_2 = 1;
    
    plugin->y1_1 = 0;
    plugin->y2_1 = 0;
    plugin->y3_1 = 0;
    plugin->u1_1 = 0;
    plugin->u2_1 = 0;
    plugin->u3_1 = 0;
    
    plugin->y1_2 = 0;
    plugin->y2_2 = 0;
    plugin->y3_2 = 0;
    plugin->u1_2 = 0;
    plugin->u2_2 = 0;
    plugin->u3_2 = 0;
    
    plugin->y1_3 = 0;
    plugin->y2_3 = 0;
    plugin->y3_3 = 0;
    plugin->u1_3 = 0;
    plugin->u2_3 = 0;
    plugin->u3_3 = 0;
    
    plugin->y2_4 = 0;
    plugin->u2_4 = 0;
    
    plugin->y2_5 = 0;
    plugin->u2_5 = 0;
    
    plugin->y2_6 = 0;
    plugin->u2_6 = 0;
    
    plugin->g_1 = 1;
    plugin->g_2 = 1;
    plugin->g_3 = 1;
    
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
        case OUT_3:
            plugin->out_3 = (float*) data;
            break;
        case FREQ_1:
            plugin->freq_1 = (float*) data;
            break;
        case FREQ_2:
            plugin->freq_2 = (float*) data;
            break;
        case GAIN_1:
            plugin->gain_1 = (float*) data;
            break;
        case GAIN_2:
            plugin->gain_2 = (float*) data;
            break;
        case GAIN_3:
            plugin->gain_3 = (float*) data;
            break;
        case ORDER:
            plugin->order = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void CrossOver::run(LV2_Handle instance, uint32_t n_samples)
{
    CrossOver *plugin;
    plugin = (CrossOver *) instance;    
    double f_before_1 = plugin->f_1;
    double f_before_2 = plugin->f_2;
    float Order;
    
    plugin->f_1 = (float)(*(plugin->freq_1));
    plugin->f_2 = (float)(*(plugin->freq_2));
    
    double fc = sqrt( plugin->f_1 * plugin->f_2);
    double fc_before = sqrt( f_before_1 * f_before_2);
    double BW = abs(plugin->f_2 - plugin->f_1);
    double BW_before = abs(f_before_2 - f_before_1);
    
    Order = (float)(*(plugin->order));
    Order = round(Order)+1;
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->u[i] = plugin->in[i];
	}
	
	/*********************************************************/	
    
    switch (int(Order))
    {
        case 1:
			LPF1(plugin->u, plugin->y1, n_samples, f_before_1, plugin->f_1, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        case 2:
			LPF2(plugin->u, plugin->y1, n_samples, f_before_1, plugin->f_1, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        case 3:
			LPF3(plugin->u, plugin->y1, n_samples, f_before_1, plugin->f_1, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
            break;
        default:
			LPF1(plugin->u, plugin->y1, n_samples, f_before_1, plugin->f_1, plugin->T, &plugin->y1_1, &plugin->u1_1, &plugin->y1_2, &plugin->u1_2, &plugin->y1_3, &plugin->u1_3  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->out_1[i] = plugin->y1[i];
	}
	
	/*********************************************************/
       
    double g_before_1 = plugin->g_1;
    plugin->g_1 = pow(10, (float)(*(plugin->gain_1))/20.0);
    
    for (uint32_t i=1; i<=n_samples; i++)
	{
		plugin->out_1[i-1] = (g_before_1 + ((plugin->g_1 - g_before_1)/(n_samples - 1))*(i-1) )*(float)plugin->out_1[i-1];
	}
	
	
	/*********************************************************//*********************************************************/
    
    switch (int(Order))
    {
        case 1:
			BPF1(plugin->u, plugin->y2, n_samples, fc_before, fc, BW_before, BW, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3, &plugin->y2_4, &plugin->u2_4, &plugin->y2_5, &plugin->u2_5, &plugin->y2_6, &plugin->u2_6  );
            break;
        case 2:
			BPF2(plugin->u, plugin->y2, n_samples, fc_before, fc, BW_before, BW, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3, &plugin->y2_4, &plugin->u2_4, &plugin->y2_5, &plugin->u2_5, &plugin->y2_6, &plugin->u2_6  );
            break;
        case 3:
			BPF3(plugin->u, plugin->y2, n_samples, fc_before, fc, BW_before, BW, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3, &plugin->y2_4, &plugin->u2_4, &plugin->y2_5, &plugin->u2_5, &plugin->y2_6, &plugin->u2_6  );
            break;
        default:
			BPF1(plugin->u, plugin->y2, n_samples, fc_before, fc, BW_before, BW, plugin->T, &plugin->y2_1, &plugin->u2_1, &plugin->y2_2, &plugin->u2_2, &plugin->y2_3, &plugin->u2_3, &plugin->y2_4, &plugin->u2_4, &plugin->y2_5, &plugin->u2_5, &plugin->y2_6, &plugin->u2_6  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->out_2[i] = plugin->y2[i];
	}
	
	/*********************************************************/
       
    double g_before_2 = plugin->g_2;
    plugin->g_2 = pow(10, (float)(*(plugin->gain_2))/20.0);
    
    for (uint32_t i=1; i<=n_samples; i++)
	{
		plugin->out_2[i-1] = (g_before_2 + ((plugin->g_2 - g_before_2)/(n_samples - 1))*(i-1) )*(float)plugin->out_2[i-1];
	} 
	
	/*********************************************************//*********************************************************/
    
    switch (int(Order))
    {
        case 1:
			HPF1(plugin->u, plugin->y3, n_samples, f_before_2, plugin->f_2, plugin->T, &plugin->y3_1, &plugin->u3_1, &plugin->y3_2, &plugin->u3_2, &plugin->y3_3, &plugin->u3_3  );
            break;
        case 2:
			HPF2(plugin->u, plugin->y3, n_samples, f_before_2, plugin->f_2, plugin->T, &plugin->y3_1, &plugin->u3_1, &plugin->y3_2, &plugin->u3_2, &plugin->y3_3, &plugin->u3_3  );
            break;
        case 3:
			HPF3(plugin->u, plugin->y3, n_samples, f_before_2, plugin->f_2, plugin->T, &plugin->y3_1, &plugin->u3_1, &plugin->y3_2, &plugin->u3_2, &plugin->y3_3, &plugin->u3_3  );
            break;
        default:
			HPF1(plugin->u, plugin->y3, n_samples, f_before_2, plugin->f_2, plugin->T, &plugin->y3_1, &plugin->u3_1, &plugin->y3_2, &plugin->u3_2, &plugin->y3_3, &plugin->u3_3  );
    }
    
    for (uint32_t i=0; i<= n_samples-1;i++)
    {
		plugin->out_3[i] = plugin->y3[i];
	}
	
	/*********************************************************/
       
    double g_before_3 = plugin->g_3;
    plugin->g_3 = pow(10, (float)(*(plugin->gain_3))/20.0);
    
    for (uint32_t i=1; i<=n_samples; i++)
	{
		plugin->out_3[i-1] = (g_before_3 + ((plugin->g_3 - g_before_3)/(n_samples - 1))*(i-1) )*(float)plugin->out_3[i-1];
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
	free(plugin->y3);
	
    delete ((CrossOver *) instance);
}

/**********************************************************************************************************************************************************/

const void* CrossOver::extension_data(const char* uri)
{
    return NULL;
}
