#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#ifdef __cplusplus
extern "C" {
#endif


#ifndef PI
#define PI 3.14159265358979323846
#endif /* PI */


int fir_filter_design(long order, double cut_low, double cut_high, double **b, long *num);
int filter(double *b, double *a, long order, double *data, long num_samples, int direction);
int filtfilt(double *b, double *a, long order, double *data, long num_samples);
int interp(double *in, double *out, long num_samples, long factor);


#ifdef __cplusplus
}
#endif

#endif  /* SIGNAL_PROCESSING_H */
