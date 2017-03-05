#ifndef RA_AP_MORPHOLOGY_H
#define RA_AP_MORPHOLOGY_H

#define AP_VALUE_NOK           0x00000001
#define AP_VALUE_INTERPOLATED  0x00000002


/* types of calibration sequences */
#define AP_CALIB_PHYSIOCAL   1	/* physiocal calibration in PortaPres systems */


struct ra_ap_morphology
{
	eval_handle eh;	/* evaluation used; if '= NULL' use default evaluation */
	
	short use_class;	     /* use 'heartbeat' event-class stored in option 'clh' */
	class_handle clh;    /* handle to the event-class holding the events which should be processed */
	class_handle clh_calib; /* handle to the event-class holding the calibration sequences */
	short save_in_class;  /* flag if the results should be saved in the event-class */

	long num_ch;		/* number of channels to process */
	long *ch;		/* channel(s) of the aterial pressure signal; if none was given,
				   all arterial pressure channels will be processed */
	
	long num_pos;        /* number of positions where rr values and positions should be get */
	long *pos;

	/* flag if positions are start and end of a region;
	   if set, num_pos must be 2 */
	short pos_are_region;
	/* flag if positions are event id's in clh */
	short pos_are_event_id;
	/* positions are bloodpressure peaks (= 1) or qrs-complexes (= 0);
	   don't be used if postions are start-/end-region */
	short rr_pos;	

	/* if set (=1) -> don't look for sequences of calibration
	   in bloodpressure measurements */
	short dont_search_calibration;

	/* if set (=1) -> do not search the positions of systolic- and
	   diastolic blood pressure but use the previously selected positions
	   (needed to re-calc the ap related measures) */
	short dont_search_positions;
}; /* struct ra_ap_morphology */


#endif /* RA_AP_MORPHOLOGY_H */
