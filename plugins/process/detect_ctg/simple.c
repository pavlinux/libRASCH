/*----------------------------------------------------------------------------
 * simple.c
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

#include <ra_ecg.h>
#include <ra_ch_list.h>

#include "simple.h"


/* debug beat detection (writes some data to file) */
#define DETECT_DEBUG  0


int
simple_detect(struct proc_info *pi, struct detect_result *dr)
{
	/* init */
	struct events **events_all;
	int *num_events;
	int i;
	char s[100];
	struct events *combined = NULL;
	int events_found = 0;
	int ret = 0;
	struct ra_ch_list *ch_list = (struct ra_ch_list *)pi->options;

	if (!ch_list)
	{
		_ra_set_error(pi, RA_ERR_INFO_MISSING, "plugin-detect-ctg");
		return -1;
	}

	events_all = malloc(sizeof(struct events *) * ch_list->num_ch);
	num_events = malloc(sizeof(int) * ch_list->num_ch);

	/* use every choosen channel for detection */
	for (i = 0; i < ch_list->num_ch; i++)
	{
		sprintf(s, gettext("detect events in channel %ld"), ch_list->ch[i] + 1);
		if (pi->callback)
			(*pi->callback) (s, 0);
		ch_detect(pi, ch_list->ch[i], &(num_events[i]), &(events_all[i]));
	}

	/* check if events are found */
	for (i = 0; i < ch_list->num_ch; i++)
	{
		if (num_events[i] > 0)
		{
			events_found = 1;
			break;
		}
	}
	if (!events_found)
	{
		ret = -1;
		goto clean_up;
	}

	/* combine detection results */
	sprintf(s, gettext("combine found events"));
	(*pi->callback) (s, 0);
	combined = combine_events(pi, events_all);

	/* save data in dr */
	sprintf(s, gettext("save results"));
	(*pi->callback) (s, 0);
	save_detect_result(pi, dr, combined);

 clean_up:
	/* clean up (no need for deleting all because all is pointer to events_all[0]) */
	for (i = 0; i < ch_list->num_ch; i++)
	{
		struct events *p = events_all[i];
		while (p)
		{
			struct events *t = p->next;
			free(p);
			p = t;
		}
	}
	free(num_events);
	free(events_all);

	return ret;
} /* detect() */


int
ch_detect(struct proc_info *pi, int ch, int *n, struct events **bhead)
{
	/* set variables that are dependent of samplerate */
	int blanktime;
	int boxcar_size1;
	int boxcar_size2;
	double sub_percent;
	double add_percent = 0.1;

	double *boxcar1 = NULL;
	int bpos1 = 0;
	double *boxcar2 = NULL;
/*	int bpos2 = 0;*/

	long cnt = -1;
	int found = 0;
	int above = 0;
	int above_cnt = 0;
	double t = 0.0;

	struct derivative_info di_1st, di_2nd;

	long num_read = 0;
	double buf[BUFFER_SIZE];
	long buf_pos = num_read;
	long ch_pos = 0;

	struct events *curr_beat = NULL;
	int percent_save = 0;

	long num_samples = 0;
	double samplerate = 0.0;
	value_handle vh;

	int width_cnt = 0;
#if DETECT_DEBUG
	FILE *fp;
#endif

	memset(&di_1st, 0, sizeof(struct derivative_info));
	memset(&di_2nd, 0, sizeof(struct derivative_info));

	/* get some infos about channel */
	vh = ra_value_malloc();
	ra_value_set_number(vh, ch);
	if (ra_info_get(pi->rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh) == 0)
		num_samples = ra_value_get_long(vh);
	if (ra_info_get(pi->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
		samplerate = ra_value_get_double(vh);
	ra_value_free(vh);

	/* set variables that are dependent of samplerate */
	blanktime = (int) (40 * samplerate);
	boxcar_size1 = (int) (20 * samplerate);
	boxcar_size2 = (int) (3 * samplerate);
	sub_percent = 0.01 / samplerate;

	/* init detect var's */
	boxcar1 = calloc(boxcar_size1, sizeof(double));
	boxcar2 = calloc(boxcar_size2, sizeof(double));

	/* detect */
	*bhead = malloc(sizeof(struct events));
	(*bhead)->pos = -1;
	(*bhead)->next = NULL;
	(*bhead)->prev = NULL;
	curr_beat = *bhead;
	curr_beat->width = -1;

#if DETECT_DEBUG
	fp = fopen("/home/rasch/det_debug.txt", "w");
#endif

	*n = 0;
	for (;;)
	{
		double v1, sum;
		/*double v2;*/

		if (buf_pos >= num_read)
		{
			int percent;

			num_read = (long)ra_raw_get_unit(pi->rh, ch, ch_pos, BUFFER_SIZE, buf);
			if (num_read <= 0)
				break;
			ch_pos += num_read;
			buf_pos = 0;

			/* check if callback should be called */
			percent = (int) (((double) cnt / (double) num_samples) * 100.0);
			if ((percent != percent_save) && pi->callback)
			{
				(*pi->callback) (NULL, percent);
				percent_save = percent;
			}
		}

		cnt++;
		blanktime--;

		/* smooth input data */
		v1 = boxcar_filter(boxcar1, boxcar_size1, &bpos1, buf[buf_pos]);
		buf_pos++;

		/* 1st derivative */
/* 		v1 = derivative(&di_1st, v1); */
/* 		v2 = derivative(&di_2nd, v1); */

		/* add 1st and 2nd derivative */
/* 		sum = 2.0 * (v1 + v2); */

		/* additional smoothing */
/* 		sum = boxcar_filter(boxcar2, boxcar_size2, &bpos2, sum); */

		/* no derivative, just use the boxcar-smoothed value */
		sum = v1;

		/* calc thresholds */
		if (sum >= t)
		{
			double diff = sum - t;
			/* above threshold */
			above = 1;
			above_cnt++;
			t += (diff * add_percent);

			width_cnt++;
		}
		else
		{
			double diff = sum - t;
			/* below threshold */
			above = 0;
			above_cnt = 0;
			found = 0;
			t += (diff * sub_percent);

			if (fabs(diff) > (0.1 * t))
			{
				if (curr_beat && (curr_beat->width == -1))
					curr_beat->width = width_cnt;
				width_cnt = 0;
			}
		}
#if DETECT_DEBUG
		fprintf(fp, "%lf\t%lf\n", sum, t);
#endif
		/* check if event */
		if ((above_cnt >= 5) && !found && (blanktime < 0))
		{
			long pos = cnt - boxcar_size1 - boxcar_size2;
			pos = search_beat(pi->rh, ch, pos, samplerate, 60000); /* search the next 60 seconds */

#if DETECT_DEBUG
			fprintf(stderr, "max found at: %ld\n", pos);
#endif
			found = 1;
			(*n)++;
			curr_beat->next = calloc(1, sizeof(struct events));
			curr_beat->next->prev = curr_beat;
			curr_beat = curr_beat->next;
			curr_beat->pos = pos;
			curr_beat->next = NULL;
			curr_beat->width = -1;

			blanktime = (int) (40 * samplerate);
		}
	}
#if DETECT_DEBUG
	fclose(fp);
#endif

	/* remove first (i.e. meaningless) entry */
	curr_beat = *bhead;
	*bhead = (*bhead)->next;
	if (*bhead)
		(*bhead)->prev = NULL;
	free(curr_beat);

	if (boxcar1)
		free(boxcar1);
	if (boxcar2)
		free(boxcar2);

	return 0;
} /* ch_detect() */


inline double
boxcar_filter(double *buf, int size, int *pos, double value)
{
	double ret = 0.0;
	int i;

	buf[*pos] = value;
	(*pos) = ((*pos) + 1) % size;

	for (i = 0; i < size; i++)
		ret += buf[i];
	ret /= (double) size;

	return ret;
} /* boxcar() */


inline double
derivative(struct derivative_info *der_i, double v)
{
	/* derivative */
	double d = v - der_i->last2;
	der_i->last2 = der_i->last1;
	der_i->last1 = v;
	d = fabs(d);
	/* smooth */
	d = 0.25 * (d + 2 * der_i->der1 + der_i->der2);
	der_i->der2 = der_i->der1;
	der_i->der1 = d;

	return d;
} /* smooth() */


long
search_beat(rec_handle rh, int ch, long pos, double samplerate, int len)	/* len in ms */
{
	long size, n, i;
	double *buf;

	double max = 0.0;
	int offset = 0;

	size = (int) ((double) len / 1000.0 * samplerate);
	buf = malloc(sizeof(double) * size);
	n = (long)ra_raw_get_unit(rh, ch, pos, size, buf);

	/* find maximum */
	for (i = 1; i < n; i++)
	{
		if (buf[i] > max)
		{
			max = buf[i];
			offset = i;
		}

	}

	if (buf)
		free(buf);

	return (pos + offset);
} /* search_beat() */


int
get_orientation(double *buf, int size, int *minmax_pos)
{
	double max, min;
	long max_pos, min_pos;

	int i;
	int ret;

	max = min = 0;
	max_pos = min_pos = 0;
	for (i = 0; i < size; i++)
	{
		if (buf[i] > max)
		{
			max = buf[i];
			max_pos = i;
		}
		if (buf[i] < min)
		{
			min = buf[i];
			min_pos = i;
		}
	}
	*minmax_pos = min_pos;
	ret = 0;		/* 0: negative orientation */
	if (max > fabs(min))
	{
		ret = 1;
		*minmax_pos = max_pos;
	}

	return ret;
} /* get_orientation() */


struct events *
combine_events(struct proc_info *pi, struct events **b)
{
	long ch_flag = 0;
	struct events *combined_head = b[0];
	struct events *combined_curr = combined_head;
	int i;
	int ch_use;
	value_handle vh;
	long *beat_range = NULL;
	struct ra_ch_list *ch_list = (struct ra_ch_list *)pi->options;

	/* calc beat-range for every channel (handling diff. samplerates) */
	beat_range = malloc(sizeof(long) * ch_list->num_ch);
	vh = ra_value_malloc();
	for (i = 0; i < ch_list->num_ch; i++)
	{
		double samplerate = 1.0;

		ra_value_set_number(vh, i);
		if (ra_info_get(pi->rh, RA_INFO_REC_CH_SAMPLERATE_D, vh) == 0)
			samplerate = ra_value_get_double(vh);
		beat_range[i] = (long) (BEAT_COMBINE_RANGE * samplerate);
	}
	ra_value_free(vh);

	/* get first used channel (needed for bitmask) */
	ch_use = ch_list->ch[0];
	ch_flag = (1L << ch_use);

	for (i = 1; i < ch_list->num_ch; i++)
	{
		struct events *curr_ch = b[i];

		ch_use = ch_list->ch[i];
		ch_flag = (1L << ch_use);

		combined_curr = combined_head;
		while (curr_ch)
		{
			/* beat alread found? */
			if ((curr_ch->pos >=
			     (combined_curr->pos - beat_range[0]))
			    && (curr_ch->pos <= (combined_curr->pos + beat_range[0])))
			{
				curr_ch = curr_ch->next;
				continue;
			}

			/* is beat before current combined beat? */
			if (curr_ch->pos < (combined_curr->pos - beat_range[0]))
			{
				struct events *ins = malloc(sizeof(struct events));
				ins->next = combined_curr;
				ins->prev = combined_curr->prev;
				combined_curr->prev = ins;
				if (ins->prev)
					ins->prev->next = ins;

				ins->pos = curr_ch->pos;

				curr_ch = curr_ch->next;
				continue;
			}

			/* beat is after current combined beat */
			if (combined_curr->next != NULL)
			{
				combined_curr = combined_curr->next;
				continue;
			}

			/* append beat to combined list */
			combined_curr->next = malloc(sizeof(struct events));
			combined_curr->next->next = NULL;
			combined_curr->next->prev = combined_curr;
			combined_curr->next->pos = curr_ch->pos;

			curr_ch = curr_ch->next;
		}
	}

	if (beat_range)
		free(beat_range);

	return combined_head;
} /* combine_events() */


int
save_detect_result(struct proc_info *pi, struct detect_result *dr, struct events *all)
{
	char s[100];

	sprintf(s, gettext("finish detection"));
	(*pi->callback) (s, 0);
	if (all)
	{
		struct events *c = all;
		struct events *dest;

		dr->num_events = 0;
		dr->ev = malloc(sizeof(struct events));
		dr->ev->prev = dr->ev->next = NULL;
		dr->ev->pos = c->pos;
		dr->num_events++;
		dest = dr->ev;
		while ((c = c->next) != NULL)	/* assignment ok !! */
		{
			dr->num_events++;
			dest->next = malloc(sizeof(struct events));
			dest->next->prev = dest;
			dest = dest->next;
			dest->next = NULL;
			dest->pos = c->pos;
		}
	}
	else
	{
		dr->num_events = 0;
		dr->ev = NULL;
	}

	return 0;
} /* save_detect_result() */
