/*----------------------------------------------------------------------------
 * beat_mem.c
 *
 * When processing Holter ECGs, allocating the memory for each beat and data-value 
 * is very inefficient, the overhead is high and a lot of memory is allocated
 * (especially when analysing 24 hour 12-lead recordings).
 *
 * -> allocate memory in big blocks and access the specific data values
 *    by offsets; the beats remain organized in a double linked list, but the
 *    memory for the beat values is in one (or more) big memory-block(s)
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "beat_mem.h"


int
init_beat_mem(struct beats2_mem *mem_info, long num_elem, long num_ch)
{
	long l, cnt, cnt_idx;

	mem_info->num_element = num_elem;
	mem_info->element_order = malloc(sizeof(struct beats2 *));
	mem_info->element_order[0] = calloc(num_elem, sizeof(struct beats2));
	mem_info->curr_element = 0;

	mem_info->num_blocks = 1;
	mem_info->element_size_l = BEATS_NUM_ONCE_LONG + BEATS_NUM_CH_LONG*num_ch;
	mem_info->l_values = malloc(sizeof(long *));
	mem_info->l_values[0] = malloc(mem_info->element_size_l * sizeof(long) * num_elem);
	memset(mem_info->l_values[0], 0, mem_info->element_size_l * sizeof(long) * num_elem);

	mem_info->element_size_d = BEATS_NUM_ONCE_DBL + BEATS_NUM_CH_DBL*num_ch;
	mem_info->d_values = malloc(sizeof(double *));
	mem_info->d_values[0] = malloc(mem_info->element_size_d * sizeof(double) * num_elem);
	memset(mem_info->d_values[0], 0, mem_info->element_size_d * sizeof(double) * num_elem);

	mem_info->curr_block = 0;
	mem_info->curr_block_element = 0;

	mem_info->num_ch = num_ch;

	cnt = cnt_idx = 0;
	for (l = 0; l < BEATS_NUM_ONCE_LONG; l++)
	{
		mem_info->index_l[cnt_idx++] = cnt;
		cnt++;
	}
	for (l = 0; l < BEATS_NUM_CH_LONG; l++)
	{
		mem_info->index_l[cnt_idx++] = cnt;
		cnt += num_ch;
	}

	cnt = cnt_idx = 0;
	for (l = 0; l < BEATS_NUM_ONCE_DBL; l++)
	{
		mem_info->index_d[cnt_idx++] = cnt;
		cnt++;
	}
	for (l = 0; l < BEATS_NUM_CH_DBL; l++)
	{
		mem_info->index_d[cnt_idx++] = cnt;
		cnt += num_ch;
	}

	return 0;
} /* init_beat_mem() */


int
free_beat_mem(struct beats2_mem *mem_info)
{
	long l;

	for (l = 0; l < mem_info->num_blocks; l++)
	{
		free(mem_info->element_order[l]);
		free(mem_info->l_values[l]);
		free(mem_info->d_values[l]);
	}
	free(mem_info->element_order);
	free(mem_info->l_values);
	free(mem_info->d_values);

	return 0;
} /* free_beat_mem() */


struct beats2*
get_beat_mem(struct beats2_mem *mem_info)
{
	struct beats2 *ret = NULL;

	if (mem_info->curr_element >= mem_info->num_element)
	{
		long add_num;

		add_num = (long)((double)(mem_info->num_element) * 0.1);
		if (add_num < 10)
			add_num += 10;

		mem_info->num_blocks += 1;

		mem_info->element_order = realloc(mem_info->element_order, sizeof(struct beats2 *) * mem_info->num_blocks);
		mem_info->element_order[mem_info->num_blocks-1] = calloc(add_num, sizeof(struct beats2));
		mem_info->num_element += add_num;

		mem_info->l_values = realloc(mem_info->l_values, sizeof(long *) * mem_info->num_blocks);
		mem_info->l_values[mem_info->num_blocks - 1] = malloc(mem_info->element_size_l * sizeof(long) * add_num);
		memset(mem_info->l_values[mem_info->num_blocks - 1], 0, mem_info->element_size_l * sizeof(long) * add_num);

		mem_info->d_values = realloc(mem_info->d_values, sizeof(double *) * mem_info->num_blocks);
		mem_info->d_values[mem_info->num_blocks - 1] = malloc(mem_info->element_size_d * sizeof(double) * add_num);
		memset(mem_info->d_values[mem_info->num_blocks - 1], 0, mem_info->element_size_d * sizeof(double) * add_num);

		mem_info->curr_block = mem_info->num_blocks - 1;
		mem_info->curr_block_element = 0;
	}

	ret = mem_info->element_order[mem_info->curr_block] + mem_info->curr_block_element;
	mem_info->curr_element += 1;

	ret->master = mem_info;
	ret->block = mem_info->curr_block;
	ret->block_element = mem_info->curr_block_element;
	mem_info->curr_block_element += 1;

	ret->next = ret->prev = NULL;

	return ret;
} /* get_beat_mem() */


struct beats2 *
get_first_beat(struct beats2_mem *mem_info)
{
	struct beats2 *b;

	if (mem_info->curr_element == 0)
		return NULL;

	b = mem_info->element_order[0];
	if (b == NULL)
		return NULL;

	while (b->prev != NULL)
		b = b->prev;

	return b;
} /* get_first_beat() */


#ifndef WIN32			/* see comment in beat_mem.h */

inline void
BEAT_VALUE_SET_LONG(struct beats2 *b, long idx, long ch_idx, long value)
{
        long offset = b->block_element*b->master->element_size_l + b->master->index_l[idx] + ch_idx;
	b->master->l_values[b->block][offset] = value;
}

inline void
BEAT_VALUE_SET_DOUBLE(struct beats2 *b, long idx, long ch_idx, double value)
{
        long offset = b->block_element*b->master->element_size_d + b->master->index_d[idx] + ch_idx;
	b->master->d_values[b->block][offset] = value;
}


inline long
BEAT_VALUE_GET_LONG(struct beats2 *b, long idx, long ch_idx)
{
        long offset = b->block_element*b->master->element_size_l + b->master->index_l[idx] + ch_idx;
	return b->master->l_values[b->block][offset];
}

inline double
BEAT_VALUE_GET_DOUBLE(struct beats2 *b, long idx, long ch_idx)
{
        long offset = b->block_element*b->master->element_size_d + b->master->index_d[idx] + ch_idx;
	return b->master->d_values[b->block][offset];
}

#endif /* WIN32 */
