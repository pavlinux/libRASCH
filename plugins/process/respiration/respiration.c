/*----------------------------------------------------------------------------
 * respiration.c  -  calculates respiration parameters 
 *
 * Description:
 * The plugin calculates parameters from a respiration signal. At the moment
 * only chest measurement based respiration signals can be processed.
 * At the moment only the mean of the chest measurement of the last inter beat
 * interval is calculated. In the future other measures (e.g. freq. of respiration)
 * will be implemented.
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2009, Raphael Schneider
 * 
 *
 * $Id$
 *--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <fftw3.h>

#define _LIBRASCH_BUILD
#include <ra.h>
#include <ra_priv.h>
#include <pl_general.h>
#include <ra_eval.h>
#include <ra_ecg.h>

#include "respiration.h"


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	/* set result infos */
	ps->info.num_results = num_results;
	ps->info.res = ra_alloc_mem(sizeof(results));
	memcpy(ps->info.res, results, sizeof(results));

	/* set option infos */
	ps->info.num_options = num_options;
	ps->info.opt = ra_alloc_mem(sizeof(options));
	memcpy(ps->info.opt, options, sizeof(options));

	/* set auto-create infos */
	ps->info.num_create_class = num_create_class;
	ps->info.create_class = ra_alloc_mem(sizeof(create_class));
	memcpy(ps->info.create_class, create_class, sizeof(create_class));
	ps->info.num_create_prop = num_create_prop;
	ps->info.create_prop = ra_alloc_mem(sizeof(create_prop));
	memcpy(ps->info.create_prop, create_prop, sizeof(create_prop));

	ps->call_gui = NULL;	/* will be set in init_ra_plugin(), eventually */
	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;
	ps->proc.do_processing = pl_do_processing;

	return 0;
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct librasch *ra, struct plugin_struct *ps)
{
	/* check if gui is available */
	if (ra_plugin_get_by_name(ra, "gui-respiration", 1))
		ps->call_gui = pl_call_gui;

	return 0;
} /* init_ra_plugin() */


LIBRAAPI int
fini_ra_plugin(void)
{
	return 0;
} /* fini_plugin() */


int
pl_call_gui(proc_handle proc)
{
	return 0;
} /* pl_call_gui() */


proc_handle
pl_get_proc_handle(plugin_handle pl)
{
	struct plugin_struct *ps = (struct plugin_struct *)pl;
	struct proc_info *pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_respiration));
	set_option_offsets(ps->info.opt, pi->options);
	set_default_options(pi->options);

	/* init auto-create structures to be sure we have the correct values */
	if (num_create_class > 0)
		memcpy(ps->info.create_class, create_class, sizeof(create_class));
	if (num_create_prop > 0)
		memcpy(ps->info.create_prop, create_prop, sizeof(create_prop));

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_respiration *opt)
{
	opt->use_start_end_pos = 0;
	opt->start_pos = -1;
	opt->end_pos = -1;

	opt->rh = NULL;
	opt->num_ch = 0;
	opt->ch = NULL;

	opt->save_in_eval = 0;

	opt->eh = NULL;
	opt->num_clh = 0;
	opt->clh = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_respiration *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_eval) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->use_start_end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->start_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->end_pos) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->rh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->save_in_eval) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->eh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_clh) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->clh) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in respiration.c - set_option_offsets():\n  number of options(%ld) != idx(%d)\n",
			num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = proc;
	
	if (pi->options)
	{
		struct ra_respiration *opt;
		opt = (struct ra_respiration *)pi->options;

		if (opt->ch)
			ra_free_mem(opt->ch);
		if (opt->clh)
			ra_free_mem(opt->clh);

		free(opt);
	}

	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(t[i]);
		free(t);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


int
pl_do_processing(proc_handle proc)
{
	int ret = 1;
	struct proc_info *pi = proc;
	struct ra_respiration *opt = NULL;
	value_handle *res;
	struct phase_infos phi;
	long l;

	if (pi == NULL)
		return -1;

	opt = (struct ra_respiration *)pi->options;

	/* if rec-handle was not set in options, use root recording */
	if (!opt->rh)
	{
		opt->rh = ra_rec_get_first(pi->mh, 0);
		if (opt->rh == NULL)
			goto clean_up;
	}
	/* if eval-handle was not set in options, use default evaluation */
	if (!opt->eh)
		opt->eh = ra_eval_get_default(pi->mh);
	if (opt->eh && !opt->clh)
	{
	}

	pi->num_result_sets = 1;
	pi->num_results = num_results;

	/* init results */
	/* first check if there are already some results are stored (maybe from a
	   run before with the same proc-handle) */
	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(t[i]);
		free(t);
		pi->results = NULL;
	}
	res = malloc(sizeof(value_handle *) * (pi->num_results * pi->num_result_sets));
	for (l = 0; l < (pi->num_results * pi->num_result_sets); l++)
	{
		int curr_idx = l % pi->num_results;

		value_handle vh = ra_value_malloc();
		set_meta_info(vh, results[curr_idx].name, results[curr_idx].desc, RA_INFO_NONE);
		res[l] = vh;
	}
	pi->results = res;

	/* if channel was not set (= -1), find respiration channel(s) */
	if (opt->num_ch <= 0)
	{
		if (find_resp_channel(opt) == -1)
			goto clean_up;
	}

	if (pi->callback)
		(*pi->callback) ("get beat data", 0);

	memset(&phi, 0, sizeof(struct phase_infos));
	phi.num_ch = opt->num_ch;
	phi.pos_scale = calloc(phi.num_ch, sizeof(double));

	for (l = 0; l < opt->num_ch; l++)
	{
		if (pi->callback)
			(*pi->callback) ("calc respiration phase", 0);

		if ((ret = get_phase(opt, l, &phi)) != 0)
			goto clean_up;
	}

	if (pi->callback)
		(*pi->callback) ("store data", 0);
	store_results(pi, &phi);

	ret = 0;

 clean_up:
	if (phi.spi)
	{
		for (l = 0; l < phi.num_phase_events; l++)
			free(phi.spi[l].phase);
		free(phi.spi);
		if (phi.pos_scale)
			free(phi.pos_scale);
	}
		     
/* 	if (beats) */
/* 		free(beats); */

	return ret;
} /* pl_do_processing() */


int
find_resp_channel(struct ra_respiration *opt)
{
	long l, n;
	value_handle vh;

	opt->num_ch = 0;
	if (opt->ch != NULL)
	{
		ra_free_mem(opt->ch);
		opt->ch = NULL;
	}

	vh = ra_value_malloc();
	if (ra_info_get(opt->rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return -1;
	}
	n = ra_value_get_long(vh);

	opt->ch = ra_alloc_mem(sizeof(long) * n);
	for (l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if (ra_info_get(opt->rh, RA_INFO_REC_CH_TYPE_L, vh) != 0)
			continue;

		if (ra_value_get_long(vh) == RA_CH_TYPE_RESP)
			opt->ch[opt->num_ch++] = l;
	}

	ra_value_free(vh);

	return 0;
} /* find_resp_channel() */


int
get_phase(struct ra_respiration *opt, long ch_idx, struct phase_infos *phi)
{
	int ret = -1;
	double samplerate;
	long dec_factor = -1;
	long l, num_data, num_data_use;
	double *resp_data, *re, *im;
	value_handle vh;
	meas_handle meas;

	resp_data = re = im = NULL;

	/* get decimation factor to downsample the respiration signal; should be
	   between 5 - 3 Hz */
	samplerate = get_samplerate(opt->rh, opt->ch[ch_idx]);
	for (l = 5; l >= 3; l--)
	{
		double temp = samplerate / (double)l;
		if ((temp - (long)temp) == 0.0)
		{
			dec_factor = (long)temp;
			break;
		}
	}
	if (dec_factor <= 0)
		return -1;

	/* get scaling factor for event-positions */
	phi->pos_scale[ch_idx] = dec_factor;
	vh = ra_value_malloc();
	ra_value_set_number(vh, opt->ch[ch_idx]);
	meas = ra_meas_handle_from_any_handle(opt->rh);
	if (ra_info_get(meas, RA_INFO_CH_XSCALE_D, vh) == 0)
		phi->pos_scale[ch_idx] /= ra_value_get_double(vh);
	ra_value_free(vh);

	/* do the calculations */
	ret = downsample_data(opt->rh, opt->ch[ch_idx], dec_factor, &num_data, &resp_data);
	if (ret != 0)
		goto clean_up;
	
	ret = do_hilbert_transformation(resp_data, num_data, &re, &im);
	if (ret != 0)
		goto clean_up;
	
	/* if first run, allocate needed memory and set event positions */
	if (phi->num_phase_events <= 0)
	{
		long curr_start = 0;

		phi->num_phase_events = num_data;
		phi->spi = calloc(num_data, sizeof(struct single_phase_info));

		for (l = 0; l < num_data; l++)
		{
			phi->spi[l].start = curr_start;
			phi->spi[l].end = phi->spi[l].start + (long)phi->pos_scale[ch_idx]-1;
			curr_start += (long)phi->pos_scale[ch_idx];

			phi->spi[l].phase = calloc(opt->num_ch, sizeof(double));
		}
	}

	/* and now calculate the phase of the signal */
	num_data_use = num_data;
	if (num_data_use > phi->num_phase_events)
		num_data_use = phi->num_phase_events;

	for (l = 0; l < num_data_use; l++)
		phi->spi[l].phase[ch_idx] = atan2(im[l], resp_data[l]);

	/* if the current channel is shorter than the first one, set the
	   remaining values ot 'INVALID_VALUE' */
	for (l = num_data_use; l < num_data; l++)
		phi->spi[l].phase[ch_idx] = INVALID_VALUE;

	ret = 0;

 clean_up:
	if (resp_data)
		free(resp_data);
	if (re)
		free(re);
	if (im)
		free(im);

	return ret;
} /* get_phase() */


int
downsample_data(rec_handle rh, long ch_num, long dec_factor, long *num_data, double **data)
{
	int ret;
	value_handle vh;
	long l, num_all, curr_pos;
	double *buffer;

	vh = ra_value_malloc();
	if ((ret = ra_info_get(rh, RA_INFO_REC_CH_NUM_SAMPLES_L, vh)) != 0)
		goto clean_up;
	num_all = ra_value_get_long(vh);
	ra_value_free(vh);

	*num_data = num_all / dec_factor;
	*data = malloc(sizeof(double) * (*num_data));

	buffer = malloc(sizeof(double) * dec_factor);
	curr_pos = 0;
	for (l = 0; l < *num_data; l++)
	{
		long n = ra_raw_get_unit(rh, ch_num, curr_pos, dec_factor, buffer);
		if (n < dec_factor)
		{
			*num_data = l-1;
			break;
		}
		curr_pos += dec_factor;

		(*data)[l] = mean(buffer, n);
	}
	
	ret = 0;

 clean_up:
	if (buffer)
		free(buffer);

	return ret;
} /* downsample_data() */


int
do_hilbert_transformation(double *resp_data, long num_data, double **re, double **im)
{
	fftw_complex *in_fftw, *out_fftw;
	fftw_plan p;
	long l, n2;

	in_fftw = fftw_malloc(sizeof(fftw_complex) * num_data);
	for (l = 0; l < num_data; l++)
	{
		in_fftw[l][0] = resp_data[l];
		in_fftw[l][1] = 0.0;
	}

	out_fftw = fftw_malloc(sizeof(fftw_complex) * num_data);
	p = fftw_plan_dft_1d(num_data, in_fftw, out_fftw, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);

	/* do Hilbert specific stuff (TODO: read about Hilbert transformation to know
	   what is going on here and maybe it can be done easier) */
	n2 = num_data/2;
	if (2*n2 == num_data)
	{
		/* even number of data */
		for (l = 1; l < n2; l++)
		{
			out_fftw[l][0] *= 2.0;
			out_fftw[l][1] *= 2.0;
		}
		for (l = n2+1; l < num_data; l++)
		{
			out_fftw[l][0] = 0;
			out_fftw[l][1] = 0;
		}
	}
	else
	{
		/* odd number of data */
		for (l = 1; l < n2+1; l++)
		{
			out_fftw[l][0] *= 2.0;
			out_fftw[l][1] *= 2.0;
		}
		for (l = n2; l < num_data; l++)
		{
			out_fftw[l][0] = 0;
			out_fftw[l][1] = 0;
		}
	}

	p = fftw_plan_dft_1d(num_data, out_fftw, in_fftw, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);

	*re = malloc(sizeof(double) * num_data);
	*im = malloc(sizeof(double) * num_data);
	for (l = 0; l < num_data; l++)
	{
		/* result of inverse FFT is in in_fftw */
		(*re)[l] = in_fftw[l][0] / num_data;
		(*im)[l] = in_fftw[l][1] / num_data;
	}

	fftw_free(in_fftw);
	fftw_free(out_fftw);
	
	return 0;
} /* do_hilbert_transformation() */


/* int */
/* get_data(struct ra_respiration *opt, struct beat_infos **beats, long *num, int *rri_avail, int *ibi_avail) */
/* { */
/* 	int ret = 0; */

/* 	ret = get_beat_data(opt, beats, num, rri_avail, ibi_avail); */

/* 	return ret; */
/* } /\* get_data() *\/ */


/* int */
/* get_beat_data(struct ra_respiration *opt, struct beat_infos **beats, long *num, int *rri_avail, int *ibi_avail) */
/* { */
/* 	long l; */
/* 	prop_handle qrs_pos, rri, diast_pos; */
/* 	evset_handle set; */

/* 	qrs_pos = ra_prop_get_by_name(opt->eh, "qrs-pos"); */
/* 	rri = ra_prop_get_by_name(opt->eh, "rri"); */
/* 	diast_pos = ra_prop_get_by_name(opt->eh, "rr-diastolic-pos"); */

/* 	if (qrs_pos && rri) */
/* 		*rri_avail = 1; */
/* 	if (diast_pos) */
/* 		*ibi_avail = 1; */

/* 	if (!rri_avail && !ibi_avail) */
/* 		return -1; */

/* 	set = ra_evset_get_by_prop((qrs_pos ? qrs_pos : diast_pos)); */
/* 	if (!set) */
/* 		return -1; */

/* 	*num = ra_evset_get_num_events(set); */
/* 	*beats = calloc(*num, sizeof(struct beat_infos)); */
/* 	for (l = 0; l < *num; l++) */
/* 	{ */
/* 		if (*rri_avail) */
/* 		{ */
/* 			(*beats)[l].beat_rri_pos = (long)ra_ev_get_value(qrs_pos, l); */
/* 			(*beats)[l].interval_rri_prev = (long)ra_ev_get_value(rri, l); */
/* 			(*beats)[l].interval_rri_post = 0; */
/* 		} */
/* 		if (*ibi_avail) */
/* 		{ */
/* 			(*beats)[l].beat_ibi_pos = (long)ra_ev_get_value(diast_pos, l); */
/* 			if (l > 0) */
/* 				(*beats)[l].interval_ibi_prev = (*beats)[l].beat_ibi_pos - (*beats)[l-1].beat_ibi_pos; */
/* 			else */
/* 				(*beats)[l].interval_ibi_prev = 0; */
/* 			(*beats)[l].interval_ibi_post = 0; */
/* 		} */
/* 	} */

/* 	return 0; */
/* } /\* get_beat_data() *\/ */


double
get_samplerate(rec_handle rh, long ch)
{
	value_handle vh;
	double samplerate = 0.0;
	meas_handle mh;

	mh = ra_meas_handle_from_any_handle(rh);

	vh = ra_value_malloc();
	if (ch == -1)
	{
		ra_info_get(mh, RA_INFO_MAX_SAMPLERATE_D, vh);
		samplerate = ra_value_get_double(vh);
	}
	else
	{
		ra_value_set_number(vh, ch);
		ra_info_get(rh, RA_INFO_REC_CH_SAMPLERATE_D, vh);
		samplerate = ra_value_get_double(vh);
	}
	ra_value_free(vh);

	return samplerate;
} /* get_samplerate() */


/* int */
/* calc_resp(struct ra_respiration *opt, struct beat_infos *beats, long num, int rri_avail, int ibi_avail) */
/* { */
/* 	int ret = 0; */
	
/* 	ret = calc_beat_resp(opt, beats, num, rri_avail, ibi_avail); */

/* 	return ret; */
/* } /\* calc_resp() *\/ */


/* int */
/* calc_beat_resp(struct ra_respiration *opt, struct beat_infos *beats, long num, int rri_avail, int ibi_avail) */
/* { */
/* 	long l; */
/* 	long buf_size = 0; */
/* 	double *buf = NULL; */

/* 	buf_size = 10000; */
/* 	buf = malloc(sizeof(double) * buf_size); */
/* 	for (l = 0; l < num; l++) */
/* 	{ */
/* 		if (rri_avail) */
/* 		{ */
/* 			long len = beats[l].interval_rri_prev + beats[l].interval_rri_post; */
/* 			if (len > buf_size) */
/* 			{ */
/* 				buf = realloc(buf, sizeof(double) * len); */
/* 				buf_size = len; */
/* 			} */
/* 			if (len > 0) */
/* 			{ */
/* 				ra_raw_get_unit(opt->rh, opt->ch_num, beats[l].beat_rri_pos - beats[l].interval_rri_prev, len, buf); */
/* 				beats[l].mean_chest_rri = mean(buf, len); */
/* 			} */
/* 			else */
/* 				beats[l].mean_chest_rri = 0; */
/* 		} */

/* 		if (ibi_avail) */
/* 		{ */
/* 			long len = beats[l].interval_ibi_prev + beats[l].interval_ibi_post; */
/* 			if (len > buf_size) */
/* 			{ */
/* 				buf = realloc(buf, sizeof(double) * len); */
/* 				buf_size = len; */
/* 			} */
/* 			if (len > 0) */
/* 			{ */
/* 				ra_raw_get_unit(opt->rh, opt->ch_num, beats[l].beat_ibi_pos - beats[l].interval_ibi_prev, len, buf); */
/* 				beats[l].mean_chest_ibi = mean(buf, len); */
/* 			} */
/* 			else */
/* 				beats[l].mean_chest_ibi = 0; */
/* 		} */
/* 	} */

/* 	if (buf) */
/* 		free(buf); */

/* 	return 0; */
/* } /\* calc_beat_resp() *\/ */


double
mean(const double *data, long n)
{
	long l;
	double d = 0.0;

	for (l = 0; l < n; l++)
		d += data[l];
	d /= (double)n;

	return d;
} /* mean() */


int
store_results(struct proc_info *pi, struct phase_infos *phi)
{
	int ret = 0;
	struct ra_respiration *opt = NULL;
	value_handle *res;
	long *start = NULL;
	long *end = NULL;
	long *pos = NULL;
	double *phase_all = NULL;
	long *ch = NULL;
	long l, m, cnt;

	if (pi == NULL)
		return -1;
	opt = (struct ra_respiration *)pi->options;

	/* fill out result 'arrays' */
	res = pi->results;
	start = malloc(sizeof(long) * phi->num_phase_events);
	end = malloc(sizeof(long) * phi->num_phase_events);
	pos = malloc(sizeof(long) * phi->num_phase_events);
	phase_all = malloc(sizeof(double) * phi->num_phase_events * phi->num_ch);
	ch = malloc(sizeof(long) * phi->num_phase_events * phi->num_ch);

	for (l = 0; l < phi->num_phase_events; l++)
	{
		start[l] = phi->spi[l].start;
		end[l] = phi->spi[l].end;
		pos[l] = start[l] + (phi->spi[l].end - phi->spi[l].start)/2;
	}

	cnt = 0;
	for (m = 0; m < phi->num_ch; m++)
	{
		for (l = 0; l < phi->num_phase_events; l++)
		{
			phase_all[cnt] = phi->spi[l].phase[m];
			ch[cnt] = opt->ch[m];
			cnt++;
		}
	}

	ra_value_set_long_array(res[START_POS], start, phi->num_phase_events);
	ra_value_set_long_array(res[END_POS], end, phi->num_phase_events);
	ra_value_set_long_array(res[PHASE_POS], pos, phi->num_phase_events);
	ra_value_set_long_array(res[VALUE_CH], ch, cnt);
	ra_value_set_double_array(res[PHASE_VALUES], phase_all, cnt);

	/* and save the results in the eval file (when wanted) */
	if (opt->save_in_eval)
	{
		value_handle vh;

		if (opt->eh == NULL)
		{
			ret = -1;
			_ra_set_error(pi->ra, RA_ERR_INFO_MISSING, "plugin-respiration");
			goto error;
		}

		/* check if there are respiration event-class's in the evaluation
		   and delete them if there are any */
		vh = ra_value_malloc();
		ra_class_get(opt->eh, "respiration", vh);
		for (l = 0; l < (long)ra_value_get_num_elem(vh); l++)
			ra_class_delete((class_handle)ra_value_get_voidp_array(vh)[l]);
		ra_value_free(vh);

		ret = ra_eval_save_result(opt->eh, NULL, pi, 0);
	}

error:
	/* free used memory */
	if (start)
		free(start);
	if (end)
		free(end);
	if (pos)
		free(pos);
	if (phase_all)
		free(phase_all);
	if (ch)
		free(ch);

	return ret;
} /* store_results() */

