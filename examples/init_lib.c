//<![CDATA[
#include <stdio.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    value_handle vh;
    ra_find_handle f;
    struct ra_find_struct fs;

    /* initialize libRASCH */
    ra = ra_lib_init();

    /* check if init was successful */
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

    /* get some infos */
    vh = ra_value_malloc();
    if (ra_info_get(ra, RA_INFO_NUM_PLUGINS_L, vh) == 0)
    {
        printf("%s (%s): %d\n", ra_value_get_name(vh),
               ra_value_get_desc(vh), ra_value_get_long(vh));
    }
    ra_value_free(vh);

    /* find all measurements in a directory */
    f = ra_meas_find_first(ra, argv[1], &fs);
    if (f)
    {
        int cnt = 1;

        printf("measurements found in %s:\n", argv[1]);
        do
        {
            printf("  %2d: %s\n", cnt, fs.name);
            cnt++;
        }
        while (ra_meas_find_next(f, &fs));

        ra_meas_close_find(f);
    }

    /* close libRASCH */
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
