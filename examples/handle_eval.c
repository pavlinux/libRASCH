//<![CDATA[
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    value_handle vh;
    meas_handle meas;
    eval_handle eval;
    long l, m, num_class, num_prop;
    class_handle *clh = NULL;
    prop_handle *ph = NULL;

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

    /* get default evaluation */
    eval = ra_eval_get_default(meas);
    if (eval == NULL)
    {
        printf("no evaluation in measurement %s\n", argv[1]);
        return -1;
    }

    /* get some infos about evaluation */
    vh = ra_value_malloc();
    if (ra_info_get(eval, RA_INFO_EVAL_NAME_C, vh) == 0)
        printf("evaluation %s ", ra_value_get_string(vh));
    if (ra_info_get(eval, RA_INFO_EVAL_ADD_TS_C, vh) == 0)
        printf("was added at %s ", ra_value_get_string(vh));
    if (ra_info_get(eval, RA_INFO_EVAL_PROG_C, vh) == 0)
        printf("using the program %s", ra_value_get_string(vh));
    printf("\n\n");

    /* list event-class's */
    num_class = 0;
    if (ra_class_get(eval, NULL, vh) == 0)
    {
        num_class = ra_value_get_num_elem(vh);
        clh = malloc(sizeof(class_handle) * num_class);
        memcpy(clh, (void *)ra_value_get_voidp_array(vh),
               sizeof(class_handle) * num_class);
    }

    for (l = 0; l < num_class; l++)
    {
        if (ra_info_get(clh[l], RA_INFO_CLASS_NAME_C, vh) == 0)
            printf("event-class %s", ra_value_get_string(vh));
        if (ra_info_get(clh[l], RA_INFO_CLASS_EV_NUM_L, vh) == 0)
            printf(" with %d events", ra_value_get_long(vh));
        printf(":\n");

        /* list event-properties */
        num_prop = 0;
        if (ra_prop_get_all(clh[l], vh) == 0)
        {
            num_prop = ra_value_get_num_elem(vh);
            ph = malloc(sizeof(prop_handle) * num_prop);
            memcpy(ph, (void *)ra_value_get_voidp_array(vh),
                   sizeof(prop_handle) * num_prop);
        }

        for (m = 0; m < num_prop; m++)
        {
            if (ra_info_get(ph[m], RA_INFO_PROP_ASCII_ID_C, vh)
                == 0)
                printf("  %s\n", ra_value_get_string(vh));
            if (ra_info_get(ph[m], RA_INFO_PROP_DESC_C, vh) == 0)
                printf("    %s", ra_value_get_string(vh));
            printf("\n");
        }

        free(ph);
    }
    free(clh);

    /* close */
    ra_value_free(vh);
    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
