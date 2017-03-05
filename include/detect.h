#ifndef _DETECT_H
#define _DETECT_H

extern "C"
{
#include <ra.h>
}


#define READ_BUFFER_SIZE  50000
#define ADD_PERCENT      0.5
#define SUB_PERCENT      0.001
#define BOXCAR_SIZE      20
#define BOXCAR_SIZE2     20

#define BEAT_RANGE2      80


struct read_buffer_struct
{
	meas_handle mh;
	size_t sig_pos;
	rec_handle rh;
	int ch;
	int num;
	int curr;
	double buf[READ_BUFFER_SIZE];
};


struct detect_info_struct
{
	meas_handle mh;
	int num_ch;
	bool *use_ch;
	int *type;
};

struct beats
{
	struct beats *next;
	struct beats *prev;

	long pos;

	long ch_found;		// each bit represents a channel where a beat was found
};

struct detect_result
{
	int num_beats;		// num of detected beats
	struct beats *b;	// detected beats

	int num_ch;		// num of channels used for detection
	int *ch_in_sig;		// ch in struct beats corresponds to channel in signal
};


int detect(struct detect_info_struct *di, struct detect_result *dr);
int ch_detect(meas_handle mh, rec_handle rh, int ch, int *n, struct beats **bhead);
int read_buffer_init(meas_handle mh, rec_handle rh, int ch, struct read_buffer_struct *rb);
int read_buffer(struct read_buffer_struct *rb, double *buf, int num);
struct beats *combine_beats(struct detect_info_struct *di, int num_ch, struct beats **b);

#endif // _DETECT_H
