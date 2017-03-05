//<![CDATA[
#include <stdio.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    value_handle vh;
    meas_handle meas;
    rec_handle rec;
    long l, num_dev, num_ch;

    /* initialize libRASCH */
    ra = ra_lib_init();
    if (ra == NULL)
    {
        printf("error initializing libRASCH\n");
        return -1;
    }

    /* open measurement */
    meas = ra_meas_open(ra, argv[1], NULL, 0);
    if (meas == NULL)
    {
        printf("can't open measurement %s\n", argv[1]);
        return -1;
    }

    /* get root recording */
    rec = ra_rec_get_first(meas, 0);
    if (rec == NULL)
    {
        printf("can't get recording-handle\n");
        return -1;
    }

    /* get some infos about recording */
    num_dev = num_ch = 0;
    vh = ra_value_malloc();
    if (ra_info_get(rec, RA_INFO_REC_GEN_NUM_DEVICES_L, vh) == 0)
        num_dev = ra_value_get_long(vh);
    if (ra_info_get(rec, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
        num_ch = ra_value_get_long(vh);
    if (ra_info_get(rec, RA_INFO_REC_GEN_NAME_C, vh) == 0)
        printf("measurement %s\n", ra_value_get_string(vh));
    if (ra_info_get(rec, RA_INFO_REC_GEN_DATE_C, vh) == 0)
        printf("recorded at %s\n", ra_value_get_string(vh));
    printf("#devices=%d\n#channels=%d\n\n", num_dev, num_ch);

    /* print name for every device */
    printf("infos about the recording devices used:\n");
    for (l = 0; l < num_dev; l++)
    {
        /* set number of device from which the info is wanted */
        ra_value_set_number(vh, l);
        if (ra_info_get(rec, RA_INFO_REC_DEV_HW_NAME_C, vh) == 0)
            printf("  device #%d: %s\n", l,
                   ra_value_get_string(vh));
    }
    printf("\n");

    /* print name and unit of every channel */
    printf("infos about the channels:\n");
    for (l = 0; l < num_ch; l++)
    {
        /* set number of channel */
        ra_value_set_number(vh, l);
        if (ra_info_get(rec, RA_INFO_REC_CH_NAME_C, vh) == 0)
            printf("  ch #%d: %s", l, ra_value_get_string(vh));
        if (ra_info_get(rec, RA_INFO_REC_CH_UNIT_C, vh) == 0)
            printf(" [%s]", ra_value_get_string(vh));
        printf("\n");
    }
    printf("\n");

    /* close */
    ra_value_free(vh);
    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
