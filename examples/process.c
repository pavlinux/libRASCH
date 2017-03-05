//<![CDATA[
#include <stdio.h>
#include <string.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    struct ra_info *inf;
    meas_handle meas;
    eval_handle eval;
    plugin_handle pl;
    struct proc_info *pi;

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

    /* get plugin-handle for hrv-plugin */
    pl = ra_plugin_get_by_name(ra, "hrv", 0);
    if (pl == NULL)
    {
        printf("can't find plugin 'hrv'\n");
        return -1;
    }

    /* calculate hrv-values using the hrv-plugin */
    pi = (struct proc_info *)ra_proc_get(meas, pl, NULL);
    if (ra_proc_do(pi) == 0)
    {
        long num_res_sets, num_results, m, l;
        value_handle vh;

        /* get number of result-sets */
        vh = ra_value_malloc();
        if (ra_info_get(pi, RA_INFO_PROC_NUM_RES_SETS_L, vh) !=
            0)
        {
            printf("no result-sets\n");
            return -1;
        }
        num_res_sets = ra_value_get_long(vh);

        /* get number of results */
        if (ra_info_get(pi, RA_INFO_PROC_NUM_RES_L, vh) != 0)
        {
            printf("no results\n");
            return -1;
        }
        num_results = ra_value_get_long(vh);

        for (m = 0; m < num_res_sets; m++)
        {
            printf("result-set #%d:\n", m + 1);

            for (l = 0; l < num_results; l++)
            {
                char out[200], t[100];

                /* set number of result in which we are interested */
                ra_value_set_number(vh, l);

                /* test if result is a default value (some
                   non-default results are arrays which we skip in
                   this example) */
                ra_info_get(pl, RA_INFO_PL_RES_DEFAULT_L, vh);
                if (ra_value_get_long(vh) == 0)
                    continue;

                out[0] = '\0';
                if (ra_info_get(pl, RA_INFO_PL_RES_NAME_C, vh) ==
                    0)
                {
                    strcpy(t, ra_value_get_string(vh));
                    strcat(out, t);
                }
                if (ra_info_get(pl, RA_INFO_PL_RES_DESC_C, vh) ==
                    0)
                {
                    sprintf(t, " (%s)", ra_value_get_string(vh));
                    strcat(out, t);
                }
                if (ra_proc_get_result(pi, l, m, vh) == 0)
                {
                    sprintf(t, ": %lf", ra_value_get_double(vh));
                    strcat(out, t);
                }

                printf("  %s\n", out);
            }
        }

        ra_value_free(vh);
    }

    /* close */
    ra_proc_free(pi);
    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
