#include <math.h>
#include <lv2.h>
#include <lv2/atom.lv2/util.h>
#include <lv2/midi.lv2/midi.h>
#include <lv2/patch.lv2/patch.h>
#include <lv2/state.lv2/state.h>
#include <lv2/urid.lv2/urid.h>
#include <lv2/atom.lv2/atom.h>

#include <aubio.h>
#include <aubio/fvec.h>
#include <aubio/mathutils.h>
#include <aubio/musicutils.h>
#include <aubio/onset/onset.h>
#include <aubio/pitch/pitch.h>
#include <stdio.h>
#include <stdlib.h>


#define PARAM_BUFFER_SIZE       256 
#define PARAM_PITCH_BUFF_TIMES  4

#define PARAM_HOP_SIZE_ONSET    256
#define PARAM_HOP_SIZE_PITCH    256

#define PLUGIN_URI "http://moddevices.com/plugins/MOD/note2midi"

/*
************************************************************************************************************************
*           LV2 Functions
************************************************************************************************************************
*/

static inline LV2_Atom_Event* lv2_atom_sequence_append_event(LV2_Atom_Sequence* seq, uint32_t capacity, const LV2_Atom_Event* event) { 
    const uint32_t total_size = (uint32_t)sizeof(*event) + event->body.size; 

    if (capacity - seq->atom.size < total_size) { 
        return NULL; 
    } 

    LV2_Atom_Event* e = lv2_atom_sequence_end(&seq->body, seq->atom.size); 
    memcpy(e, event, total_size); 

    seq->atom.size += lv2_atom_pad_size(total_size); 
    return e; 
}

static inline void 
lv2_atom_sequence_clear(LV2_Atom_Sequence* seq) 
{ 
        seq->atom.size = sizeof(LV2_Atom_Sequence_Body); 
} 

/*
************************************************************************************************************************
*           
************************************************************************************************************************
*/

enum {IN, OUT, ONSET_METHOD, ONSET_THRESHOLD, PITCH_METHOD, PITCH_TOLERANCE, SILENCE_THRESHOLD};

enum {  ONSET_METHOD_ENERGY,
        ONSET_METHOD_SPECDIFF,
        ONSET_METHOD_HFC,
        ONSET_METHOD_COMPLEXDOMAIN,
        ONSET_METHOD_COMPLEX,
        ONSET_METHOD_PHASE,
        ONSET_METHOD_MKL,
        ONSET_METHOD_KL,
        ONSET_METHOD_SPECFLUX,
        ONSET_METHOD_CENTROID,
        ONSET_METHOD_SPREAD,
        ONSET_METHOD_SKEWNESS,
        ONSET_METHOD_KURTOSIS,
        ONSET_METHOD_SLOPE,
        ONSET_METHOD_DECREASE,
        ONSET_METHOD_ROLLOFF,
        ONSET_METHOD_DEFAULT};


enum {  PITCH_METHOD_MCOMB,
        PITCH_METHOD_YINFFT,
        PITCH_METHOD_YIN,
        PITCH_METHOD_SCHMITT,
        PITCH_METHOD_FCOMB,
        PITCH_METHOD_SPECACF,
        PITCH_METHOD_DEFAULT};

typedef struct {
    LV2_Atom_Event  event;
    uint8_t         msg[3];
}MIDINoteEvent;

uint32_t out_capacity;
uint32_t counter = 0;
smpl_t silence_threshold;

class Note2midi
{
public:

/*
************************************************************************************************************************
*           Ports variables
************************************************************************************************************************
*/

    float *in;
    LV2_Atom_Sequence *out;
    float *_onset_method;
    smpl_t *_onset_threshold;
    float *_pitch_method;
    smpl_t *_pitch_tolerance;
    smpl_t *_silence_threshold;

/*
************************************************************************************************************************
*           LV2 midi related variables
************************************************************************************************************************
*/
    
    LV2_URID_Map* map;
    LV2_URID atom_sequence;

    MIDINoteEvent note;

/*
************************************************************************************************************************
*           Aubio related variables
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*           ONSET variables
************************************************************************************************************************
*/
    char_t *onset_method;
    unsigned int current_onset_method;
    smpl_t onset_threshold;
    fvec_t *onset;
    aubio_onset_t *o;

/*
************************************************************************************************************************
*           PITCH variables
************************************************************************************************************************
*/

    char_t * pitch_unit;
    char_t * pitch_method;
    unsigned int current_pitch_method;
    char_t * tempo_method;
    smpl_t pitch_tolerance;

    fvec_t *pitch_obuf;
    aubio_pitch_t *pitch;

    fvec_t *note_buffer;
    fvec_t *note_buffer2;
/*
************************************************************************************************************************
*           midi variables
************************************************************************************************************************
*/

    smpl_t curnote;
    smpl_t newnote;
    uint_t isready;

/*
************************************************************************************************************************
*           common variables
************************************************************************************************************************
*/

    fvec_t *ibuf;
    uint_t median;
    uint_t samplerate;
    // uint_t buffer_size;
    // uint_t hop_size;
    // uint_t pitch_buff_times;



    Note2midi() {}
    ~Note2midi() {}
    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);

    void send_noteon (int pitch, int velo);
    void process_block ();
    void note_append (fvec_t * note_buffer, smpl_t curnote);
    uint_t get_note (fvec_t * note_buffer, fvec_t * note_buffer2);


    void set_plugin();
};

/*
************************************************************************************************************************
*           AUBIO Functions
************************************************************************************************************************
*/

void Note2midi::send_noteon (int pitch, int velo)
{


    // smpl_t mpitch = pitch;
    smpl_t mpitch = floor (aubio_freqtomidi (pitch) + .5);
    note.event.body.size = 3;
    note.event.body.type = map->map(map->handle, LV2_MIDI__MidiEvent);
    note.event.time.frames = counter;
    note.msg[2] = velo;
    note.msg[1] = mpitch;
    if (velo == 0) {
        printf("off ");
        note.msg[0] = 0x80;      /* note off */
    }
    else{
        printf("on ");
        note.msg[0] = 0x90;      /* note on */
    }

    printf("FREQ :%i\n", pitch);
    printf("MIDI :%f\n\n", mpitch);

    lv2_atom_sequence_append_event(this->out, out_capacity, &note.event);

}

void Note2midi::process_block (){


    smpl_t new_pitch, curlevel;
    // fvec_zeros(obuf);
    aubio_onset_do(o, ibuf, onset);

    aubio_pitch_do (pitch, ibuf, pitch_obuf);
    new_pitch = fvec_get_sample(pitch_obuf, 0);
    if(median){
        note_append(note_buffer, new_pitch);
    }

  /* curlevel is negatif or 1 if silence */
    curlevel = aubio_level_detection(ibuf, *_silence_threshold);
    if (fvec_get_sample(onset, 0)) {
    /* test for silence */
        if (curlevel == 1.) {
            if (median) isready = 0;
      /* send note off */
            send_noteon(curnote, 0);
        } else {
            if (median) {
                isready = 1;
            } else {
        /* kill old note */
                send_noteon(curnote, 0);
        /* get and send new one */
                send_noteon(new_pitch, 127+(int)floor(curlevel));
                curnote = new_pitch;
            }
        }
    } 
    else {
        if (median) {
            if (isready > 0)
                isready++;
            if (isready == median){
        /* kill old note */
                send_noteon(curnote, 0);
                newnote = get_note(note_buffer, note_buffer2);
                curnote = newnote;
        /* get and send new one */
                if (curnote>45){
                    send_noteon(curnote, 127+(int)floor(curlevel));
                }
            }
        } // if median
    }

    // lv2_atom_sequence_append_event(this->out, out_capacity, &note.event);
}

void Note2midi::note_append (fvec_t * note_buffer, smpl_t curnote){
    uint_t i = 0;
    for (i = 0; i < note_buffer->length - 1; i++) {
        note_buffer->data[i] = note_buffer->data[i + 1];
    }
    note_buffer->data[note_buffer->length - 1] = curnote;
    return;
}

uint_t Note2midi::get_note (fvec_t * note_buffer, fvec_t * note_buffer2){
    uint_t i;
    for (i = 0; i < note_buffer->length; i++) {
        note_buffer2->data[i] = note_buffer->data[i];
    }
    return fvec_median (note_buffer2);
}

void Note2midi::set_plugin(){
   o = new_aubio_onset (onset_method, PARAM_BUFFER_SIZE, PARAM_HOP_SIZE_ONSET, samplerate);
   if (onset_threshold != 0.) aubio_onset_set_threshold (o, onset_threshold);
   onset = new_fvec (1);

   pitch = new_aubio_pitch (pitch_method, PARAM_BUFFER_SIZE * PARAM_PITCH_BUFF_TIMES, PARAM_HOP_SIZE_PITCH, samplerate);
   if (pitch_tolerance != 0.) aubio_pitch_set_tolerance (pitch, pitch_tolerance);
   pitch_obuf = new_fvec (1);

   printf("%s\n", pitch_unit);

   if (median) {
      note_buffer = new_fvec (median);
      note_buffer2 = new_fvec (median);
  }
}

/*
************************************************************************************************************************
*           LV2 Functions
************************************************************************************************************************
*/

LV2_Handle Note2midi::instantiate(const LV2_Descriptor* descriptor, double SampleRate, const char* bundle_path, const LV2_Feature* const* features)
{
    Note2midi *plugin = new Note2midi();
    plugin->out = new LV2_Atom_Sequence();



    for (int i = 0; features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_URID__map)) {
            plugin->map = (LV2_URID_Map*)features[i]->data;
        }
    }

    if (!plugin->map) {
        // fprintf(stderr, "Missing feature urid:map\n");
        delete (plugin);
        return NULL;
    }

    plugin->atom_sequence = plugin->map->map(plugin->map->handle, LV2_ATOM__Sequence);

    plugin->ibuf = new_fvec(PARAM_BUFFER_SIZE);

    plugin->median = 6;
    plugin->curnote = 0.;
    plugin->newnote = 0.;
    plugin->isready = 0;

    // plugin->buffer_size = PARAM_BUFFER_SIZE;
    // plugin->hop_size = PARAM_HOP_SIZE;
    
    plugin->onset_method = "default";
    plugin->current_onset_method = 16;
    plugin->onset_threshold = 0.0; // will be set if != 0.
    
    // plugin->pitch_unit = "midi";
    plugin->pitch_unit = "default";
    plugin->pitch_method = "default";
    plugin->current_pitch_method = 6;
    plugin->pitch_tolerance = 0.0; // will be set if != 0.
    // plugin->pitch_buff_times = PARAM_PITCH_BUFF_TIMES; 
    
    plugin->tempo_method = "default";

    plugin->samplerate = SampleRate;

    plugin->set_plugin();


    printf("\n\n");
    printf("buffer size :%i\n", PARAM_BUFFER_SIZE);
    printf("hop size onset:%i\n", PARAM_HOP_SIZE_ONSET);
    printf("hop size pitch:%i\n", PARAM_HOP_SIZE_PITCH);
    printf("pitch buff times :%i\n", PARAM_PITCH_BUFF_TIMES);
    printf("\n\n");

    return (LV2_Handle)plugin;
}

void Note2midi::activate(LV2_Handle instance)
{   
}

void Note2midi::deactivate(LV2_Handle instance)
{
}

void Note2midi::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Note2midi *plugin;
    plugin = (Note2midi *) instance;

    switch (port)
    {
    case IN:
        plugin->in = (float*) data;
        break;
    case OUT:
        plugin->out = (LV2_Atom_Sequence*) data;
        break;
    case ONSET_METHOD:
        plugin->_onset_method = (float*) data;
        break;
    case ONSET_THRESHOLD:
        plugin->_onset_threshold = (float*) data;
        break;
    case PITCH_METHOD:
        plugin->_pitch_method = (float*) data;
        break;
    case PITCH_TOLERANCE:
        plugin->_pitch_tolerance = (float*) data;
        break;
    case SILENCE_THRESHOLD:
        plugin->_silence_threshold = (float*) data;
        break;
    }
}

void Note2midi::run(LV2_Handle instance, uint32_t SampleCount)
{
    Note2midi *plugin = (Note2midi *) instance;

    unsigned int new_onset_method = (unsigned int)(*plugin->_onset_method);
    unsigned int new_pitch_method = (unsigned int)(*plugin->_pitch_method);
    char_t* on_meth;
    char_t* pi_meth;

    silence_threshold = *(plugin->_silence_threshold);

    aubio_onset_set_threshold(plugin->o, *plugin->_onset_threshold);
    aubio_pitch_set_tolerance(plugin->pitch, *plugin->_pitch_method);

    if(0 <= new_onset_method && 16 >= new_onset_method &&
        new_onset_method != plugin->current_onset_method){ 
        
        del_aubio_onset(plugin->o);

        switch(new_onset_method){
            case ONSET_METHOD_ENERGY:
            on_meth = "energy";
            break;
            case ONSET_METHOD_SPECDIFF:
            on_meth = "specdiff";
            break;
            case ONSET_METHOD_HFC:
            on_meth = "hfc";
            break;
            case ONSET_METHOD_COMPLEXDOMAIN:
            on_meth = "complexdomain";
            break;
            case ONSET_METHOD_COMPLEX:
            on_meth = "complex";
            break;
            case ONSET_METHOD_PHASE:
            on_meth = "phase";
            break;
            case ONSET_METHOD_MKL:
            on_meth = "mkl";
            break;
            case ONSET_METHOD_KL:
            on_meth = "kl";
            break;
            case ONSET_METHOD_SPECFLUX:
            on_meth = "specflux";
            break;
            case ONSET_METHOD_CENTROID:
            on_meth = "centroid";
            break;
            case ONSET_METHOD_SPREAD:
            on_meth = "spread";
            break;
            case ONSET_METHOD_SKEWNESS:
            on_meth = "skewness";
            break;
            case ONSET_METHOD_KURTOSIS:
            on_meth = "kurtosis";
            break;
            case ONSET_METHOD_SLOPE:
            on_meth = "slope";
            break;
            case ONSET_METHOD_DECREASE:
            on_meth = "decrease";
            break;
            case ONSET_METHOD_ROLLOFF:
            on_meth = "rolloff";
            break;
            default:
            on_meth = "default";
            break;
        }


        plugin->o = new_aubio_onset (on_meth, PARAM_BUFFER_SIZE, PARAM_HOP_SIZE_ONSET, plugin->samplerate);
        
        plugin->current_onset_method = new_onset_method;
    }

    if(0 <= new_pitch_method && 6 >= new_pitch_method &&
        new_pitch_method != plugin->current_pitch_method){
        del_aubio_pitch(plugin->pitch);

        switch(new_pitch_method){
            case PITCH_METHOD_MCOMB:
            pi_meth = "mcomb";
            break;
            case PITCH_METHOD_YINFFT:
            pi_meth = "yinfft";
            break;
            case PITCH_METHOD_YIN:
            pi_meth = "yin";
            break;
            case PITCH_METHOD_SCHMITT:
            pi_meth = "schmitt";
            break;
            case PITCH_METHOD_FCOMB:
            pi_meth = "fcomb";
            break;
            case PITCH_METHOD_SPECACF:
            pi_meth = "specacf";
            break;
            default:
            pi_meth = "default";
            break;
        }


        plugin->pitch = new_aubio_pitch (pi_meth, PARAM_BUFFER_SIZE * PARAM_PITCH_BUFF_TIMES, PARAM_HOP_SIZE_PITCH, plugin->samplerate);
     
        // aubio_pitch_set_unit (plugin->pitch, plugin->pitch_unit);

        plugin->current_pitch_method = new_pitch_method;
    }

    out_capacity = plugin->out->atom.size;

    plugin->out->atom.type = plugin->atom_sequence;
    lv2_atom_sequence_clear(plugin->out);

    plugin->ibuf->data = (smpl_t *) plugin->in;

    counter++;

    if(counter++ >= PARAM_BUFFER_SIZE)
        counter = 0;

    plugin->process_block();
}


void Note2midi::cleanup(LV2_Handle instance)
{
    Note2midi *plugin;
    plugin = (Note2midi *) instance;


    plugin->send_noteon(plugin->curnote, 0);

    lv2_atom_sequence_append_event(plugin->out, out_capacity, &(plugin->note.event));

    del_aubio_pitch (plugin->pitch);
    if (plugin->median) {
        del_fvec (plugin->note_buffer);
        del_fvec (plugin->note_buffer2);
    }
    del_fvec (plugin->pitch_obuf);

    del_fvec (plugin->ibuf);

    delete ((Note2midi *) instance);
}


const void* Note2midi::extension_data(const char* uri)
{
    return NULL;
}

static const LV2_Descriptor Descriptor =
{
    PLUGIN_URI,
    Note2midi::instantiate,
    Note2midi::connect_port,
    Note2midi::activate,
    Note2midi::run,
    Note2midi::deactivate,
    Note2midi::cleanup,
    Note2midi::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}