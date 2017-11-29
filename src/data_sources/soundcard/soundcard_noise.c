/**
 *  Created by smelvinsky on 25.10.17.
 */

#include <malloc.h>
#include "soundcard_noise.h"

/** Uncomment for a debug */
#define PCM_DEBUG
#define PCM_DEBUG_DESC
//#define PCM_DEBUG_RW

typedef struct pcm_dev_state_info_t
{
    /** PCM device state enum value  */
    snd_pcm_state_t pcm_dev_state;

    /** PCM device state name */
    char *pcm_dev_state_name;

    /** PCM device state description */
    char *pcm_dev_state_desc;

} pcm_dev_state_info;

/** static variables */

/* stores current state od PCM device (name, description) */
static pcm_dev_state_info pcm_dev_state_inf;

/* stores given buffer size */
static const snd_pcm_uframes_t pcm_dev_buff_size;

/* stores period time */
static unsigned int pcm_dev_period_time;

/* stores period size */
static snd_pcm_uframes_t pcm_dev_period_size;

/* stores buffer time */
static unsigned int pcm_dev_buff_time;

static void pcm_dev_check_state(pcm_dev *pcm_dev, pcm_dev_state_info *pcm_dev_state_info)
{
    pcm_dev_state_info->pcm_dev_state = snd_pcm_state(pcm_dev->pcm_handle);

    if(pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_OPEN)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_OPEN";
        pcm_dev_state_info->pcm_dev_state_desc = "The PCM device is in the open state. After the snd_pcm_open() "
                "open call, \n\tthe device is in this state. Also, when snd_pcm_hw_params() call fails, then this state "
                "is entered \n\tto force application calling snd_pcm_hw_params() function to set right communication parameters.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_SETUP)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_SETUP";
        pcm_dev_state_info->pcm_dev_state_desc = "The PCM device has accepted communication parameters and it is waiting for\n\t "
                "\tsnd_pcm_prepare() call to prepare the hardware for selected operation (playback or capture).";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_PREPARED)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_PREPARED";
        pcm_dev_state_info->pcm_dev_state_desc = "The PCM device is prepared for operation. Application can use snd_pcm_start()\n\t\t    "
                "call, write or read data to start the operation.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_RUNNING)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_RUNNING";
        pcm_dev_state_info->pcm_dev_state_desc = "The PCM device has been started and is running. It processes the samples.\n\t "
                "The stream can be stopped using the snd_pcm_drop() or snd_pcm_drain() calls.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_XRUN)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_XRUN";
        pcm_dev_state_info->pcm_dev_state_desc = "The PCM device reached overrun (capture) or underrun (playback). \n\t"
                "You can use the -EPIPE return code from I/O functions (snd_pcm_writei(), \n\tsnd_pcm_writen(), snd_pcm_readi(), "
                "snd_pcm_readn()) to determine this state without checking \n\tthe actual state via snd_pcm_state() call. "
                "It is recommended to use the helper\n\t function snd_pcm_recover() to recover from this state, "
                "but you can \n\talso use snd_pcm_prepare(), snd_pcm_drop() or snd_pcm_drain() calls.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_DRAINING)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_DRAINING";
        pcm_dev_state_info->pcm_dev_state_desc = "The device is in this state when application using the capture mode called\n\t "
                "snd_pcm_drain() function. Until all data are read from the internal ring buffer \n\tusing I/O routines (snd_pcm_readi()"
                ", snd_pcm_readn())\n\t, then the device stays in this state.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_PAUSED)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_PAUSED";
        pcm_dev_state_info->pcm_dev_state_desc = "The device is in this state when application called the snd_pcm_pause() function \n\t"
                "until the pause is released. Not all hardware supports this feature. \n\tApplication should check the capability "
                "with the snd_pcm_hw_params_can_pause().";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_SUSPENDED)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_SUSPENDED";
        pcm_dev_state_info->pcm_dev_state_desc = "The device is in the suspend state provoked with the power management system. \n\t"
                "The stream can be resumed using snd_pcm_resume() call, \n\tbut not all hardware supports this feature. "
                "Application should check \n\tthe capability with the snd_pcm_hw_params_can_resume(). In other case, \n\t"
                "the calls snd_pcm_prepare(), snd_pcm_drop(), snd_pcm_drain() can be used to leave this state.";
    }
    else if (pcm_dev_state_info->pcm_dev_state == SND_PCM_STATE_DISCONNECTED)
    {
        pcm_dev_state_info->pcm_dev_state_name = "SND_PCM_STATE_DISCONNECTED";
        pcm_dev_state_info->pcm_dev_state_desc = "The device is physicaly disconnected. It does not accept any I/O calls in this state.";
    }
}

void pcm_dev_init(pcm_dev *pcm_dev, pcm_dev_config *pcm_dev_conf)
{
    #ifdef PCM_DEBUG
    printf("==> PCM device initialization...\n");
    #endif

    if (!pcm_dev)
    {
        fprintf(stderr, "pcm_dev pointer cannot be NULL (in \"pcm_dev_init\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    if(!pcm_dev_conf)
    {
        fprintf(stderr, "pcm_dev_config pointer cannot be NULL (in \"pcm_dev_init\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    pcm_dev->pcm_dev_conf = pcm_dev_conf;

    #ifdef PCM_DEBUG
    printf("==> PCM device initialized:\n\tDevice name: %s\n\tMode: %d\n", pcm_dev->pcm_dev_conf->pcm_dev_name, *(pcm_dev->pcm_dev_conf->pcm_dev_mode));
    #endif
}

void pcm_dev_open(pcm_dev *pcm_dev)
{
    #ifdef PCM_DEBUG
    printf("==> Opening PCM device...\n");
    #endif

    int err;

    if (!pcm_dev)
    {
        fprintf(stderr, "pcm_dev pointer cannot be NULL (in \"pcm_dev_open\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */


    if ((err = snd_pcm_open(&pcm_dev->pcm_handle, pcm_dev->pcm_dev_conf->pcm_dev_name, *(pcm_dev->pcm_dev_conf->pcm_dev_stream), *(pcm_dev->pcm_dev_conf->pcm_dev_mode))) < 0)
    {
        fprintf(stderr, "Cannot open audio device \"%s\" (%s)\nPlease check if your device is properly detected or if is being used by other software.\n"
                "Useful commands: \"arecord -L\" / \"arecord -l\" or \"aplay -L\" / \"aplay -l\"", pcm_dev->pcm_dev_conf->pcm_dev_name, snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    pcm_dev_check_state(pcm_dev, &pcm_dev_state_inf);
    printf("==> PCM device opened:\n\tState: %s\n", pcm_dev_state_inf.pcm_dev_state_name);
    #ifdef PCM_DEBUG_DESC
    printf("\tState description: %s\n", pcm_dev_state_inf.pcm_dev_state_desc);
    #endif
    #endif
}

size_t pcm_dev_set_parameters(pcm_dev *pcm_dev, void **dst_buff)
{
    #ifdef PCM_DEBUG
    printf("==> Setting Hardware parameters...\n");
    #endif

    int err;
    size_t buff_size;

    if (!dst_buff)
    {
        fprintf(stderr, "(*dst_buff) pointer cannot be NULL (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    if (!pcm_dev)
    {
        fprintf(stderr, "pcm_dev pointer cannot be NULL (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    /* Allocate the snd_pcm_hw_params_t structure of pcm_dev on the stack. */
    if ((err = snd_pcm_hw_params_malloc(&pcm_dev->pcm_dev_params)) < 0)
    {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Hardware parameters structure allocated on the stack\n");
    #endif

    /* Init pcm_dev params with full configuration space */
    if ((err = snd_pcm_hw_params_any(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Hardware configuration space initialized\n");
    #endif

    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */

    /* The access type specifies the way in which multichannel data is stored in the buffer. For INTERLEAVED access, */
    /* each frame in the buffer contains the consecutive sample data for the channels. For 16 Bit stereo data,       */
    /* this means that the buffer contains alternating words of sample data for the left and right channel.          */
    /* For NONINTERLEAVED access, each period contains first all sample data for the first channel followed by       */
    /* the sample data for the second channel and so on.                                                             */
    if ((err = snd_pcm_hw_params_set_access(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params, *(pcm_dev->pcm_dev_conf->pcm_dev_access_type))) < 0)
    {
        fprintf(stderr, "cannot set access type (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Access type set\n");
    #endif

    /* Set sample format */
    /* SND_PCM_FORMAT_S16_LE - 16-bit little-endian */
    /* SND_PCM_FORMAT_U8     - 8-bit, unsigned      */
    if ((err = snd_pcm_hw_params_set_format(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params, *(pcm_dev->pcm_dev_conf->pcm_dev_sample_format))) < 0)
    {
        fprintf(stderr, "cannot set sample format (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Sample format set\n");
    #endif

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    unsigned int supported_sample_rate = *(pcm_dev->pcm_dev_conf->pcm_dev_sample_rate);
    {
        if ((err = snd_pcm_hw_params_set_rate_near(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params, &supported_sample_rate, 0)) < 0)
        {
            fprintf(stderr, "cannot set sample rate (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }

        if (*(pcm_dev->pcm_dev_conf->pcm_dev_sample_rate) != supported_sample_rate)
        {
            fprintf(stderr, "the rate %d is not supported by your hardware.\n ===> using %d instead.", *(pcm_dev->pcm_dev_conf->pcm_dev_sample_rate), supported_sample_rate);
        }
    }

    #ifdef PCM_DEBUG
    printf("\t==> Sample rate set to %d\n", supported_sample_rate);
    #endif

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params, *(pcm_dev->pcm_dev_conf->pcm_dev_num_of_channels))) < 0)
    {
        fprintf(stderr, "cannot set channel count (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Number of channels set to %d\n", *(pcm_dev->pcm_dev_conf->pcm_dev_num_of_channels));
    #endif

    /* Apply pcm_dev parameter settings to */
    /* device and prepare device           */
    if ((err = snd_pcm_hw_params(pcm_dev->pcm_handle, pcm_dev->pcm_dev_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    pcm_dev_check_state(pcm_dev, &pcm_dev_state_inf);
    printf("\t==> Hardware parameters applied : waiting for snd_pcm_prepare() call \n\t    to prepare "
                       "the hardware for selected operation (playback or capture)... \n\t    \tState: %s\n\t\t", pcm_dev_state_inf.pcm_dev_state_name);
    #ifdef PCM_DEBUG_DESC
    printf("\tState description: %s\n", pcm_dev_state_inf.pcm_dev_state_desc);
    #endif
    #endif

    #ifdef PCM_DEBUG
    printf("\t==> Checking for given buffer size...\n");
    #endif

    /* Check what buffer size have been given and passes to pcm_dev_conf structure*/
    if ((err = snd_pcm_hw_params_get_buffer_size(pcm_dev->pcm_dev_params, (snd_pcm_uframes_t *) &pcm_dev_buff_size)) < 0)
    {
        fprintf(stderr, "cannot get buffer size from configuration space (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    pcm_dev->pcm_dev_conf->pcm_dev_buff_size = &pcm_dev_buff_size;

    #ifdef PCM_DEBUG
    printf("\t==> Given buffer size = %d frames\n", (int) *(pcm_dev->pcm_dev_conf->pcm_dev_buff_size));
    #endif

    #ifdef PCM_DEBUG
    printf("\t==> Checking for given period time...\n");
    #endif

    if ((err = snd_pcm_hw_params_get_period_time(pcm_dev->pcm_dev_params, &pcm_dev_period_time, NULL)) < 0)
    {
        fprintf(stderr, "cannot get period time from configuration space (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Given period time  = %d us\n", pcm_dev_period_time);
    #endif

    #ifdef PCM_DEBUG
    printf("\t==> Checking for given period size...\n");
    #endif

    if ((err = snd_pcm_hw_params_get_period_size(pcm_dev->pcm_dev_params, &pcm_dev_period_size, NULL)) < 0)
    {
        fprintf(stderr, "cannot get period size from configuration space (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Given period size  = %d frames\n", (int) pcm_dev_period_size);
    #endif

    #ifdef PCM_DEBUG
    printf("\t==> Checking for given buffer time...\n");
    #endif

    if ((err = snd_pcm_hw_params_get_buffer_time(pcm_dev->pcm_dev_params, &pcm_dev_buff_time, NULL)) < 0)
    {
        fprintf(stderr, "cannot get buffer time from configuration space (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("\t==> Given buffer time  = %d us\n", (int) pcm_dev_buff_time);
    #endif

    #ifdef PCM_DEBUG
    printf("\t==> Allocating memory for buffer...\n");
    #endif

    /* Use a buffer large enough to hold one period  */
    /* 2 bytes/sample, 2 channels => 4 * period_size */

    buff_size = (*(pcm_dev->pcm_dev_conf->pcm_dev_num_of_channels) * pcm_dev_period_size * (snd_pcm_hw_params_get_sbits(pcm_dev->pcm_dev_params) / 8));
    *dst_buff = malloc(buff_size);

    #ifdef PCM_DEBUG
    printf("\t==> Buffer allocated with size of %d\n", (int) malloc_usable_size(*dst_buff));
    #endif

    memset(*dst_buff, 0, malloc_usable_size(*dst_buff));

    /* Frees a previously allocated snd_pcm_hw_params_t */
    snd_pcm_hw_params_free (pcm_dev->pcm_dev_params);

    #ifdef PCM_DEBUG
    printf("\t==> Hardware parameters structure freed from the stack\n");
    #endif

    /* Prepares PCM for use */
    if ((err = snd_pcm_prepare(pcm_dev->pcm_handle)) < 0)
    {
        fprintf (stderr, "cannot prepare audio interface for use (%s) (in \"pcm_dev_set_parameters\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    pcm_dev_check_state(pcm_dev, &pcm_dev_state_inf);
    printf("\t==> Hardware parameters prepared : waiting for write/read operation \n\t    \tState: %s\n\t\t", pcm_dev_state_inf.pcm_dev_state_name);
    #ifdef PCM_DEBUG_DESC
    printf("\tState description: %s\n", pcm_dev_state_inf.pcm_dev_state_desc);
    #endif
    #endif

    return buff_size;
}

snd_pcm_sframes_t pcm_dev_read(pcm_dev *pcm_dev, void *dst_buff)
{
    #ifdef PCM_DEBUG_RW
    printf("==> Reading from PCM device...\n");
    #endif

    snd_pcm_sframes_t frames_read = 0;

    /* Read num_frames frames from the PCM device  */
    /* pointed to by pcm_handle to buffer capdata. */
    /* Returns the number of frames actually read. */
    if (*(pcm_dev->pcm_dev_conf->pcm_dev_access_type) == SND_PCM_ACCESS_RW_INTERLEAVED)
    {
        #ifdef PCM_DEBUG_RW
        printf("\t\tReading in interleaved mode...\n");
        #endif

        frames_read = snd_pcm_readi(pcm_dev->pcm_handle, dst_buff, pcm_dev_period_size);
    }
    if (*(pcm_dev->pcm_dev_conf->pcm_dev_access_type) == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
        #ifdef PCM_DEBUG_RW
        printf("\t\tReading in non-interleaved mode...\n");
        #endif

        frames_read = snd_pcm_readn(pcm_dev->pcm_handle, dst_buff, pcm_dev_period_size);
    }

    if (frames_read == -EPIPE)
    {
        /* EPIPE means overrun */
        fprintf(stderr, "overrun occurred\n");
        snd_pcm_prepare(pcm_dev->pcm_handle);
    }
    else if (frames_read < 0)
    {
        fprintf(stderr, "error from read: %s\n", snd_strerror((int) frames_read));
    }
    else if (frames_read != (int) pcm_dev_period_size)
    {
        fprintf(stderr, "short read, read %d frames\n", (int) frames_read);
    }

    return frames_read;
}

void pcm_dev_drain(pcm_dev *pcm_dev)
{
    #ifdef PCM_DEBUG
    printf("==> Draining PCM device...\n");
    #endif

    int err;

    if (!pcm_dev)
    {
        fprintf(stderr, "pcm_dev pointer cannot be NULL (in \"pcm_dev_drain\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    /* For playback wait for all pending frames to be played and then stop the PCM. */
    /* For capture stop PCM permitting to retrieve residual frames.                 */

    if ((err = snd_pcm_drain(pcm_dev->pcm_handle)) < 0)
    {
        fprintf (stderr, "cannot drain audio interface (%s) (in \"pcm_dev_drain\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    pcm_dev_check_state(pcm_dev, &pcm_dev_state_inf);
    printf("\t\tState: %s\n", pcm_dev_state_inf.pcm_dev_state_name);
    #ifdef PCM_DEBUG_DESC
    printf("\t\tState description: %s\n", pcm_dev_state_inf.pcm_dev_state_desc);
    #endif
    #endif

    #ifdef PCM_DEBUG
    printf("==> PCM device drained\n");
    #endif

}

void pcm_dev_close(pcm_dev *pcm_dev, void **buff)
{
    #ifdef PCM_DEBUG
    printf("==> Closing PCM device...\n");
    #endif

    int err;

    if (!pcm_dev)
    {
        fprintf(stderr, "pcm_dev pointer cannot be NULL (in \"pcm_dev_close\" function, soundcard_noise.c)\n");
        exit(EXIT_FAILURE);
    }

    /* Closes PCM handle */
    if ((err = snd_pcm_close(pcm_dev->pcm_handle)) < 0)
    {
        fprintf (stderr, "cannot close audio interface (%s) (in \"pcm_dev_close\" function, soundcard_noise.c)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    #ifdef PCM_DEBUG
    printf("==> PCM Device closed\n");
    #endif

    free(*buff);

    #ifdef PCM_DEBUG
    printf("==> Data buffer freed\n");
    #endif


}