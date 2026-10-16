/* Block-size helpers shared by the plugins in this repository.
 *
 * LV2 hosts announce the largest block they will ever pass to run() through the
 * bufsz:maxBlockLength option. Plugins that need scratch buffers allocate them
 * once, at instantiate, for that size; run() then processes in chunks that never
 * exceed it, so no allocation ever happens on the audio thread.
 */
#ifndef MOD_BUFFER_SIZE_H
#define MOD_BUFFER_SIZE_H

#include <stdint.h>
#include <string.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#define MOD_BLOCK_CAPACITY_MIN 64

/* Number of samples to allocate per processing buffer: the host's
 * bufsz:maxBlockLength, or `fallback` when the host does not provide it.
 * Never below MOD_BLOCK_CAPACITY_MIN. */
static inline uint32_t mod_block_capacity(const LV2_Feature* const* features, uint32_t fallback)
{
    const LV2_Options_Option* options = NULL;
    const LV2_URID_Map* map = NULL;
    uint32_t capacity = fallback;

    for (int i = 0; features != NULL && features[i] != NULL; ++i)
    {
        if (strcmp(features[i]->URI, LV2_OPTIONS__options) == 0)
            options = (const LV2_Options_Option*)features[i]->data;
        else if (strcmp(features[i]->URI, LV2_URID__map) == 0)
            map = (const LV2_URID_Map*)features[i]->data;
    }

    if (options != NULL && map != NULL)
    {
        const LV2_URID keyMax = map->map(map->handle, LV2_BUF_SIZE__maxBlockLength);
        const LV2_URID typeInt = map->map(map->handle, LV2_ATOM__Int);

        for (int i = 0; options[i].key != 0; ++i)
        {
            if (options[i].key == keyMax && options[i].type == typeInt && options[i].value != NULL)
            {
                const int32_t value = *(const int32_t*)options[i].value;
                if (value > 0)
                    capacity = (uint32_t)value;
                break;
            }
        }
    }

    return capacity < MOD_BLOCK_CAPACITY_MIN ? MOD_BLOCK_CAPACITY_MIN : capacity;
}

/* Length of the next chunk when processing `remaining` samples through buffers
 * of `capacity` samples: at most `capacity`, and never leaving a final chunk
 * shorter than 8 samples (the filters keep a few samples of history). */
static inline uint32_t mod_next_chunk(uint32_t remaining, uint32_t capacity)
{
    uint32_t n = remaining > capacity ? capacity : remaining;
    if (remaining > n && remaining - n < 8)
        n = remaining - 8;
    return n;
}

#endif /* MOD_BUFFER_SIZE_H */
