/*----------------------------------------------------------------------------
 * get_peaks.c
 *
 * Get the peaks in the prepared (filtered) signals.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "get_peaks.h"

#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
	#undef fprintf
	#undef printf
#endif  /* WIN32 */


int
get_peaks(struct data_buffer *buf, double *sig_use, long start, long num,
	  double threshold, double area_filt, struct peak_buffer **peaks, long *num_peaks,
	  long *morph_type, long force_morph_type, long ms30, int allow_look_before, int allow_look_after)
{
	int ret;
	long num_peaks_force, *peak_positive;
	static int runs = 0;	/* TODO: implement more save way to count iterations */
	double threshold2, area_filt2;		

	if (runs > 30)
		return 0;

	ret = find_peaks(buf, sig_use, start, num, threshold, area_filt,
			 peaks, num_peaks, morph_type, ms30,
			 allow_look_before, allow_look_after);
	if (ret != 0)
		return ret;

	if ((force_morph_type <= 0) || (*morph_type == force_morph_type))
		return ret;

	num_peaks_force = 0;
	peak_positive = NULL;
	ret = get_morph_type_info(force_morph_type, &num_peaks_force, &peak_positive);
	if (ret != 0)
	{
		if (peak_positive)
			free(peak_positive);
		return ret;
	}

	if (*num_peaks > num_peaks_force)
	{
		(*num_peaks) -= ((*num_peaks) - num_peaks_force);
		if ((((*peaks)[0].amp > 0) && (peak_positive[0] == 0))
		    || (((*peaks)[0].amp < 0) && (peak_positive[0] == 1)))
		{
			memcpy(*peaks, (*peaks)+1, sizeof(struct peak_buffer) * (*num_peaks));
		}
		*morph_type = force_morph_type;
	}
	else
	{
		runs++;

 		threshold2 = threshold * 0.95;
		area_filt2 = area_filt * 0.95;

		if (*peaks)
		{
			free(*peaks);
			*peaks = NULL;
		}
		*num_peaks = 0;

		ret = get_peaks(buf, sig_use, start, num, threshold2, area_filt2, peaks, num_peaks,
				morph_type, force_morph_type, ms30, allow_look_before, allow_look_after);
		runs--;
	}
	

	if (peak_positive)
		free(peak_positive);

	return ret;
} /* get_peaks */
		

int
get_morph_type_info(long morph_type, long *num_peaks, long **peak_positive)
{
	int ret = 0;

	if ((num_peaks == NULL) || (peak_positive == NULL))
		return -1;

	if (morph_type <= 0)
		return 0;

	(*num_peaks)++;
	*peak_positive = realloc(*peak_positive, sizeof(long) * (*num_peaks));
	
	if (morph_type > 999)
	{
		long t = (morph_type/1000)-1;
		(*peak_positive)[(*num_peaks)-1] = t;
		ret = get_morph_type_info(morph_type - 1000 - t*1000, num_peaks, peak_positive);
	}
	else if (morph_type > 99)
	{
		long t = (morph_type/100)-1;
		(*peak_positive)[(*num_peaks)-1] = t;
		ret = get_morph_type_info(morph_type - 100 - t*100, num_peaks, peak_positive);
	}
	else if (morph_type > 9)
	{
		long t = (morph_type/10)-1;
		(*peak_positive)[(*num_peaks)-1] = t;
		ret = get_morph_type_info(morph_type - 10 - t*10, num_peaks, peak_positive);
	}
	else
		(*peak_positive)[(*num_peaks)-1] = morph_type - 1;

	return ret;
} /* get_morph_type_info() */


/* TODO: split function in several shorter functions */
int
find_peaks(struct data_buffer *buf, double *sig_use, long start, long num,
	   double threshold, double area_filt, struct peak_buffer **peaks, long *num_peaks,
	   long *morph_type, long ms30, int allow_look_before, int allow_look_after)
{
	int ret = 0;
	int *flag = NULL;
	struct peak_buffer *p_temp = NULL;
	long num_peaks_temp = 0;
	long l, m, idx, skip, start_use, num_use;
	double max;
	int look_begin;
	int debug = 1;

	if ((buf == NULL) || (sig_use == NULL) || (peaks == NULL) || (num_peaks == NULL) || (morph_type == NULL))
		return -1;

	*num_peaks = 0;
	start_use = start;
	num_use = num;

	if (allow_look_before)
	{
		for (l = start_use; l >= 0; l--)
		{
			if (fabs(sig_use[l]) <= threshold)
				break;
			if ((sig_use[l+1] >= threshold) && (sig_use[l] <= threshold))
				break;
			if ((sig_use[l+1] <= threshold) && (sig_use[l] >= threshold))
				break;
			start_use--;
			num_use++;
		}
	}
	if (allow_look_after)
	{
		for (l = (start_use + num_use); l < buf->num_data; l++)
		{
			if (fabs(sig_use[l]) <= threshold)
				break;
			if ((sig_use[l-1] >= threshold) && (sig_use[l] <= threshold))
				break;
			if ((sig_use[l-1] <= threshold) && (sig_use[l] >= threshold))
				break;
		}
		num_use = l - start_use + 1;
	}

	/* TODO: check why num_use can be negative */
	if (num_use > 0)
	{
		flag = calloc(num_use, sizeof(int));
		for (l = 0; l < num_use; l++)
		{
			if (sig_use[start_use + l] > threshold)
				flag[l] = 1;
			if (sig_use[start_use + l] < -threshold)
				flag[l] = -1;
		}
	}

	look_begin = 1;
	for (l = 1; l < num_use; l++)
	{
		if (flag[l] == flag[l-1])
			continue;

		if (look_begin)
		{
			(*num_peaks)++;
			(*peaks) = realloc(*peaks, sizeof(struct peak_buffer) * (*num_peaks));
			(*peaks)[*num_peaks-1].start = l + start_use;
			look_begin = 0;
		}
		else
		{
			(*peaks)[*num_peaks-1].end = l-1 + start_use;

			(*peaks)[*num_peaks-1].amp = sig_use[(*peaks)[*num_peaks-1].start];
			(*peaks)[*num_peaks-1].idx = (*peaks)[*num_peaks-1].start;
			for (m = ((*peaks)[*num_peaks-1].start + 1); m < (l+start_use); m++)
			{
				if ((flag[l-1] > 0) && (sig_use[m] > (*peaks)[*num_peaks-1].amp))
				{
					(*peaks)[*num_peaks-1].amp = sig_use[m];
					(*peaks)[*num_peaks-1].idx = m;
				}
				if ((flag[l-1] < 0) && (sig_use[m] < (*peaks)[*num_peaks-1].amp))
				{
					(*peaks)[*num_peaks-1].amp = sig_use[m];
					(*peaks)[*num_peaks-1].idx = m;					
				}
			}

			/* check if we have a fast change (from one peak directly to the next one) */
			if (fabs(flag[l-1] - flag[l]) == 2)
			{
				(*num_peaks)++;
				(*peaks) = realloc(*peaks, sizeof(struct peak_buffer) * (*num_peaks));
				(*peaks)[*num_peaks-1].start = l + start_use;
				look_begin = 0;
			}
			else
				look_begin = 1;
		}
	}

	/* check if a not finished peak is at the end; if yes, ignore it */
	if (look_begin == 0)
 		(*num_peaks)--; 

	if (flag)
		free(flag);
	if (debug) fprintf(stderr, "get peaks: found peaks = %ld\n", *num_peaks);

	if (*peaks == NULL)
	{
		*num_peaks = 0;
		return 0;
	}

	/* remove peaks which are too distant (checking before and after the largest one) */
	max = fabs((*peaks)[0].amp);
	idx = 0;
	for (l = 1; l < (*num_peaks); l++)
	{
		if (fabs((*peaks)[l].amp) > max)
		{
			max = fabs((*peaks)[l].amp);
			idx = l;
		}
	}
	for (l = idx+1; l < (*num_peaks); l++)
	{
		if (((*peaks)[l].start - (*peaks)[l-1].end) > 2*ms30)
		{
			*num_peaks = l;
			break;
		}
	}
	for (l = idx-1; l >= 0; l--)
	{
		if (((*peaks)[l+1].start - (*peaks)[l].end) > 2*ms30)
		{
			*num_peaks -= (l+1);
			memmove((*peaks), (*peaks)+l+1, sizeof(struct peak_buffer) * (*num_peaks));
			break;
		}
	}
	if (debug) fprintf(stderr, "  num peaks after distance check = %ld\n", *num_peaks);

	if ((*num_peaks) <= 0)
	{
		free(*peaks);
		*peaks = NULL;
		return 0;
	}

	/* concatenate peaks when one or more small peaks are between two large ones
	   (amplitude less than 33% of the surrounding peaks) */
	p_temp = malloc(sizeof(struct peak_buffer) * (*num_peaks));
	memcpy(p_temp, (*peaks), sizeof(struct peak_buffer));
	num_peaks_temp = 1;
	skip = 0;
	for (l = 1; l < *num_peaks; l++)
	{
		double amp, amp_pre;

		if (skip)
		{
			skip--;
			continue;
		}

		amp = fabs((*peaks)[l].amp);
		amp_pre = fabs((*peaks)[l-1].amp);
		for (m = l+1; m < *num_peaks; m++)
		{
			double amp_post = fabs((*peaks)[m].amp);

			if ((amp < 0.33*amp_pre) && (amp < 0.33*amp_post) &&
			    (((*peaks)[l-1].amp >= 0 ? 1 : -1) != ((*peaks)[m].amp >= 0 ? 1 : -1)))
			{
				if (amp_pre > amp_post)
				{
					p_temp[num_peaks_temp-1].end = (*peaks)[l].end;
					skip = 1;
				}
				else
				{
					(*peaks)[m].start = (*peaks)[l].start;
					skip = m-l;
				}
			}
		}
		if (skip > 0)
			continue;

		memcpy(p_temp + num_peaks_temp, (*peaks)+l, sizeof(struct peak_buffer));
		num_peaks_temp++;
	}
	memcpy((*peaks), p_temp, sizeof(struct peak_buffer) * num_peaks_temp);
	*num_peaks = num_peaks_temp;
	if (debug) fprintf(stderr, "  num peaks after concatenating small peaks = %ld\n", *num_peaks);

	/* when wanted, remove peaks with low energy */
	if (area_filt > 0.0)
	{
		double max_area = 0.0;

		max = fabs((*peaks)[0].amp);
		idx = 0;
		for (l = 1; l < (*num_peaks); l++)
		{
			if (fabs((*peaks)[l].amp) > max)
			{
				max = fabs((*peaks)[l].amp);
				idx = l;
			}
		}

		max_area = 0.0;
		for (l = (*peaks)[idx].start; l <= (*peaks)[idx].end; l++)
			max_area += fabs(sig_use[l]);

		num_peaks_temp = 0;
		for (l = 0; l < *num_peaks; l++)
		{
			double area = 0.0;
			for (m = (*peaks)[l].start; m <= (*peaks)[l].end; m++)
				area += fabs(sig_use[m]);

			if (area < (area_filt * max_area))
				continue;

			memcpy(p_temp + num_peaks_temp, (*peaks)+l, sizeof(struct peak_buffer));
			num_peaks_temp++;
		}
		memcpy((*peaks), p_temp, sizeof(struct peak_buffer) * num_peaks_temp);
		*num_peaks = num_peaks_temp;			
	}
	if (debug) fprintf(stderr, "  num peaks after removing small ones = %ld\n", *num_peaks);

	/* only alternating peaks are allowed */
	max = fabs((*peaks)[0].amp);
	idx = 0;
	for (l = 1; l < (*num_peaks); l++)
	{
		if (fabs((*peaks)[l].amp) > max)
		{
			max = fabs((*peaks)[l].amp);
			idx = l;
		}
	}
	for (l = idx+1; l < (*num_peaks); l++)
	{
		if (((*peaks)[l-1].amp >= 0 ? 1 : -1) == ((*peaks)[l].amp >= 0 ? 1 : -1))
		{
			*num_peaks = l;
			break;
		}
	}
	for (l = idx-1; l >= 0; l--)
	{
		if (((*peaks)[l+1].amp >= 0 ? 1 : -1) == ((*peaks)[l].amp >= 0 ? 1 : -1))
		{
			*num_peaks -= (l+1);
			memmove((*peaks), (*peaks)+l+1, sizeof(struct peak_buffer) * (*num_peaks));
			break;
		}
	}
	if (debug) fprintf(stderr, "  num peaks after check if they are alternating = %ld\n", *num_peaks);
	
	/* max 4 peaks are allowed */
	if (*num_peaks > 4)
	{
		max = fabs((*peaks)[0].amp);
		idx = 0;
		for (l = 1; l < (*num_peaks); l++)
		{
			if (fabs((*peaks)[l].amp) > max)
			{
				max = fabs((*peaks)[l].amp);
				idx = l;
			}
		}

		if (idx <= 1)
			*num_peaks = 4;
		else if (idx >= (*num_peaks - 2))
		{
			memmove((*peaks), (*peaks)+(*num_peaks)-4, sizeof(struct peak_buffer) * 4);
			*num_peaks = 4;
		}
		else
		{
			if (fabs((*peaks)[idx-1].amp) > fabs((*peaks)[idx+1].amp))
				memmove((*peaks), (*peaks)+idx-2, sizeof(struct peak_buffer) * 4);
			else
				memmove((*peaks), (*peaks)+idx-1, sizeof(struct peak_buffer) * 4);

			*num_peaks = 4;
		}
	}

	/* get order of peaks */
	*morph_type = 0;
	for (l = 0; l < *num_peaks; l++)
	{
		if (l > 0)
			(*morph_type) *= 10;
		if ((*peaks)[l].amp > 0)
			(*morph_type) += 2;
		else
			(*morph_type) += 1;
	}

	if (p_temp)
		free(p_temp);

	return ret;
} /* get_peaks() */


