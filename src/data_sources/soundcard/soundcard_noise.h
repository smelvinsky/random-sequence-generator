/**
 *  Created by smelvinsky on 25.10.17.
 *
 *
 * A typical audio application has this rough structure:
 *
 *      open_the_device();
 *      set_the_parameters_of_the_device();
 *      while (!done) {
 *            // one or both of these
 *            receive_audio_data_from_the_device();
 *            deliver_audio_data_to_the_device();
 *        }
 *      close the device
 */

#ifndef SOUNDCARD_NOISE_H
#define SOUNDCARD_NOISE_H

#include "alsa/asoundlib.h"

typedef struct pcm_dev_config_t
{
    /* CONFIGURABLE BY USER: */

    /** PCM device name
     *  use command "arecord -L" / "arecord -l" or "aplay -L" / "aplay -l"
     *  for more information about detected devices  */
    const char *pcm_dev_name;

    /** PCM stream (direction) */
    snd_pcm_stream_t *pcm_dev_stream;

    /** PCM device mode */
    int *pcm_dev_mode;

    /** PCM device acces type */
    snd_pcm_access_t *pcm_dev_access_type;

    /** Sample format */
    snd_pcm_format_t *pcm_dev_sample_format;

    /** Sample rate */
    unsigned int *pcm_dev_sample_rate;

    /** Number of channels */
    unsigned int *pcm_dev_num_of_channels;

    /** Buffer size */
    const snd_pcm_uframes_t *pcm_dev_buff_size;

} pcm_dev_config;

typedef struct pcm_dev_t
{
    /* ALSA API structures */

    /** PCM handle structure */
    snd_pcm_t *pcm_handle;

    /** PCM hardware configuration space container */
    snd_pcm_hw_params_t *pcm_dev_params;

    /* CONFIGURABLE BY USER: */

    pcm_dev_config *pcm_dev_conf;

} pcm_dev;

void pcm_dev_init(pcm_dev *pcm_dev, pcm_dev_config *pcm_dev_conf);
void pcm_dev_open(pcm_dev *pcm_dev);

/** returns the size of data buffer (not allocated memory !)*/
size_t pcm_dev_set_parameters(pcm_dev *pcm_dev, void **dst_buff);

snd_pcm_sframes_t pcm_dev_read(pcm_dev *pcm_dev, void *dst_buff);
void pcm_dev_drain(pcm_dev *pcm_dev);
void pcm_dev_close(pcm_dev *pcm_dev, void **buff);


#endif //SOUNDCARD_NOISE_H
