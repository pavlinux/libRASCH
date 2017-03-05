/**
 * \file pl_comm.c
 *
 * This file implements the inter-plugin-communication.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/src/pl_comm.c,v 1.6 2004/06/22 13:43:45 rasch Exp $
 *
 *--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define _LIBRASCH_BUILD
#include <ra_pl_comm.h>
#include <ra_priv.h>

#ifdef WIN32
	/* Using fprintf() and printf() from libintl will crash on Win32.
	   The reason is maybe using different c-libs. */
#undef fprintf
#undef printf
#endif /* WIN32 */

/* for debugging */
void _trace_signame (int level, const char *sender, const char *sig_name,
		     int num_para, struct comm_para *para);
void _trace_sigdest (int level, const char *dest);


/**
 * \brief add a signal receiver
 * \param <mh> measurement handle
 * \param <p> plugin handle
 * \param <dh> destination handle
 * \param <sig_name> name of the signal
 *
 * This function adds a new signal receiver p and dh for the signal sig_name to the
 * inter-plugin-communication. Whenever the signal sig_name is emitted, the signal will
 * be send to the plugin p. The plugin-instance will be identified with the help of dh.
 */
LIBRAAPI int
ra_comm_add (meas_handle mh, plugin_handle p, dest_handle dh,
	     const char *sig_name)
{
  struct ra_meas *mm = (struct ra_meas *) mh;
  struct signal *sig = find_signal (&(mm->plc), sig_name);

  if (!sig)
    {
      if ((sig = add_signal (&(mm->plc), sig_name)) == NULL)
	return -1;
    }

  sig->num_slots++;
  sig->slot = realloc (sig->slot, sizeof (struct slot_info) * sig->num_slots);
  sig->slot[sig->num_slots - 1].plugin = p;
  sig->slot[sig->num_slots - 1].dest = dh;

  return 0;
}				/* ra_comm_add() */


/**
 * \brief delete a signal receiver
 * \param <mh> measurement handle
 * \param <dh> destination handle
 * \param <sig_name> name of the signal
 *
 * This function deletes the signal receiver dh for the signal sig_name.
 */
LIBRAAPI void
ra_comm_del (meas_handle mh, dest_handle dh, const char *sig_name)
{
  int idx;
  struct ra_meas *mm = (struct ra_meas *) mh;
  struct signal *sig;

  if (mh == NULL)
    return;

  if (sig_name == NULL)
    {
      int i;
      for (i = 0; i < mm->plc.num_signals; i++)
	ra_comm_del (mh, dh, mm->plc.sig[i].name);

      return;
    }

  sig = find_signal (&(mm->plc), sig_name);
  if (!sig)
    return;

  idx = find_slot (sig, dh);
  if (idx < 0)
    return;

  if (idx < (sig->num_slots - 1))
    memmove (&(sig->slot[idx]), &(sig->slot[idx + 1]),
	     sizeof (struct slot_info) * (sig->num_slots - idx - 1));
  sig->num_slots--;
  sig->slot = realloc (sig->slot, sizeof (struct slot_info) * sig->num_slots);
}				/* ra_comm_del() */


/**
 * \brief emits a signal
 * \param <mh> measurement handle
 * \param <sender> name of the sender (needed for debug, can be NULL)
 * \param <sig_name> name of the signal
 * \param <num_para> number of signal parameters
 * \param <para> signal parameters
 *
 * This function emits the signal sig_name wit num_para parameters.
 */
LIBRAAPI void
ra_comm_emit (meas_handle mh, const char *sender, const char *sig_name,
	      int num_para, struct comm_para *para)
{
  int i;
  struct ra_meas *mm = (struct ra_meas *) mh;
  struct signal *sig = find_signal (&(mm->plc), sig_name);
  static int level = 0;

  if (!sig)
    return;

  level++;
  _trace_signame (level, sender, sig_name, num_para, para);

  for (i = 0; i < sig->num_slots; i++)
    {
      struct plugin_struct *p =
	(struct plugin_struct *) (sig->slot[i].plugin);
      if (!p)
	continue;

      if (p->handle_signal)
	{
	  _trace_sigdest (level, p->info.name);
	  (*p->handle_signal) (mh, sig->slot[i].dest, sig_name, num_para,
			       para);
	}
    }
  level--;

  return;
}				/* ra_comm_emit() */


/**
 * \brief look for a signal
 * \param <plc> pointer to a plugin-communication struct
 * \param <sig_name> name of the signal looked for
 *
 * The functions looks if the signal 'sig_name' is set in 'plc' and
 * returns a pointer to it.
 */
struct signal *
find_signal (struct plugin_comm *plc, const char *sig_name)
{
  int i;
  struct signal *sig = NULL;

  for (i = 0; i < plc->num_signals; i++)
    {
      if (strcmp (plc->sig[i].name, sig_name) == 0)
	{
	  sig = &(plc->sig[i]);
	  break;
	}
    }

  return sig;
}				/* find_signal() */


/**
 * \brief add a signal
 * \param <plc> pointer to a plugin-communication struct
 * \param <sig_name> name of the signal to be added
 *
 * The functions adds the signal 'sig_name' to 'plc'.
 */
struct signal *
add_signal (struct plugin_comm *plc, const char *sig_name)
{
  /* first be sure that signal is not already created */
  struct signal *sig = find_signal (plc, sig_name);
  if (sig)
    return sig;

  plc->num_signals++;
  plc->sig = realloc (plc->sig, sizeof (struct signal) * plc->num_signals);
  sig = &(plc->sig[plc->num_signals - 1]);
  strncpy (sig->name, sig_name, MAX_SIGNAL_NAME);
  sig->num_slots = 0;
  sig->slot = NULL;

  return sig;
}				/* add_signal() */


/**
 * \brief checks if a signal can be handled
 * \param <sig> pointer to a signal
 * \param <dh> handle to a signal-destination
 *
 * The functions checks if the signal 'sig' can be send to 'dh'.
 */
int
find_slot (struct signal *sig, dest_handle dh)
{
  int idx = -1;
  int i;

  for (i = 0; i < sig->num_slots; i++)
    {
      if (sig->slot[i].dest == dh)
	{
	  idx = i;
	  break;
	}
    }

  return idx;
}				/* find_slot() */


/**
 * \brief print infos about a signal (for debug)
 * \param <level> level of the current signal
 * \param <sender> name of the sender
 * \param <sig_name> name of the signal
 * \param <num_para> number of parameters
 * \param <para> pointer to a communication-parameter struct
 *
 * For debugging purposes. The function prints debug information about a signal. Is only
 * called when the define 'TRACE_SIGNAL' is set when compiling this source.
 */
void
_trace_signame (int level, const char *sender, const char *sig_name,
		int num_para, struct comm_para *para)
{
  int i;
  char *sender_locale;
  char *sig_name_locale;
  char *temp;

  if (!ra_print_debug_infos)
    return;

  for (i = 0; i < (level - 1); i++)
    fprintf (stderr, "  ");

  if (sender != NULL)
    {
      sender_locale = malloc (strlen (sender) * 2);
      utf8_to_local (sender, sender_locale, strlen (sender) * 2);
      fprintf (stderr, "%s->", sender_locale);
      free (sender_locale);
    }

  sig_name_locale = malloc (strlen (sig_name) * 2);
  utf8_to_local (sig_name, sig_name_locale, strlen (sig_name) * 2);
  fprintf (stderr, "signal %s: ", sig_name_locale);
  free (sig_name_locale);

  for (i = 0; i < num_para; i++)
    {
      switch (para[i].type)
	{
	case PARA_CHAR:
	  fprintf (stderr, "%c  ", para[i].v.c);
	  break;
	case PARA_BYTE:
	  fprintf (stderr, "%c  ", para[i].v.b);
	  break;
	case PARA_SHORT:
	  fprintf (stderr, "%d  ", para[i].v.sn);
	  break;
	case PARA_UNSIGNED_SHORT:
	  fprintf (stderr, "%d  ", para[i].v.usn);
	  break;
	case PARA_INT:
	  fprintf (stderr, "%d  ", para[i].v.n);
	  break;
	case PARA_UNSIGNED_INT:
	  fprintf (stderr, "%d  ", para[i].v.un);
	  break;
	case PARA_LONG:
	  fprintf (stderr, "%ld  ", para[i].v.l);
	  break;
	case PARA_UNSIGNED_LONG:
	  fprintf (stderr, "%ld  ", para[i].v.ul);
	  break;
	case PARA_FLOAT:
	  fprintf (stderr, "%f  ", para[i].v.f);
	  break;
	case PARA_DOUBLE:
	  fprintf (stderr, "%f  ", para[i].v.d);
	  break;
	case PARA_CHAR_POINTER:
	  temp = malloc (strlen (para[i].v.pc) * 2);
	  utf8_to_local (para[i].v.pc, temp, strlen (para[i].v.pc) * 2);
	  fprintf (stderr, "%s  ", temp);
	  free (temp);
	  break;
	}
    }
  fprintf (stderr, "\n");
}				/* _trace_signame() */


/**
 * \brief print signal destination (for debug)
 * \param <level> level of the current signal
 * \param <dest> name of the destination
 *
 * For debugging purposes. The function prints the destination the signal is sent to.
 */
void
_trace_sigdest (int level, const char *dest)
{
  int i;
  char *dest_locale;

  if (!ra_print_debug_infos)
    return;

  for (i = 0; i < (level - 1); i++)
    fprintf (stderr, "  ");

  dest_locale = malloc (strlen (dest) * 2);
  utf8_to_local (dest, dest_locale, strlen (dest) * 2);
  fprintf (stderr, "-> %s\n", dest_locale);
  free (dest_locale);
}				/* _trace_sigdest() */
