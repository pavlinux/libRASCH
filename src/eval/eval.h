#ifndef EVAL_H
#define EVAL_H

/* extension added to property value column-name to create property-ch-number column */
#define CH_PROP_COL_ADD   "_ch"


/* ---------- eval.c ---------- */
int do_transaction_step(any_handle h, const char *command);

int set_env(struct eval *e);

long  get_summary_data_id(sum_handle sh, long sum_elem_key, long channel, long dim);
int split(const char *string, char *sep, char ***elem, const char *ignore);
int join(char **elem, int n, char *string, size_t len, char *sep);

#endif  /* EVAL_H */
