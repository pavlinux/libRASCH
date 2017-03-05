/**
 * \file linked_list.c
 *
 * This file implements the functions to handle double-linked-lists. The functions
 * assume that the first three structure-elements of the data, managed with this
 * linked-list functions, are defined as in ra_linked_list.h described.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos/librasch/src/linked_list.c,v 1.4 2004/02/19 14:42:12 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>

#define _LIBRASCH_BUILD
#include <ra_linked_list.h>


/**
 * \brief add an entry to a linked list
 * \param <head> head of linked list
 * \param <item> item to be added
 *
 * This function adds 'item' to the linked list identified by 'head' (head of list).
 */
int
ra_list_add(void **head, void *item)
{
	struct linked_list *h;
	struct linked_list *i;

	if ((head == NULL) || (item == NULL))
		return -1;

	h = (struct linked_list *) *head;
	i = (struct linked_list *) item;

	if (h == NULL)
	{
		i->next = i->prev = NULL;
		*head = item;

		return 0;
	}

	do
	{
		if (h->next == NULL)
		{
			h->next = i;
			i->prev = h;
			i->next = NULL;

			return 0;
		}

		h = h->next;
	}
	while (h != NULL);

	return -1;		/* if func. comes here, something went wrong */
} /* ra_list_add() */


/**
 * \brief insert an entry to a linked list
 * \param <head> head of linked list
 * \param <prev> previous item
 * \param <item> item to be added
 *
 * This function inserts 'item' in a linked list (identified by 'head').
 * The item will come directly after 'prev'.
 */
int
ra_list_insert(void **head, void *prev, void *item)
{
	struct linked_list *h;
	struct linked_list *p;
	struct linked_list *i;

	h = (struct linked_list *) *head;
	p = (struct linked_list *) prev;
	i = (struct linked_list *) item;

	if ((head == NULL) || (*head == NULL) || (i == NULL))
		return -1;

	if (p == NULL)
	{
		i->next = h;
		i->prev = NULL;
		h->prev = i;
		*head = item;
	}
	else
	{
		i->next = p->next;
		i->prev = p;

		p->next = i;

		if (i->next != NULL)
			i->next->prev = item;
	}

	return 0;
} /* ra_list_insert() */


/**
 * \brief delete an entry from a linked list
 * \param <head> head of linked list
 * \param <item> item to be deleted
 *
 * This function deletes 'item' from a linked list (identified by 'head').
 */
int
ra_list_del(void **head, void *item)
{
	struct linked_list *h;
	struct linked_list *i;

	if ((head == NULL) || (*head == NULL) || (item == NULL))
		return -1;

	h = (struct linked_list *) *head;
	i = (struct linked_list *) item;

	if (h == i)
	{
		*head = h->next;
		if (h->next != NULL)
			h->next->prev = NULL;
	}
	else
	{
		i->prev->next = i->next;
		if (i->next != NULL)
			i->next->prev = i->prev;
	}

	return 0;
} /* ra_list_del() */


int
ra_list_len(void *head)
{
	struct linked_list *h = (struct linked_list *)head;
	int cnt;

	cnt = 0;
	if (h == NULL)
		return cnt;

	while (h)
	{
		cnt++;
		h = h->next;
	}

	return cnt;
} /* ra_list_len() */
