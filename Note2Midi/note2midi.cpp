#include <lv2.h>
#include <lv2/atom.lv2/util.h>
#include <lv2/midi.lv2/midi.h>
#include <lv2/patch.lv2/patch.h>
#include <lv2/state.lv2/state.h>
#include <lv2/urid.lv2/urid.h>
#include <lv2/atom.lv2/atom.h>

// #include "./utils.h"
#include <aubio.h>
#include <aubio/fvec.h>
#include <aubio/mathutils.h>
#include <aubio/musicutils.h>
#include <aubio/onset/onset.h>
#include <aubio/pitch/pitch.h>
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_COUNT 256 // for fvec_median

#define AUBIO_UNSTABLE 1 // for fvec_median
#include "utils.h"
#define PROG_HAS_PITCH 1
#define PROG_HAS_ONSET 1
#define PROG_HAS_JACK 1
// TODO add PROG_HAS_OUTPUT
// #include "parse_args.h"

#define PLUGIN_URI "http://portalmod.com/plugins/MOD/note2midi"

/*
************************************************************************************************************************
*           LV2 Functions
************************************************************************************************************************
*/

// static inline LV2_Atom_Event* lv2_atom_sequence_append_event(LV2_Atom_Sequence* seq, uint32_t capacity, const LV2_Atom_Event* event) { 
//     const uint32_t total_size = (uint32_t)sizeof(*event) + event->body.size; 
//     if (capacity - seq->atom.size < total_size) { 
//         return NULL; 
//     } 

//     LV2_Atom_Event* e = lv2_atom_sequence_end(&seq->body, seq->atom.size); 
//     memcpy(e, event, total_size); 

//     seq->atom.size += lv2_atom_pad_size(total_size); 

//     return e; 
// } 

static inline LV2_Atom_Event* lv2_atom_sequence_append_event(LV2_Atom_Sequence* seq, uint32_t capacity, const LV2_Atom_Event* event) { 
    const uint32_t total_size = (uint32_t)sizeof(*event) + event->body.size; 

    // printf("total_size: %i\n", total_size);

    if (capacity - seq->atom.size < total_size) { 
        // printf("NULLLL\n");
        return NULL; 
    } 
    // printf("capacity: %i, seq->atom.size: %i \n", capacity, seq->atom.size);

    LV2_Atom_Event* e = lv2_atom_sequence_end(&seq->body, seq->atom.size); 
    memcpy(e, event, total_size); 

    seq->atom.size += lv2_atom_pad_size(total_size); 
    // printf("Event.atom.size: %i\n", seq->atom.size);

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

uint32_t out_capacity;
uint32_t counter = 0;

enum {IN, OUT, BYPASS};

typedef struct {
    LV2_Atom_Event  event;
    uint8_t         msg[3];
}MIDINoteEvent;

smpl_t silence_threshold = -90.;

class Note2midi
{
public:

    float *in;
    LV2_Atom_Sequence *out;
    float *bypass;
    
    LV2_URID_Map* map;
    LV2_URID atom_sequence;

    MIDINoteEvent note;



    fvec_t *ibuf;

    uint_t median;

    fvec_t *note_buffer;
    fvec_t *note_buffer2;

    smpl_t curnote;
    smpl_t newnote;
    uint_t isready;

    aubio_pitch_t *pitch;
    aubio_onset_t *o;
    fvec_t *onset;
    fvec_t *pitch_obuf;

    uint_t samplerate;

    uint_t buffer_size;
    uint_t hop_size;

    char_t * onset_method;
    smpl_t onset_threshold;

    char_t * pitch_unit;
    char_t * pitch_method;
    smpl_t pitch_tolerance;
    char_t * tempo_method;

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

void Note2midi::send_noteon (int pitch, int velo)
{

    static int lastnote = 0;

    smpl_t mpitch = floor (aubio_freqtomidi (pitch) + .5);
    note.event.body.size = 3;
    note.event.body.type = 19;
    note.event.time.frames = counter;
    note.msg[2] = velo;
    note.msg[1] = mpitch;
    if (velo == 0) {
      note.msg[0] = 0x80;      /* note off */
    }
    else{
      note.msg[0] = 0x90;      /* note on */
    }

    // printf("Ev.time.frames %i, Ev.body.type %i, Ev.body.size %i\n", note.event.time.frames,  note.event.body.type, note.event.body.size);

    // printf("buff[2] velo: %i, buff[1] pitch: %i, buff[0] on/off: %i\n", note.msg[2], note.msg[1], note.msg[0] );

    if(lastnote != mpitch){
        lv2_atom_sequence_append_event(this->out, out_capacity, &note.event);
    }

    lastnote = (int) mpitch;
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
    curlevel = aubio_level_detection(ibuf, silence_threshold);
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
   o = new_aubio_onset (onset_method, buffer_size, hop_size, samplerate);
   if (onset_threshold != 0.) aubio_onset_set_threshold (o, onset_threshold);
   onset = new_fvec (1);

   pitch = new_aubio_pitch (pitch_method, buffer_size * 4, hop_size, samplerate);
   if (pitch_tolerance != 0.) aubio_pitch_set_tolerance (pitch, pitch_tolerance);
   pitch_obuf = new_fvec (1);

   if (median) {
      note_buffer = new_fvec (median);
      note_buffer2 = new_fvec (median);
  }
}

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
    // plugin->atom_sequence = plugin->map->map(plugin->map->handle, LV2_MIDI__MidiEvent);

    plugin->ibuf = new_fvec(SAMPLE_COUNT);

    plugin->median = 6;
    plugin->curnote = 0.;
    plugin->newnote = 0.;
    plugin->isready = 0;
    plugin->buffer_size = SAMPLE_COUNT;
    plugin->hop_size = SAMPLE_COUNT/2;
    plugin->onset_method = "default";
    plugin->onset_threshold = 0.0; // will be set if != 0.
    plugin->pitch_unit = "default";
    plugin->pitch_method = "default";
    plugin->pitch_tolerance = 0.0; // will be set if != 0.
    plugin->tempo_method = "default";

    plugin->samplerate = SampleRate;

    plugin->set_plugin();

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
    case BYPASS:
        plugin->bypass = (float*) data;
        break;
    }
}

void Note2midi::run(LV2_Handle instance, uint32_t SampleCount)
{
    Note2midi *plugin = (Note2midi *) instance;

    out_capacity = plugin->out->atom.size;

    plugin->out->atom.type = plugin->atom_sequence;
    lv2_atom_sequence_clear(plugin->out);

    plugin->ibuf->data = (smpl_t *) plugin->in;

    counter++;

    if(counter++ >= SAMPLE_COUNT)
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