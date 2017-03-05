#ifndef GET_PEAKS_H
#define GET_PEAKS_H

#include "main_detect.h"

int get_peaks(struct data_buffer *buf, double *sig_use, long start, long num,
	      double threshold, double area_filt, struct peak_buffer **peaks, long *num_peaks,
	      long *morph_type, long force_morph_type, long ms30, int allow_look_before, int allow_look_after);


int get_morph_type_info(long morph_type, long *num_peaks, long **peak_positive);
int find_peaks(struct data_buffer *buf, double *sig_use, long start, long num,
	       double threshold, double area_filt, struct peak_buffer **peaks, long *num_peaks,
	       long *morph_type, long ms30, int allow_look_before, int allow_look_after);


#endif /* GET_PEAKS_H */
