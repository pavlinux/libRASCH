#ifndef RA_XS_H
#define RA_XS_H


#include <ra.h>
#include "../shared/infos.h"

/*
void
value_to_stack(value_handle v)
{
	EXTEND(SP, 3);
	switch (ra_value_get_type(v))
	{
	case RA_VALUE_TYPE_SHORT:
		PUSHs(sv_2mortal(newSViv(ra_value_get_short(v))));
		break;
	case RA_VALUE_TYPE_LONG:
		PUSHs(sv_2mortal(newSViv(ra_value_get_long(v))));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		PUSHs(sv_2mortal(newSVnv(ra_value_get_double(v))));
		break;
	case RA_VALUE_TYPE_CHAR:
		PUSHs(sv_2mortal(newSVpv(ra_value_get_string(v), strlen(ra_value_get_string(v)))));
		break;
	}
	PUSHs(sv_2mortal(newSVpv(ra_value_get_name(v), strlen(ra_value_get_name(v)))));
	PUSHs(sv_2mortal(newSVpv(ra_value_get_desc(v), strlen(ra_value_get_desc(v)))));

	if (ra_value_get_type(v) == RA_VALUE_TYPE_CHAR)
	{
		EXTEND(SP, 1);
		PUSHs(sv_2mortal(newSVpv(ra_value_get_string_utf8(v), strlen(ra_value_get_string_utf8(v)))));
	}
}*/ /* value_to_stack() */


#define VALUE_TO_STACK(v)  \
do {  \
	EXTEND(SP, 4);  \
	switch (ra_value_get_type(v))  \
	{  \
	case RA_VALUE_TYPE_SHORT:  \
		PUSHs(sv_2mortal(newSViv(ra_value_get_short(v))));  \
		break;  \
	case RA_VALUE_TYPE_LONG:  \
		PUSHs(sv_2mortal(newSViv(ra_value_get_long(v))));  \
		break;  \
	case RA_VALUE_TYPE_DOUBLE:  \
		PUSHs(sv_2mortal(newSVnv(ra_value_get_double(v))));  \
		break;  \
	case RA_VALUE_TYPE_CHAR:  \
		PUSHs(sv_2mortal(newSVpv(ra_value_get_string(v), strlen(ra_value_get_string(v)))));  \
		break;  \
	}  \
	PUSHs(sv_2mortal(newSVpv(ra_value_get_name(v), strlen(ra_value_get_name(v)))));  \
	PUSHs(sv_2mortal(newSVpv(ra_value_get_desc(v), strlen(ra_value_get_desc(v)))));  \
	if (ra_value_info_modifiable(v)) \
		PUSHs(sv_2mortal(newSViv(1))); \
	else \
		PUSHs(sv_2mortal(newSViv(0))); \
	if (ra_value_get_type(v) == RA_VALUE_TYPE_CHAR)  \
	{  \
		EXTEND(SP, 1);  \
		PUSHs(sv_2mortal(newSVpv(ra_value_get_string_utf8(v), strlen(ra_value_get_string_utf8(v)))));  \
	}  \
} while(0)



long
get_info_id(char *name)
{
	long id = -1;
	long l;
	
	for (l = 0; l < (long)(sizeof(infos)/sizeof(infos[0])); l++)
	{
		if (infos[l].name == NULL)
			continue;
		if (strcmp(name, infos[l].name) == 0)
		{
			id = infos[l].id;
			break;
		}
	}

	return id;
} /* get_info_id() */


long
get_info_type(char *name)
{
	long type = -1;

	if (strcmp(name, "meas") == 0)
		type = RA_INFO_OBJECT;
	else if (strcmp(name, "rec") == 0)
		type = RA_INFO_REC_GENERAL;
	else if (strcmp(name, "dev") == 0)
		type = RA_INFO_REC_DEVICE;
	else if (strcmp(name, "ch") == 0)
		type = RA_INFO_REC_CHANNEL;

	return type;
} /* get_info_type() */


SV *
value_to_sv(value_handle vh)
{
	SV *val = NULL;
	AV *val_arr = NULL;
	long l, n;
	const short *sp;
	const long *lp;
	const double *dp;
	const char **cp;
	const void **vp;

	n = ra_value_get_num_elem(vh);
	switch (ra_value_get_type(vh))
	{
	case RA_VALUE_TYPE_SHORT:
		val = newSViv(ra_value_get_short(vh));
		break;
	case RA_VALUE_TYPE_LONG:
		val = newSViv(ra_value_get_long(vh));
		break;
	case RA_VALUE_TYPE_DOUBLE:
		val = newSVnv(ra_value_get_double(vh));
		break;
	case RA_VALUE_TYPE_CHAR:
		val = newSVpv(ra_value_get_string(vh), strlen(ra_value_get_string(vh)));
		break;
	case RA_VALUE_TYPE_VOIDP:
		val = newSViv((long)(ra_value_get_voidp(vh)));
		break;
	case RA_VALUE_TYPE_SHORT_ARRAY:
		val_arr = newAV();
		sv_2mortal((SV *)val_arr);
		sp = ra_value_get_short_array(vh);
		for (l = 0; l < n; l++)
			av_store(val_arr, l, newSViv(sp[l]));
		val = newRV((SV *)val_arr);
		break;
	case RA_VALUE_TYPE_LONG_ARRAY:
		val_arr = newAV();
		sv_2mortal((SV *)val_arr);
		lp = ra_value_get_long_array(vh);
		for (l = 0; l < n; l++)
			av_store(val_arr, l, newSViv(lp[l]));
		val = newRV((SV *)val_arr);
		break;
	case RA_VALUE_TYPE_DOUBLE_ARRAY:
		val_arr = newAV();
		sv_2mortal((SV *)val_arr);
		dp = ra_value_get_double_array(vh);
		for (l = 0; l < n; l++)
			av_store(val_arr, l, newSVnv(dp[l]));
		val = newRV((SV *)val_arr);
		break;
	case RA_VALUE_TYPE_CHAR_ARRAY:
		val_arr = newAV();
		sv_2mortal((SV *)val_arr);
		cp = ra_value_get_string_array(vh);
		for (l = 0; l < n; l++)
			av_store(val_arr, l, newSVpv(cp[l], strlen(cp[l])));
		val = newRV((SV *)val_arr);
		break;
	case RA_VALUE_TYPE_VOIDP_ARRAY:
		val_arr = newAV();
		sv_2mortal((SV *)val_arr);
		vp = ra_value_get_voidp_array(vh);
		for (l = 0; l < n; l++)
		{
			long p = (long)(vp[l]);
			av_store(val_arr, l, newSViv(p));
		}
		val = newRV((SV *)val_arr);
		break;
	}

	return val;
} /* value_to_sv() */


int
sv_to_value(SV *value, char *type, value_handle vh)
{
	int ret = 0;

	if (type[0] == 'p')  /* check if it's an array */
	{
		long len, l;
		AV *arr;
		SV **elem;

		if (SvROK(value) && SvTYPE(SvRV(value)) == SVt_PVAV)
			arr = (AV*)SvRV(value);
		else
			return -1;
		len = av_len(arr) + 1;
	
		if (type[1] == 's')  /* short array? */
		{
			short *all = malloc(sizeof(short) * len);
			for (l = 0; l < len; l++)
			{
				elem = av_fetch(arr, l, 0);
				all[l] = (short)SvIV(*elem);
			}
			ra_value_set_short_array(vh, all, len);
			
			free(all);
		}
		else if (type[1] == 'l')  /* long array? */
		{
			long *all = malloc(sizeof(long) * len);
			for (l = 0; l < len; l++)
			{
				elem = av_fetch(arr, l, 0);
				all[l] = (long)SvIV(*elem);
			}
			ra_value_set_long_array(vh, all, len);
			
			free(all);
		}
		else if (type[1] == 'h')  /* handle array? */
		{
			void **all = malloc(sizeof(void *) * len);
			for (l = 0; l < len; l++)
			{
				elem = av_fetch(arr, l, 0);
				all[l] = (void *)SvIV(*elem);
			}
			ra_value_set_voidp_array(vh, (const void **)all, len);
			
			free(all);
		}
		else if (type[1] == 'd')  /* or double array? */
		{
			double *all = malloc(sizeof(double) * len);
			for (l = 0; l < len; l++)
			{
				elem = av_fetch(arr, l ,0);
				all[l] = (double)SvNV(*elem);
			}
			ra_value_set_double_array(vh, all, len);
			
			free(all);
		}
		else if (type[1] == 'c')  /* or string array? */
		{
			STRLEN str_len;
			char *str;
			char **all = malloc(sizeof(char *) * len);
				
			for (l = 0; l < len; l++)
			{
				elem = av_fetch(arr, l ,0);
				str = (char *)SvPV(*elem, str_len);
				all[l] = malloc(sizeof(char) * (str_len+1));
				strncpy(all[l], str, len);
			}
			if (type[2] == 'u')
				ra_value_set_string_array_utf8(vh, (const char **)all, len);
			else
				ra_value_set_string_array(vh, (const char **)all, len);
			
			for (l = 0; l < len; l++)
				free(all[l]);
			free(all);
		}
	}
	else if (type[0] == 's') /* is it a short? */
	{
		short val = (short)SvIV(value);
		
		ra_value_set_short(vh, val);
	}
	else if (type[0] == 'l') /* is it a long? */
	{
		long val = (long)SvIV(value);
		
		ra_value_set_long(vh, val);
	}
	else if (type[0] == 'h') /* is it a handle? */
	{
		void *val = (void *)SvIV(value);
		
		ra_value_set_voidp(vh, val);
	}
	else if (type[0] == 'd') /* is it a double? */
	{
		double val = (double)SvNV(value);

		ra_value_set_double(vh, (double)val);
	}
	else if (type[0] == 'c') /* is it a string? */
	{
		STRLEN len;
		char *str;
		
		str = (char *)SvPV(value, len);

		if (type[1] == 'u')
			ra_value_set_string(vh, str);
		else
			ra_value_set_string_utf8(vh, str);
	}
	else
		ret = -1;
	
	return ret;
} /* sv_to_value() */


#endif /* RA_XS_H */
