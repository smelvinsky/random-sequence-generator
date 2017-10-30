//
// Created by smelvinsky on 30.10.17.
//

#include "generator.h"
#include "soundcard/soundcard_noise.h"

/** variables associated with source 1 - soundcard */
static char *pcm_dev_name;
static snd_pcm_stream_t pcm_dev_stream;
static int pcm_dev_mode;
static snd_pcm_access_t pcm_dev_access_type;
static snd_pcm_format_t pcm_dev_sample_format;
static unsigned int pcm_dev_sample_rate;
static unsigned int pcm_dev_num_of_channels;
static pcm_dev_config pcm_dev_conf;
static pcm_dev pcm_device;

/** variables associated with source 2 - ... */

void gen_hw_init(buffer *buff_1, buffer *buff_2, buffer *buff_3, buffer *buff_4)
{
    /** source 1 - soundcard - hardware initialization */
    pcm_dev_name = "default";
    pcm_dev_stream = SND_PCM_STREAM_CAPTURE;
    pcm_dev_mode = 0;
    pcm_dev_access_type = SND_PCM_ACCESS_RW_INTERLEAVED;
    pcm_dev_sample_format = SND_PCM_FORMAT_S16_LE;
    pcm_dev_sample_rate = 44100;
    pcm_dev_num_of_channels = 2;

    pcm_dev_conf.pcm_dev_name = pcm_dev_name;
    pcm_dev_conf.pcm_dev_stream =  &pcm_dev_stream;
    pcm_dev_conf.pcm_dev_mode = &pcm_dev_mode;
    pcm_dev_conf.pcm_dev_access_type = &pcm_dev_access_type;
    pcm_dev_conf.pcm_dev_sample_format = &pcm_dev_sample_format;
    pcm_dev_conf.pcm_dev_sample_rate = &pcm_dev_sample_rate;
    pcm_dev_conf.pcm_dev_num_of_channels = &pcm_dev_num_of_channels;

    pcm_dev_init(&pcm_device, &pcm_dev_conf);
    pcm_dev_open(&pcm_device);
    buff_1->size = pcm_dev_set_parameters(&pcm_device, &(buff_1->buff));


    /** source 2 - ... - hardware innitialization */
}

void gen_hw_close(buffer *buff_1, buffer *buff_2, buffer *buff_3, buffer *buff_4)
{
    /** source 1 - soundcard - hardware deinitialization */
    pcm_dev_drain(&pcm_device);
    pcm_dev_close(&pcm_device, &(buff_1->buff));


    /** source 2 - ... - hardware deinitialization */
}

int gen_read_from_source1(buffer *buff_1)
{
    int samples_read = (int) pcm_dev_read(&pcm_device, buff_1->buff);
    return samples_read;
}
