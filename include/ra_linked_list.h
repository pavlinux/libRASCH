/**
 * \file ra_linked_list.h
 *
 * The header file contains needed infos for linked-list implementation in libRASCH.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_LINKED_LIST_H
#define RA_LINKED_LIST_H

#include <ra.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  struct's which are handled with this functions must have the following format:

  first field: used to identify struct (not used in linked-list)
  second field: pointer to next struct
  third field: pointer to previous struct

  
  The remaining fields contain the struct-specific data and are not accessed with
  this functions.
*/

/**
 * \struct linked_list
 * \brief the elements of a structure needed for a linked-list
 * \param <not_used> not used
 * \param <next> pointer to the next element
 * \param <prev> pointer to the previous element
 *
 * If a structure wants to be a libRASCH-handled linked-list, the first elements
 * of the structure must contains the members of this structure.
 */
struct linked_list
{
	unsigned short not_used; /* some list-elements are returned to user-space as handles,
				    the type of the handle are set in this var */

	struct linked_list *next;
	struct linked_list *prev;
};				/* struct linked_list */


/* TODO: think about if these prototypes should be moved to ra.h */
LIBRAAPI int ra_list_add(void **head, void *item);
LIBRAAPI int ra_list_insert(void **head, void *prev, void *item);
LIBRAAPI int ra_list_del(void **head, void *item);
LIBRAAPI int ra_list_len(void *head);

#ifdef __cplusplus
}
#endif

#endif /* RA_LINKED_LIST_H */
