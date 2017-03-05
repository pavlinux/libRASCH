//<![CDATA[
#include <stdio.h>
#include <stdlib.h>
#include <ra.h>

int main(int argc, char *argv[])
{
    ra_handle ra;
    meas_handle meas;
    eval_handle eval;
    class_handle *clh;
    prop_handle prop_rri, prop_qrs_pos;
    long l, m, num_heartbeat, num_events, num_rri;
    const long *ev_ids;
    double *rri = NULL;
    double *qrs_pos = NULL;
    value_handle vh, vh_id, value;

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

    vh = ra_value_malloc();
    if (ra_class_get(eval, "heartbeat", vh) != 0)
    {
        printf
            ("no 'heartbeat' event-classes in measurement %s\n",
             argv[1]);
        return -1;
    }
    num_heartbeat = ra_value_get_num_elem(vh);
    clh = (class_handle *) ra_value_get_voidp_array(vh);
    vh_id = ra_value_malloc();
    value = ra_value_malloc();
    for (l = 0; l < num_heartbeat; l++)
    {
        if (ra_class_get_events(clh[l], -1, -1, 0, 1, vh_id) !=
            0)
        {
            printf("error getting event-id's\n");
            continue;
        }
        ev_ids = ra_value_get_long_array(vh_id);
        num_events = ra_value_get_num_elem(vh_id);

        /* get event-properties for RR-intervals and position of
           QRS-complex */
        prop_rri = ra_prop_get(clh[l], "rri");
        if (prop_rri == NULL)
        {
            printf("no event-property 'rri' in event-class\n");
            continue;
        }
        prop_qrs_pos = ra_prop_get(clh[l], "qrs-pos");
        if (prop_qrs_pos == NULL)
        {
            printf
                ("no event-property 'qrs-pos' in event-class\n");
            continue;
        }

        rri = malloc(sizeof(double) * num_events);
        qrs_pos = malloc(sizeof(double) * num_events);
        num_rri = 0;
        for (m = 0; m < num_events; m++)
        {
            if (ra_prop_get_value(prop_rri, ev_ids[m], -1, value)
                != 0)
                continue;
            rri[num_rri] = ra_value_get_double(value);

            if (ra_prop_get_value
                (prop_qrs_pos, ev_ids[m], -1, value) != 0)
                continue;
            qrs_pos[num_rri] = ra_value_get_double(value);

            num_rri++;
        }

        /* now do something with the RR-intervals and QRS-complex
           positions */

        if (rri)
            free(rri);
        if (qrs_pos)
            free(qrs_pos);
    }

    /* clean up */
    ra_value_free(vh);
    ra_value_free(vh_id);
    ra_value_free(value);

    ra_meas_close(meas);
    ra_lib_close(ra);

    return 0;
}                               /* main() */

//]]>
