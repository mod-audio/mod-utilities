#include <cstdio>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include <string.h>

#ifdef __SSE__
/* On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
   flags to avoid costly denormals */
#ifdef __SSE3__
#include <pmmintrin.h>
inline void AVOIDDENORMALS()
{
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#else
#include <xmmintrin.h>
inline void AVOIDDENORMALS()
{
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}
#endif //__SSE3__

#else
inline void AVOIDDENORMALS() {}
#endif //__SSE__



#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/peakmeter"


// globals

int samplerate;
int meanperiod;
int peakperiod;

#define SECDIV 10 //number that divides 1 second to create mean period
#define MEANSIZE samplerate/SECDIV
#define todb(x) 20*log10(fabs(x))

enum {IN_L, IN_R, METEROUTL, METEROUTR, PKOUTL, PKOUTR};

class PeakmeterPlugin
{
public:
    PeakmeterPlugin() {}
    ~PeakmeterPlugin() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *inl;
    float *inr;
    float *meteroutl;
    float *meteroutr;
    float *peakoutl;
    float *peakoutr;

};

LV2_Handle PeakmeterPlugin::instantiate(const LV2_Descriptor* descriptor, double SampleRate, const char* bundle_path, const LV2_Feature* const* features)
{
    PeakmeterPlugin *plugin = new PeakmeterPlugin();

    samplerate = SampleRate;
    meanperiod = MEANSIZE;
    peakperiod = SECDIV*meanperiod;

    return (LV2_Handle)plugin;
}

void PeakmeterPlugin::activate(LV2_Handle instance)
{
    AVOIDDENORMALS();

}

void PeakmeterPlugin::deactivate(LV2_Handle instance)
{
}

void PeakmeterPlugin::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    PeakmeterPlugin *plugin;
    plugin = (PeakmeterPlugin *) instance;

    switch (port)
    {
    case IN_L:
        plugin->inl = (float*) data;
        break;
    case IN_R:
        plugin->inr = (float*) data;
        break;
    case METEROUTL:
        plugin->meteroutl = (float*) data;
        break;
    case METEROUTR:
        plugin->meteroutr = (float*) data;
        break;
    case PKOUTL:
        plugin->peakoutl = (float*) data;
        break;
    case PKOUTR:
        plugin->peakoutr = (float*) data;
        break;
    }
}

void PeakmeterPlugin::run(LV2_Handle instance, uint32_t SampleCount)
{
    PeakmeterPlugin *plugin;
    plugin = (PeakmeterPlugin *) instance;

    static int counter = 0; //measures samples counted until it reaches meanperiod
    static int counter2 = 0; //measures samples counted until it reaches peakperiod
    // static float mol = 0; //retains mean value of input until meanperiod is reached
    // static float mor = 0;
    static float pol = -60;//retains peak in a number of meanperiod samples
    static float por = -60;

    static float pol2 = -60; //retains peak in a number of samplecount samples
    static float por2 = -60;


    for(uint32_t j = 0; j < SampleCount; j++, counter++, counter2++) {

        // mol +=  (float) pow(plugin->inl[j],2);  //uncomment to use RMS instead of
        // mor +=  (float) pow(plugin->inr[j],2);  //Arithmetic mean
        // mol +=  (float) fabs(plugin->inl[j]);             //uncomment to use Arithmetic mean instead of
        // mor +=  (float) fabs(plugin->inr[j]);             //RMS

        if(todb(plugin->inl[j]) > pol){
            pol = todb(plugin->inl[j]);
        }
        if(todb(plugin->inr[j]) > por){
            por = todb(plugin->inr[j]);
        }
        if(todb(plugin->inl[j]) > pol2){
            pol2 = todb(plugin->inl[j]);
        }
        if(todb(plugin->inr[j]) > por2){
            por2 = todb(plugin->inr[j]);
        }
    }

    if(counter >= meanperiod - 1){

        // mol = mol/counter;
        // mor = mor/counter;

        // mol = sqrt(mol);    //uncomment to use RMS instead of
        // mor = sqrt(mor);    //Arithmetic mean

        // mol = todb(mol);
        // mor = todb(mor);

        // if(mol < -30)
        //     mol = -30;
        // else if(mol > 0)
        //     mol = 0;
        // if(mor < -30)
        //     mor = -30;
        // else if(mor > 0)
        //     mor = 0;

        if(pol2 < -30)
            pol2 = -30;
        else if(pol2 > 0)
            pol2 = 0;
        if(por2 < -30)
            por2 = -30;
        else if(por2 > 0)
            por2 = 0;

        *(plugin->meteroutl) = pol2;
        *(plugin->meteroutr) = por2;

        // *(plugin->meteroutl) = mol;
        // *(plugin->meteroutr) = mor;

        counter = 0;
        pol2 = -30;
        por2 = -30;
        // mol = 0;
        // mor = 0;

    }

    if(counter2 >= peakperiod - 1){

        if(pol < -30)
            pol = -30;
        else if(pol > 0)
            pol = 0;
        if(por < -30)
            por = -30;
        else if(por > 0)
            por = 0;

        *(plugin->peakoutl) = pol;
        *(plugin->peakoutr) = por;

        pol = pol2;
        por = por2;

        counter2 = 0;
    }


}


void PeakmeterPlugin::cleanup(LV2_Handle instance)
{
    delete ((PeakmeterPlugin *) instance);
}


const void* PeakmeterPlugin::extension_data(const char* uri)
{
    return NULL;
}

static const LV2_Descriptor Descriptor =
{
    PLUGIN_URI,
    PeakmeterPlugin::instantiate,
    PeakmeterPlugin::connect_port,
    PeakmeterPlugin::activate,
    PeakmeterPlugin::run,
    PeakmeterPlugin::deactivate,
    PeakmeterPlugin::cleanup,
    PeakmeterPlugin::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}
