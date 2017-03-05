//<![CDATA[
#include <stdio.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    value_handle vh;
    meas_handle meas;

    /* initialize libRASCH */
    ra = ra_lib_init();
    if ((ra == NULL)
        || (ra_lib_get_error(ra, NULL, 0) != RA_ERR_NONE))
    {
        if (!ra)
            printf("error initializing libRASCH\n");
        else
        {
            char err_t[200];
            long err_num;

            err_num = ra_lib_get_error(ra, err_t, 200);
            printf("while initializing libRASCH, error #%d "
                   "occured\n  %s\n", err_num, err_t);

            ra_lib_close(ra);
        }
        return -1;
    }

    /* open measurement */
    meas = ra_meas_open(ra, argv[1], NULL, 0);
    if (meas == NULL)
    {
        char err_t[200];
        long err_num;

        err_num = ra_lib_get_error(ra, err_t, 200);
        printf("can't open measurement %s\nerror #%d: %s\n",
               argv[1], err_num, err_t);
        ra_lib_close(ra);
        return -1;
    }

    /* get some infos */
    vh = ra_value_malloc();
    if (ra_info_get(meas, RA_INFO_NUM_SESSIONS_L, vh) == 0)
        printf("%s (%s): %d\n", ra_value_get_name(vh),
               ra_value_get_desc(vh), ra_value_get_long(vh));
    if (ra_info_get(meas, RA_INFO_MAX_SAMPLERATE_D, vh) == 0)
        printf("%s (%s): %f\n", ra_value_get_name(vh),
               ra_value_get_desc(vh), ra_value_get_double(vh));

    /* get all measurement-object infos */
    if (ra_info_get(meas, RA_INFO_NUM_OBJ_INFOS_L, vh) == 0)
    {
        long l;
        long n = ra_value_get_long(vh);

        for (l = 0; l < n; l++)
        {
            ra_info_get_by_idx(meas, RA_INFO_OBJECT, l, vh);
            printf("%s (%s): ", ra_value_get_name(vh),
                   ra_value_get_desc(vh));
            switch (ra_value_get_type(vh))
            {
            case RA_VALUE_TYPE_LONG:
                printf("%d\n", ra_value_get_long(vh));
                break;
            case RA_VALUE_TYPE_DOUBLE:
                printf("%f\n", ra_value_get_double(vh));
                break;
            case RA_VALUE_TYPE_CHAR:
                printf("%s\n", ra_value_get_string(vh));
                break;
            default:
                printf("not supported type\n");
                break;
            }
        }
    }

    /* close */
    ra_value_free(vh);
    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
