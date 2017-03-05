//<![CDATA[
#include <stdio.h>
#include <stdlib.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    value_handle vh;
    meas_handle meas;
    rec_handle rec;
    long l, num_ch;
    double *buf = NULL;

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

    /* get first 10000 samples for each channel */
    vh = ra_value_malloc();
    if (ra_info_get(rec, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) == 0)
        num_ch = ra_value_get_long(vh);
    buf = malloc(sizeof(double) * 10000);
    for (l = 0; l < num_ch; l++)
    {
        long m, num_read;

        num_read = ra_raw_get_unit(rec, l, 0, 10000, buf);
        for (m = 0; m < num_read; m++)
            ;                   /* do something with every sample */
    }

    /* clean up */
    free(buf);
    ra_value_free(vh);
    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
