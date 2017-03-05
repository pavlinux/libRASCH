/*----------------------------------------------------------------------------
 * beat_mem.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef BEAT_MEM_H
#define BEAT_MEM_H



/* offsets for the values in the beats2 struct */
#define BEATS_POS_L          0	/* once -  1 */
#define BEATS_FID_POS_L      1	/* once -  2 */
#define BEATS_CH_FOUND_L     2	/* once -  3 */
#define BEATS_EVENT_ID_L     3	/* once -  4 */

#define BEATS_BEAT_IN_CH_L   4	/* for each channel -  1 */
#define BEATS_MORPH_FLAGS_L  5	/* for each channel -  2 */
#define BEATS_P_TYPE_L	     6	/* for each channel -  3 */
#define BEATS_P_START_L	     7	/* for each channel -  4 */
#define BEATS_P_PEAK_1_L     8	/* for each channel -  5 */
#define BEATS_P_PEAK_2_L     9	/* for each channel -  6 */
#define BEATS_P_END_L	    10	/* for each channel -  7 */
#define BEATS_QRS_TYPE_L    11	/* for each channel -  8 */
#define BEATS_QRS_START_L   12	/* for each channel -  9 */
#define BEATS_Q_L			13	/* for each channel - 10 */
#define BEATS_R_L			14	/* for each channel - 11 */
#define BEATS_S_L			15	/* for each channel - 12 */
#define BEATS_RS_L			16	/* for each channel - 13 */
#define BEATS_QRS_END_L	    17	/* for each channel - 14 */
#define BEATS_T_TYPE_L	    18	/* for each channel - 15 */
#define BEATS_T_START_L	    19	/* for each channel - 16 */
#define BEATS_T_PEAK_1_L    20	/* for each channel - 17 */
#define BEATS_T_PEAK_2_L    21	/* for each channel - 18 */
#define BEATS_T_END_L	    22	/* for each channel - 19 */

#define BEATS_HR_D			 0	/* once - 1 */

#define BEATS_NOISE_D	     1	/* for each channel -  1 */
#define BEATS_P_WIDTH_D	     2	/* for each channel -  2 */
#define BEATS_QRS_WIDTH_D    3	/* for each channel -  3 */
#define BEATS_T_WIDTH_D	     4	/* for each channel -  4 */
#define BEATS_PQ_D			 5	/* for each channel -  5 */
#define BEATS_QT_D		     6	/* for each channel -  6 */
#define BEATS_QTC_D			 7	/* for each channel -  7 */

#define BEATS_NUM_ONCE_LONG  4
#define BEATS_NUM_ONCE_DBL   1
#define BEATS_NUM_ONCE       (BEATS_NUM_ONCE_LONG + BEATS_NUM_ONCE_DBL)

#define BEATS_NUM_CH_LONG   19
#define BEATS_NUM_CH_DBL     7
#define BEATS_NUM_CH         (BEATS_NUM_CH_LONG + BEATS_NUM_CH_DBL)

#define BEATS_NUM_DATA_LONG  (BEATS_NUM_ONCE_LONG + BEATS_NUM_CH_LONG)
#define BEATS_NUM_DATA_DBL   (BEATS_NUM_ONCE_DBL + BEATS_NUM_CH_DBL)

#define BEATS_NUM_DATA       (BEATS_NUM_ONCE + BEATS_NUM_CH)


/* forward declaration */
struct beats2_mem;

struct beats2
{
	struct beats2 *next;
	struct beats2 *prev;

	struct beats2_mem *master;

	long block;
	long block_element;
	
	long contiguous;	/* flag if beats are contiguous (=1) or maybe not (=0) */
}; /* struct beats2 */


struct beats2_mem
{
	long num_blocks;

	long num_element;
	struct beats2 **element_order;
	long curr_element;

	long element_size_l;
	long **l_values;
	long element_size_d;
	double **d_values;

	long curr_block;
	long curr_block_element;

	long num_ch;
	long index_l[BEATS_NUM_DATA_LONG];
	long index_d[BEATS_NUM_DATA_DBL];
}; /* struct beats2_mem */


/* VS6 don't understand 'inline' and there are some problems with inline prototypes
   (and I don't understand the problem completely). Therefore the ifdef construct here
   and in the c file. */
#ifdef WIN32

__inline void
BEAT_VALUE_SET_LONG(struct beats2 *b, long idx, long ch_idx, long value)
{
        long offset = b->block_element*b->master->element_size_l + b->master->index_l[idx] + ch_idx;
	b->master->l_values[b->block][offset] = value;
}

__inline void
BEAT_VALUE_SET_DOUBLE(struct beats2 *b, long idx, long ch_idx, double value)
{
        long offset = b->block_element*b->master->element_size_d + b->master->index_d[idx] + ch_idx;
	b->master->d_values[b->block][offset] = value;
}


__inline long
BEAT_VALUE_GET_LONG(struct beats2 *b, long idx, long ch_idx)
{
        long offset = b->block_element*b->master->element_size_l + b->master->index_l[idx] + ch_idx;
	return b->master->l_values[b->block][offset];
}

__inline double
BEAT_VALUE_GET_DOUBLE(struct beats2 *b, long idx, long ch_idx)
{
        long offset = b->block_element*b->master->element_size_d + b->master->index_d[idx] + ch_idx;
	return b->master->d_values[b->block][offset];
}

#else

inline void BEAT_VALUE_SET_LONG(struct beats2 *b, long idx, long ch_idx, long value);
inline void BEAT_VALUE_SET_DOUBLE(struct beats2 *b, long idx, long ch_idx, double value);
inline long BEAT_VALUE_GET_LONG(struct beats2 *b, long idx, long ch_idx);
inline double BEAT_VALUE_GET_DOUBLE(struct beats2 *b, long idx, long ch_idx);

#endif  /* WIN32 */


#define SET_BEAT_POS(b, val) BEAT_VALUE_SET_LONG(b, BEATS_POS_L, 0, val)
#define SET_BEAT_FID_POS(b, val) BEAT_VALUE_SET_LONG(b, BEATS_FID_POS_L, 0, val)
#define SET_CH_FOUND(b, val) BEAT_VALUE_SET_LONG(b, BEATS_CH_FOUND_L, 0, val)
#define SET_EVENT_ID(b, val) BEAT_VALUE_SET_LONG(b, BEATS_EVENT_ID_L, 0, val)
#define SET_HR(b, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_HR_D, 0, val)

#define SET_BEAT_IN_CH(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_BEAT_IN_CH_L, ch_idx, val)
#define SET_MORPH_FLAGS(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_MORPH_FLAGS_L, ch_idx, val)
#define SET_NOISE(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_NOISE_D, ch_idx, val)

#define SET_P_TYPE(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_P_TYPE_L, ch_idx, val)
#define SET_P_START(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_P_START_L, ch_idx, val)
#define SET_P_PEAK_1(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_P_PEAK_1_L, ch_idx, val)
#define SET_P_PEAK_2(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_P_PEAK_2_L, ch_idx, val)
#define SET_P_END(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_P_END_L, ch_idx, val)
#define SET_P_WIDTH(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_P_WIDTH_D, ch_idx, val)
#define SET_QRS_TYPE(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_QRS_TYPE_L, ch_idx, val)
#define SET_QRS_START(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_QRS_START_L, ch_idx, val)

/* #define SET_QRS_START(b, ch_idx, val) do { \ */
/* BEAT_VALUE_SET_LONG(b, BEATS_QRS_START_L, ch_idx, val); \ */
/*  fprintf(stderr, "qrs-start: %ld\n", val); \ */
/* } while (0) */

#define SET_Q(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_Q_L, ch_idx, val)
#define SET_R(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_R_L, ch_idx, val)
#define SET_S(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_S_L, ch_idx, val)
#define SET_RS(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_RS_L, ch_idx, val)
#define SET_QRS_END(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_QRS_END_L, ch_idx, val)
#define SET_QRS_WIDTH(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_QRS_WIDTH_D, ch_idx, val)
#define SET_T_TYPE(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_T_TYPE_L, ch_idx, val)
#define SET_T_START(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_T_START_L, ch_idx, val)
#define SET_T_PEAK_1(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_T_PEAK_1_L, ch_idx, val)
#define SET_T_PEAK_2(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_T_PEAK_2_L, ch_idx, val)
#define SET_T_END(b, ch_idx, val) BEAT_VALUE_SET_LONG(b, BEATS_T_END_L, ch_idx, val)
#define SET_T_WIDTH(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_T_WIDTH_D, ch_idx, val)

#define SET_PQ(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_PQ_D, ch_idx, val)
#define SET_QT(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_QT_D, ch_idx, val)
#define SET_QTC(b, ch_idx, val) BEAT_VALUE_SET_DOUBLE(b, BEATS_QTC_D, ch_idx, val)


#define GET_BEAT_POS(b) BEAT_VALUE_GET_LONG(b, BEATS_POS_L, 0)
#define GET_BEAT_FID_POS(b) BEAT_VALUE_GET_LONG(b, BEATS_FID_POS_L, 0)
#define GET_CH_FOUND(b) BEAT_VALUE_GET_LONG(b, BEATS_CH_FOUND_L, 0)
#define GET_EVENT_ID(b) BEAT_VALUE_GET_LONG(b, BEATS_EVENT_ID_L, 0)
#define GET_HR(b) BEAT_VALUE_GET_DOUBLE(b, BEATS_HR_D, 0)

#define GET_BEAT_IN_CH(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_BEAT_IN_CH_L, ch_idx)
#define GET_MORPH_FLAGS(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_MORPH_FLAGS_L, ch_idx)
#define GET_NOISE(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_NOISE_D, ch_idx)

#define GET_P_TYPE(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_P_TYPE_L, ch_idx)
#define GET_P_START(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_P_START_L, ch_idx)
#define GET_P_PEAK_1(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_P_PEAK_1_L, ch_idx)
#define GET_P_PEAK_2(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_P_PEAK_2_L, ch_idx)
#define GET_P_END(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_P_END_L, ch_idx)
#define GET_P_WIDTH(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_P_WIDTH_D, ch_idx)
#define GET_QRS_TYPE(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_QRS_TYPE_L, ch_idx)
#define GET_QRS_START(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_QRS_START_L, ch_idx)
#define GET_Q(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_Q_L, ch_idx)
#define GET_R(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_R_L, ch_idx)
#define GET_S(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_S_L, ch_idx)
#define GET_RS(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_RS_L, ch_idx)
#define GET_QRS_END(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_QRS_END_L, ch_idx)
#define GET_QRS_WIDTH(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_QRS_WIDTH_D, ch_idx)
#define GET_T_TYPE(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_T_TYPE_L, ch_idx)
#define GET_T_START(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_T_START_L, ch_idx)
#define GET_T_PEAK_1(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_T_PEAK_1_L, ch_idx)
#define GET_T_PEAK_2(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_T_PEAK_2_L, ch_idx)
#define GET_T_END(b, ch_idx) BEAT_VALUE_GET_LONG(b, BEATS_T_END_L, ch_idx)
#define GET_T_WIDTH(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_T_WIDTH_D, ch_idx)

#define GET_PQ(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_PQ_D, ch_idx)
#define GET_QT(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_QT_D, ch_idx)
#define GET_QTC(b, ch_idx) BEAT_VALUE_GET_DOUBLE(b, BEATS_QTC_D, ch_idx)


int init_beat_mem(struct beats2_mem *mem_info, long num_elem, long num_ch);
int free_beat_mem(struct beats2_mem *mem_info);
struct beats2 *get_beat_mem(struct beats2_mem *mem_info);
struct beats2 *get_first_beat(struct beats2_mem *mem_info);


#endif /* BEAT_MEM_H */
