/**
 * \file estimate_ch_infos.c
 *
 * This file implements the functions to estimate the type of a recording-channel
 * from the channel name.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _LIBRASCH_BUILD
#include <ra_estimate_ch_infos.h>
#include <ra_priv.h>

int find_name(const char *name, char *part);
int isletter(char c);
int read_folder_ch_map(const char *folder, struct ch_map **map, int *num_entries);
int read_meas_ch_map(const char *filename, int is_dir_based, struct ch_map **map, int *num_entries);
int read_ch_map(FILE *fp, struct ch_map **map, int *num_entries);


/**
 * \brief estimate channel type
 * \param <ra> ra-handle, needed to check for system-wide ch-map
 * \param <folder> folder to look for the folder specific ch-map
 * \param <filename> filename of the measurement, needed to look for the meas-specific ch-map
 * \param <is_dir_based> flag if measurement files are stored in a folder
 * \param <name> channel name
 * \param <inf> this variable contains the estimated type
 *
 * This function tries to estimate the type of a recording channel from the channel name.
 */
LIBRAAPI int
ra_est_ch_type(ra_handle ra, const char *folder, const char *filename, int is_dir_based,
	       const char *name, struct ra_est_ch_infos *inf)
{
	int found = 0;
	int i;
	char *name_utf8;
	struct ch_map *ch_map = NULL;
	int num_entries = 0;
	struct librasch *lib = (struct librasch *)ra;
	
	if (name[0] == '\0')
		return 0;

	name_utf8 = malloc(strlen(name) * 2);
	local_to_utf8(name, name_utf8, strlen(name) * 2);

	/* first check measurement specific channel-map */
	if (filename != NULL)
	{
		read_meas_ch_map(filename, is_dir_based, &ch_map, &num_entries);
		for (i = 0; i < num_entries; i++)
		{
			if ((RA_STRNICMP(name_utf8, ch_map[i].ch_name, strlen(name_utf8)) == 0) &&
			    (ch_map[i].inf_idx >= 0))
			{
				memcpy(inf, &(_inf[ch_map[i].inf_idx]), sizeof(_inf[0]));
				found = 1;
				break;
			}
		}
	}

	/* when not found, check folder specific channel-map */
	if (!found && (folder != NULL))
	{
		read_folder_ch_map(folder, &ch_map, &num_entries);
		for (i = 0; i < num_entries; i++)
		{
			if ((RA_STRNICMP(name_utf8, ch_map[i].ch_name, strlen(name_utf8)) == 0) &&
			    (ch_map[i].inf_idx >= 0))
			{
				memcpy(inf, &(_inf[ch_map[i].inf_idx]), sizeof(_inf[0]));
				found = 1;
				break;
			}
		}
	}
	
	/* when not found, check system-wide channel-map */
	if (!found && lib)
	{
		for (i = 0; i < lib->config.num_ch_map_system; i++)
		{
			if ((RA_STRNICMP(name_utf8, lib->config.ch_map_system[i].ch_name, strlen(name_utf8)) == 0) &&
			    (lib->config.ch_map_system[i].inf_idx >= 0))
			{
				memcpy(inf, &(_inf[lib->config.ch_map_system[i].inf_idx]), sizeof(_inf[0]));
				found = 1;
				break;
			}
		}
	}

	/* when not found, check build-in channel-map */
	if (!found)
	{
		for (i = 0; i < _num_infs; i++)
		{
			found = find_name(name_utf8, _inf[i].name_contains);
			if (found)
			{
				memcpy(inf, &(_inf[i]), sizeof(_inf[0]));
				break;
			}
		}
	}

	free(name_utf8);
	free_ch_map(&ch_map, num_entries);

	return found;
} /* ra_est_ch_type() */


/**
 * \brief check if string contains text sequence 
 * \param <name> string which is searched
 * \param <part> string which is looked for
 *
 * The function checks if the string 'name' contains the text 'part'.
 */
int
find_name(const char *name, char *part)
{
	int found = 0;
	int i;

	char *buf = NULL;

	buf = malloc(strlen(part) + 1);
	memset(buf, 0, strlen(part) + 1);

	for (i = 0; i < (int) strlen(name) - 1; i++)
	{
		if ((i == 0) || (!isletter(name[i]) && isletter(name[i + 1])))
		{
			if (i == 0)
				strncpy(buf, &(name[i]), strlen(part));
			else
				strncpy(buf, &(name[i + 1]), strlen(part));

			if (RA_STRICMP(buf, part) == 0)
			{
				found = 1;
				break;
			}
		}
	}

	if (buf)
		free(buf);

	return found;
} /* find_name() */


/**
 * \brief checks if character is a letter
 * \param <c> channel name
 *
 * Returns 1 if the character 'c' is a letter.
 */
int
isletter(char c)
{
	if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
		return 1;

	return 0;
} /* isletter() */


int
read_folder_ch_map(const char *folder, struct ch_map **map, int *num_entries)
{
	int ret = 0;
	char fn[MAX_PATH_RA];
	FILE *fp;

	free_ch_map(map, *num_entries);
	*num_entries = 0;

	sprintf(fn, "%s%clibrasch.ch_map", folder, SEPARATOR);
	fp = fopen(fn, "r");
	if (!fp)
		return ret;

	ret = read_ch_map(fp, map, num_entries);
	fclose(fp);

	return ret;
} /* read_folder_ch_map() */


int
read_meas_ch_map(const char *filename, int is_dir_based, struct ch_map **map, int *num_entries)
{
	int ret = 0;
	char fn[MAX_PATH_RA];
	FILE *fp;

	free_ch_map(map, *num_entries);
	*num_entries = 0;

	if (is_dir_based)
		sprintf(fn, "%s%clibrasch.ch_map", filename, SEPARATOR);
	else
		sprintf(fn, "%s.ch_map", filename);
	fp = fopen(fn, "r");
	if (!fp)
		return ret;

	ret = read_ch_map(fp, map, num_entries);
	fclose(fp);

	return ret;
} /* read_meas_ch_map() */


int
free_ch_map(struct ch_map **map, int num_entries)
{
	int i;

	if (*map == NULL)
		return 0;

	for (i = 0; i < num_entries; i++)
	{
		if ((*map)[i].ch_name)
			free((*map)[i].ch_name);
	}
	free(*map);
	*map = NULL;

	return 0;
} /* free_ch_map() */


int
read_ch_map(FILE *fp, struct ch_map **map, int *num_entries)
{
	int ret;
	char line[MAX_PATH_RA];

	while (fgets(line, MAX_PATH_RA, fp))
		ret = find_ch_map_entry(line, map, num_entries);

	return 0;
} /* read_ch_map() */


int
find_ch_map_entry(char *string, struct ch_map **map, int *num_entries)
{
	int i, stop_looking;
	char *name_utf8 = NULL;
	char *p;

	p = strchr(string, '=');	
	if (!p)
		return -1;

	*p = '\0';
	if (strlen(string) <= 0)
		return -1;
	if (strlen(p+1) <= 0)
		return -1;

	name_utf8 = malloc(strlen(string) * 2);
	local_to_utf8(string, name_utf8, strlen(string) * 2);

	(*num_entries)++;
	*map = realloc(*map, sizeof(struct ch_map) * (*num_entries));
	(*map)[*num_entries-1].ch_name = name_utf8;
	(*map)[*num_entries-1].ch_type = RA_CH_TYPE_UNKNOWN;
	(*map)[*num_entries-1].inf_idx = -1;
	
	do
	{
		stop_looking = 1;
		for (i = 0; i < _num_infs; i++)
		{
			if ((RA_STRNICMP(_inf[i].name_contains, p+1, strlen(_inf[i].name_contains)) == 0) &&
				(((*map)[*num_entries-1].ch_type == 0) || ((*map)[*num_entries-1].ch_type == _inf[i].parent_type)))
			{
				(*map)[*num_entries-1].ch_type = _inf[i].type;
				(*map)[*num_entries-1].inf_idx = i;

				/* check if sub-types are possible */
				if (_inf[i].has_sub_types)
				{
					/* sub-type id has to come after the parent id*/
					p += (1 + strlen(_inf[i].name_contains));
					/* start from beginning to look for sub-type id */
					stop_looking = 0;
				}
				break;
			}
		}
	} while (!stop_looking);

	return 0;
} /* find_ch_map_entry() */

