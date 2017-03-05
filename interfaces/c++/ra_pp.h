/**
 * \file ra_pp.h
 *
 * The header file contains the class declerations for the C++ interface of libRASCH
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2004-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/


#ifndef RA_PP_H
#define RA_PP_H


#include <ra.h>

#include <vector>
#include <string>

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#ifndef _LIBRASCHPP_BUILD
#define LIBRAPPAPI __declspec(dllimport)
#else
#define LIBRAPPAPI __declspec(dllexport)
#endif
#else
#define LIBRAPPAPI
#endif


//
// Unfortunately no top-down order of the class definitions (which I prefer)
// because I have some problems with forward declarations. I do not know why,
// but GNU C++ 3.3.3 coming with SuSE 9.1 (I have not tried other versions) has
// problems with it (or I have problems understanding the concept and/or
// usage of forward declarations). If someone knows what the reason is, please
// let me know. Thanks.
//

using namespace std;

// ----------------------------------------------------------------------
/**
 * The class handles the values used in libRASCH.
 */
class rasch_value
{
 public:
	/**
	 * The default constructor.
	 */
	LIBRAPPAPI rasch_value();
	/**
	 * The copy constructor.
	 * \param v Reference to a rasch_value instance.
	 */
	LIBRAPPAPI rasch_value(const rasch_value &v);
	/**
	 * Constructor initializing class with a value-handle from libRASCH.
	 * \param vh value-handle from libRASCH.
	 */
	LIBRAPPAPI rasch_value(value_handle vh);
	/**
	 * Constructor initializing class with a short value.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(short value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a long value.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(long value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a double value.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(double value, const char *name=NULL, const
			       char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a string.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const string &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a char-pointer.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const char *value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a void pointer.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(void *value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a short vector.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const vector<short> &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a short array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const short *value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a long vector.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const vector<long> &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a long array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const long *value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a double vector.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const vector<double> &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a double array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const double *value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a string vector.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const vector<string> &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a char-pointer array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const char ** value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a void pointer vector.
	 * \param value value which will be used to initialize the class.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const vector<void *> &value, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a void pointer array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const void **value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);
	/**
	 * Constructor initializing class with a value_handle array.
	 * \param value value which will be used to initialize the class.
	 * \param n_elem number of elements in the value-array.
	 * \param name name of value (optional).
	 * \param desc description of the value (optional).
	 * \param id ID of the value if a libRASCH info is handled by the class (optional).
	 */
	LIBRAPPAPI rasch_value(const value_handle *value, long n_elem, const char *name=NULL,
			       const char *desc=NULL, int id=RA_INFO_NONE);

	/**
	 * Destructor.
	 */
	LIBRAPPAPI ~rasch_value();

	/**
	 * The copy operator.
	 * \param val Reference to the rasch_value instance which will be copied.
	 */
	LIBRAPPAPI rasch_value &operator=(const rasch_value &val);

	LIBRAPPAPI bool is_valid() { return (_vh != NULL ? true : false); }

	/**
	 * Initialize class with the values/informations handled by a libRASCH value_handle.
	 * \param vh The value-handle.
	 */
	LIBRAPPAPI void set_by_value_handle(value_handle vh);
	/**
	 * Set a value-handle with the values/informations set in the class.
	 * \param vh The value-handle.
	 */
	LIBRAPPAPI void fill_value_handle(value_handle vh);

	/**
	 * Return the type of the value (see RA_VALUE_TYPE_* in ra_defines.h).
	 */
	LIBRAPPAPI long get_type() { return ra_value_get_type(_vh); }
	/**
	 * Returns true if the value handled by the class is valid.
	 */
	LIBRAPPAPI bool is_ok() { return (ra_value_is_ok(_vh) ? true : false); }
	/**
	 * Returns the id of the value (if any) (see RA_INFO_* in ra_defines.h).
	 */
	LIBRAPPAPI long get_info() { return ra_value_get_info(_vh); }
	/**
	 * Returns the name (if any) of the value.
	 */
	LIBRAPPAPI string get_name() { return ra_value_get_name(_vh); }
	/**
	 * Returns the description (if any) of the value.
	 */
	LIBRAPPAPI string get_desc() { return ra_value_get_desc(_vh); }

	/**
	 * Returns if the info currently handled by rasch_value can be modified
	 * in the measurement file(s).
	 */
	LIBRAPPAPI bool is_info_modifiable();
	/**
	 * Set if the info currently handled by rasch_value can be modified
	 * in the measurement file(s).
	 */
	LIBRAPPAPI void set_info_modifiable(bool flag);
	
	/**
	 * Returns the number of values handled by the class (if the value is a vector or an array).
	 */
	LIBRAPPAPI long get_num_elem();

	/**
	 * Set the number-parameter which will be used in some libRASCH functions
	 * (e.g. set the result number when calling the ra_proc_get_results()).
	 * \param n The number.
	 */
	LIBRAPPAPI void set_number(long n) { ra_value_set_number(_vh, n); }
	/**
	 * Returns the number-parameter set in the class.
	 */
	LIBRAPPAPI long get_number() { return ra_value_get_number(_vh); }

	/**
	 * Set the value handled by the rasch-value.
	 * \param value Short value which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(short value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Long value which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(long value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Double value which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(double value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value String which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const string &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value String (char pointer) which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const char *value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value UTF-8 encoded string (char pointer) which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value_utf8(const char *value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value void pointer value which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(void *value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Short vector which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const vector<short> &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Short array which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const short *value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Long vector which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const vector<long> &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Long array which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const long *value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Double vector which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const vector<double> &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Double array which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const double *value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value String vector which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const vector<string> &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Array of strings (char pointers)  which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const char **value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value Array of UTF-8 encoded strings (char pointers) which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value_utf8(const char **value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value void pointer vector which will be rasch-value set to.
	 */
	LIBRAPPAPI void set_value(const vector<void *> &value);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value void pointer array which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const void **value, long n_elem);
	/**
	 * Set the value handled by the rasch-value.
	 * \param value value_handle array which will be rasch-value set to.
	 * \param n_elem Number of elements of the array.
	 */
	LIBRAPPAPI void set_value(const value_handle *value, long n_elem);

	/**
	 * Set the information about the current value.
	 * \param name The name of the value.
	 * \param desc The description of the value.
	 * \param id The id of the value (if any) (see RA_INFO_* in ra_defines.h).
	 * \param can_be_changed If the value is a RA_INFO_* "value", this flags shows if the value can be changed 
         *                       and written back to the measurement file(s).
	 */
	LIBRAPPAPI void set_meta_info(const char *name, const char *desc, int id, bool can_be_changed);

	/**
	 * Return the value (if it is a short value).
	 */
	LIBRAPPAPI long get_value_s() { return ra_value_get_short(_vh); }
	/**
	 * Return the value (if it is a long value).
	 */
	LIBRAPPAPI long get_value_l() { return ra_value_get_long(_vh); }
	/**
	 * Return the value (if it is a double value).
	 */
	LIBRAPPAPI double get_value_d() { return ra_value_get_double(_vh); }
	/**
	 * Return the value (if it is a string).
	 */
	LIBRAPPAPI string get_value_c();
	/**
	 * Return the value UTF-8 encoded (if it is a string).
	 */
	LIBRAPPAPI string get_value_c_utf8();
	/**
	 * Return the value (if it is a void pointer).
	 */
	LIBRAPPAPI const void *get_value_v() { return ra_value_get_voidp(_vh); }
	/**
	 * Return the values (if it is a short vector).
	 */
	LIBRAPPAPI vector<short> get_value_sv();
	/**
	 * Return the values (if it is a long vector).
	 */
	LIBRAPPAPI vector<long> get_value_lv();
	/**
	 * Return the values (if it is a double vector).
	 */
	LIBRAPPAPI vector<double> get_value_dv();
	/**
	 * Return the values (if it is a string vector).
	 */
	LIBRAPPAPI vector<string> get_value_cv();
	/**
	 * Return the values UTF-8 encoded (if it is a string vector).
	 */
	LIBRAPPAPI vector<string> get_value_cv_utf8();
	/**
	 * Return the values (if it is a void * vector).
	 */
	LIBRAPPAPI vector<void *> get_value_vv();

	/**
	 * Reset the value-handle.
	 */
	LIBRAPPAPI void reset();

 private:
	/**
	 * The libRASCH value-handle.
	 */
	value_handle _vh;
};  // class rasch_value


/**
 * The class handles the measurements found with the rasch.find_meas() method.
 * The 
 */
class rasch_find
{
 public:
	/**
	 * Default constructor.
	 */
	LIBRAPPAPI rasch_find();
	/**
	 * Constructor setting the file name.
	 */
	LIBRAPPAPI rasch_find(const char *fn, const char *pl) : _fn(fn), _pl(pl) { }
	/**
	 * Destructor; nothing has to be done.
	 */
	LIBRAPPAPI ~rasch_find() { }

	/**
	 * Set the file name.
	 * \param fn The file name set in the instance of rasch_find.
	 */
	LIBRAPPAPI void set_fn(const char *fn) { _fn = fn; }
	/**
	 * Set the plugin name.
	 * \param pl The plugin name used to handle the measurement.
	 */
	LIBRAPPAPI void set_pl(const char *pl) { _pl = pl; }
	/**
	 * Returns the name of the measurement file which allows to open then measurement with rasch.open_meas().
	 */
	LIBRAPPAPI const string &get_fn() { return _fn; }
	/**
	 * Returns the name of the plugin which handles the measurement file(s).
	 */
	LIBRAPPAPI const string &get_pl() { return _pl; }

 private:
	/**
	 * Name of the measurement file which allows to open then measurement with rasch.open_meas().
	 */
	string _fn;
	/**
	 * Name of the plugin handling the measurement file(s).
	 */
	string _pl;
}; // class rasch_find


/**
 * The class handles a single evaluation related attribute.
 * The 
 */
class rasch_attribute
{
 public:
	/**
	 * Default constructor.
	 */
	LIBRAPPAPI rasch_attribute();
	/**
	 * Constructor setting the file name.
	 */
	LIBRAPPAPI rasch_attribute(any_handle h, const char *name, rasch_value &val);
	/**
	 * Destructor; nothing has to be done.
	 */
	LIBRAPPAPI ~rasch_attribute() { }

	LIBRAPPAPI bool is_valid() { return (_h != NULL ? true : false); }

	/**
	 * Init attribute value.
	 * \param h handle the attribute belongs to
	 * \param name name of the attribute
	 */
	LIBRAPPAPI int init(any_handle h, const char *name);
	/**
	 * Set the attribute value.
	 * \param val value to be set
	 */
	LIBRAPPAPI int set_value(rasch_value &val);
	/**
	 * Get the attribute value.
	 */
	LIBRAPPAPI rasch_value get_value() { return _value; }
	/**
	 * Returns the name of the attribute.
	 */
	LIBRAPPAPI const string &get_name() { return _name; }

	/**
	 * Returns the name of the attribute.
	 */
	LIBRAPPAPI int unset();

 private:
	/**
	 * Handle the attribute belongs to.
	 */
	any_handle _h;	
	/**
	 * Name of the attribute.
	 */
	string _name;
	/**
	 * Value of the attribute.
	 */
	rasch_value _value;
}; // class rasch_attribute


/**
 * The class handles one recording channel.
 */
class rasch_channel
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_channel(const rasch_channel &);
	/**
	 * Constructor initializing the rec-handle and the channel number. This constructor will be
	 * called from rasch_rec. It is not intended to create an instance of the class outside the
	 * rasch_rec implementation.
	 * \param rec The rec-handle which the channel belongs to.
	 * \param channel The channel handled by the instance of rasch_channel.
	 */
	LIBRAPPAPI rasch_channel(rec_handle rec, int channel);
	/**
	 * Destructor, nothing has to be done.
	 */
	LIBRAPPAPI ~rasch_channel();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_channel &operator=(const rasch_channel &);

	LIBRAPPAPI bool is_valid() { return ((_rec != NULL) && (_ch != -1) ? true : false); }

	/**
	 * Returns a channel specific information selected by an ID (see RA_INFO_REC_CH_* in ra_defines.h).
	 * \param id The RA_INFO_REC_CH_* id.
	 */
	LIBRAPPAPI rasch_value get_info(int id);
	/**
	 * Returns a channel specific information selected by the name (see pl_general.h).
	 * \param name The name of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info(const char *name);
	/**
	 * Returns a channel specific information selected by an index. This is usefull
	 * to show (eg. in a list) all supported informations without prior knowledge what
	 * information is available for the current channel.
	 * \param index The zero-based index of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info_by_idx(int index);
	
	/**
	 * Returns the unscaled channel raw data (the samples).
	 * TODO: handle higher 4 bytes
	 * \param start The position (in sample-units) of the first sample-value wanted.
	 * \param num_data The number of samples wanted.
	 */
	LIBRAPPAPI vector<unsigned long> get_raw(size_t start, size_t num_data);
	/**
	 * Returns the channel raw data (the samples) scaled to 'unit-values'.
	 * \param start The position (in sample-units) of the first sample-value wanted.
	 * \param num_data The number of samples wanted.
	 */
	LIBRAPPAPI vector<double> get_raw_unit(size_t start, size_t num_data);

 private:
	/**
	 * The rec-handle which the channel belongs to.
	 */
	rec_handle _rec;
	/**
	 * The channel handled by the instance of rasch_channel.
	 */
	int _ch;
};  // class rasch_channel


/**
 * The class handles one recording device.
 */
class rasch_device
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_device(const rasch_device &);
	/**
	 * Constructor initializing the rec-handle and the device number. This constructor will be
	 * called from rasch_rec. It is not intended to create an instance of the class outside the
	 * rasch_rec implementation.
	 * \param rec The rec-handle which the device belongs to.
	 * \param dev_number The device handled by the instance of rasch_device.
	 */
	LIBRAPPAPI rasch_device(rec_handle rec, int dev_number);
	/**
	 * Destructor, nothing has to be done.
	 */
	LIBRAPPAPI ~rasch_device();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_device &operator=(const rasch_device &);

	LIBRAPPAPI bool is_valid() { return ((_rec != NULL) && (_device != -1) ? true : false); }

	/**
	 * Returns a device specific information selected by an ID (see RA_INFO_REC_DEV_* in ra_defines.h).
	 * \param id The RA_INFO_REC_DEV_* id.
	 */
	LIBRAPPAPI rasch_value get_info(int id);
	/**
	 * Returns a device specific information selected by the name (see pl_general.h).
	 * \param name The name of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info(const char *name);
	/**
	 * Returns a device specific information selected by an index. This is usefull
	 * to show (eg. in a list) all supported informations without prior knowledge what
	 * information is available for the current device.
	 * \param index The zero-based index of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

 private:
	/**
	 * The rec-handle which the device belongs to.
	 */
	rec_handle _rec;
	/**
	 * The device handled by the instance of rasch_device.
	 */
	int _device;
};  // class rasch_device


class rasch_rec
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_rec(const rasch_rec &);
	/**
	 * Constructor initializing the meas-handle and the rec-handle. This constructor will be
	 * called from rasch_meas. It is not intended to create an instance of the class outside the
	 * rasch_meas implementation.
	 * \param meas The meas-handle which the recording belongs to.
	 * \param session The session handled by the instance of rasch_rec.
	 */
	LIBRAPPAPI rasch_rec(meas_handle meas, long session);
	/**
	 * Constructor initialzing the rec-handle and the meas-handle. This constructor will be
	 * called from rasch_meas. It is not intended to create an instance of the class outside the
	 * rasch_meas implementation.
	 * \param rec The rec-handle for the instance of rasch_rec.
	 */
	LIBRAPPAPI rasch_rec(rec_handle rec);
	/**
	 * Destructor, nothing has to be done.
	 */
	LIBRAPPAPI ~rasch_rec();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_rec &operator=(const rasch_rec &);

	/**
	 * Returns true if it is a valid recording.
	 */
	LIBRAPPAPI bool is_valid() { return (_rec == NULL ? false : true); }
	/**
	 * Returns the next (sibling) recording. If no further recording is available,
	 * the function rasch_rec.is_valid() will return false.
	 */
	LIBRAPPAPI rasch_rec get_next();
	/**
	 * Returns the first child recording. If no child recording is available,
	 * the function rasch_rec.is_valid() will return false.
	 */
	LIBRAPPAPI rasch_rec get_first_child();

	/**
	 * Returns a general recording information selected by an ID (see RA_INFO_REC_GEN_* in ra_defines.h).
	 * \param id The RA_INFO_REC_GEN_* id.
	 */
	LIBRAPPAPI rasch_value get_info(int id);
	/**
	 * Returns a general recording information selected by the name (see pl_general.h).
	 * \param name The name of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info(const char *name);
	/**
	 * Returns a general recording information selected by an index. This is usefull
	 * to show (eg. in a list) all supported informations without prior knowledge what
	 * information is available for the current device.
	 * \param index The zero-based index of the wanted information.
	 */
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

	/**
	 * Sets the general recording information selected by 'id' (see RA_INFO_REC_GEN_* in ra_defines.h).
	 * \param id The RA_INFO_REC_GEN_* id to be set.
	 * \param value A rasch-value with the value which should be set.
	 */
	LIBRAPPAPI int set_info(int id, rasch_value &value);
	/**
	 * Sets the general recording information selected by 'name' (see pl_general.h).
	 * \param name The name of the information to be set.
	 * \param value A rasch-value with the value which should be set.
	 */
	LIBRAPPAPI int set_info(const char *name, rasch_value &value);

	/**
	 * Returns a rasch-channel object handling the channel specific tasks.
	 * \param ch The channel which one is interested in.
	 */
	LIBRAPPAPI rasch_channel get_channel(int ch)
		{ return rasch_channel(_rec, ch); }

	/**
	 * Returns a rasch-device object handling the device specific tasks.
	 * \param device_number The device which one is interested in.
	 */
	LIBRAPPAPI rasch_device get_device(int device_number)
		{ return rasch_device(_rec, device_number); }

 private:
	/**
	 * The meas-handle the recording belongs to.
	 */
	meas_handle _meas;
	/**
	 * The rec-handle handled by the instance of rasch_rec
	 */
	rec_handle _rec;
};  // class rasch_rec



class rasch_summary
{
 public:
	LIBRAPPAPI rasch_summary();
	LIBRAPPAPI rasch_summary(class_handle clh, sum_handle sh);
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
/* 	rasch_summary(const rasch_summary &); */
	LIBRAPPAPI ~rasch_summary();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
/* 	rasch_summary &operator=(const rasch_summary &); */

	LIBRAPPAPI bool is_valid() { return (_summary != NULL ? true : false); }

	LIBRAPPAPI void init(class_handle clh, sum_handle sh) { _class = clh; _summary = sh; }

	LIBRAPPAPI vector<rasch_attribute> get_attributes();

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

	LIBRAPPAPI int del();

	LIBRAPPAPI int add_ch(long ch, long fiducial_offset=0);

	LIBRAPPAPI long add_part(vector<long> event_ids);
	LIBRAPPAPI int del_part(long part_id);
	LIBRAPPAPI vector<long> get_part_all();

	LIBRAPPAPI int set_part_data(long part_id, long ch_idx, long dim, rasch_value value);
	LIBRAPPAPI int get_part_data(long part_id, long ch_idx, long dim, rasch_value &value);

	LIBRAPPAPI vector<long> get_part_events(long part_id);
	LIBRAPPAPI int set_part_events(long part_id, vector<long> events);
	
 private:
	class_handle _class;
	sum_handle _summary;
};  // class rasch_summary


class rasch_prop
{
 public:
	LIBRAPPAPI rasch_prop();
	LIBRAPPAPI rasch_prop(eval_handle eval, prop_handle prop);
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
/* 	rasch_prop(const rasch_prop &); */

	LIBRAPPAPI ~rasch_prop();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
/* 	rasch_prop &operator=(const rasch_prop &); */

	LIBRAPPAPI bool is_valid() { return (_prop != NULL ? true : false); }

	LIBRAPPAPI void init(eval_handle eval, prop_handle prop) { _eval = eval; _prop = prop; }

	LIBRAPPAPI vector<rasch_attribute> get_attributes();

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

	LIBRAPPAPI int del();

	LIBRAPPAPI int set_value(long ev_id, long ch, rasch_value value);
	LIBRAPPAPI int set_value_mass(const long *ev_ids, const long *chs, rasch_value value);

	LIBRAPPAPI vector<long> get_ch(long ev_id);

	LIBRAPPAPI rasch_value get_value(long ev_id, long ch);
	LIBRAPPAPI int get_value_all(rasch_value &ev_ids, rasch_value &chs, rasch_value &values);

	LIBRAPPAPI vector<long> get_events(rasch_value min, rasch_value max, long ch);

 private:
	eval_handle _eval;
	prop_handle _prop;
};  // class rasch_prop


class rasch_class
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
/* 	rasch_class(const rasch_class &); */
	LIBRAPPAPI rasch_class();
	LIBRAPPAPI rasch_class(eval_handle eval, class_handle cl);
	LIBRAPPAPI ~rasch_class();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
/* 	rasch_class &operator=(const rasch_class &); */

	LIBRAPPAPI bool is_valid() { return (_class != NULL ? true : false); }

	LIBRAPPAPI void init(eval_handle eh, class_handle clh) { _eval = eh; _class = clh; }
	
	LIBRAPPAPI vector<rasch_attribute> get_attributes();

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

	LIBRAPPAPI long add_event(long start, long end);
	LIBRAPPAPI int del_event(long ev_id);

	LIBRAPPAPI int set_event_pos(long ev_id, long start, long end);
	LIBRAPPAPI int get_event_pos(long ev_id, long &start, long &end);

	LIBRAPPAPI vector<long> get_events(long start=0, long end=-1, bool complete=false, bool sort=true);

	LIBRAPPAPI long get_prev_event(long ev_id);
	LIBRAPPAPI long get_next_event(long ev_id);

	LIBRAPPAPI rasch_prop add_prop(const char *id, long val_type, const char *name,
			    const char *desc, const char *unit, bool use_minmax,
			    double min, double max, bool has_ignore_value, double ignore_value);

	LIBRAPPAPI rasch_prop add_prop_predef(const char *id);

	LIBRAPPAPI rasch_prop get_prop(const char *id);
	LIBRAPPAPI vector<rasch_prop> get_prop_all();

	LIBRAPPAPI rasch_summary add_sum(const char *id, const char *name, const char *desc,
			      long num_dim, const char **dim_name, const char **dim_unit);

	LIBRAPPAPI vector<rasch_summary> get_sum(const char *id);

 private:
	eval_handle _eval;
	class_handle _class;
};  // class rasch_class


class rasch_eval
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
/* 	LIBRAPPAPI rasch_eval(const rasch_eval &); */
	LIBRAPPAPI rasch_eval();
	LIBRAPPAPI rasch_eval(meas_handle meas, eval_handle eh);
	LIBRAPPAPI ~rasch_eval();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
/* 	LIBRAPPAPI rasch_eval &operator=(const rasch_eval &); */

	LIBRAPPAPI bool is_valid() { return (_eval != NULL ? true : false); }

	LIBRAPPAPI void init(meas_handle meas, eval_handle eh) { _meas = meas; _eval = eh; }

	LIBRAPPAPI vector<rasch_attribute> get_attributes();

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int index);

	LIBRAPPAPI int del();
	LIBRAPPAPI int set_default();

	LIBRAPPAPI rasch_class add_class(const char *id, const char *name, const char *desc);
	LIBRAPPAPI rasch_class add_class_predef(const char *id);

	LIBRAPPAPI vector<rasch_class> get_class(const char *id);

	LIBRAPPAPI int save(const char *fn=NULL, bool use_ascii=false);

 private:
	meas_handle _meas;
	eval_handle _eval;
};  // class rasch_eval


class rasch_meas
{
	friend class rasch_plugin;

 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_meas(const rasch_meas &);
	LIBRAPPAPI rasch_meas(ra_handle ra, const char *fn, const char *eval_fn=NULL, bool fast=false, bool new_meas=false);
	LIBRAPPAPI ~rasch_meas();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_meas &operator=(const rasch_meas &);

	LIBRAPPAPI bool is_valid() { return (_meas != NULL ? true : false); }

	LIBRAPPAPI void close() { if (_meas) ra_meas_close(_meas); _meas = NULL; }

	LIBRAPPAPI rasch_rec get_first_rec(long session=0)
		{ return rasch_rec(_meas, session); }

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int info_type, int index);

	LIBRAPPAPI int set_info(int id, rasch_value &value);
	LIBRAPPAPI int set_info(const char *name, rasch_value &value);

	LIBRAPPAPI int move(const char *dest_dir);
	LIBRAPPAPI int copy(const char *dest_dir);
	LIBRAPPAPI int del();

	LIBRAPPAPI int save();

	LIBRAPPAPI int load_eval(const char *fn=NULL);

	LIBRAPPAPI rasch_eval add_eval(const char *name, const char *desc, bool original);
	LIBRAPPAPI rasch_eval get_default_eval();
	LIBRAPPAPI rasch_eval get_original_eval();
	LIBRAPPAPI vector<rasch_eval> get_eval_all();

 private:
	meas_handle get_handle() { return _meas; }
    
	ra_handle _ra;
	meas_handle _meas;
};  // class rasch_meas


class rasch_proc
{
 public:
	LIBRAPPAPI rasch_proc(plugin_handle pl, meas_handle mh, void (*callback) (const char *, int ));
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_proc(const rasch_proc &);
	LIBRAPPAPI ~rasch_proc();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_proc &operator=(const rasch_proc &);

	LIBRAPPAPI bool is_valid() { return (_proc != NULL ? true : false); }

	LIBRAPPAPI long get_result_idx(const char *res_ascii_id);

	LIBRAPPAPI int set_option(const char *name, rasch_value &value);
	LIBRAPPAPI rasch_value get_option(const char *name);

	LIBRAPPAPI vector<rasch_value> do_processing(long &num_sets, long &num_res);

 private:
	plugin_handle _pl;
	meas_handle _meas;
	proc_handle _proc;
};  // class rasch_proc


class rasch_view
{
 public:
	LIBRAPPAPI rasch_view(plugin_handle pl, meas_handle mh, void *parent);
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_view(const rasch_view &);
	LIBRAPPAPI ~rasch_view();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_view &operator=(const rasch_view &);

	LIBRAPPAPI bool is_valid() { return (_view != NULL ? true : false); }

	LIBRAPPAPI void *get_window() { return ra_view_get_window(_view, 0); }

 private:
	plugin_handle _pl;
	meas_handle _meas;
	view_handle _view;
};  // class rasch_view


class rasch_dialog
{
 public:
	LIBRAPPAPI rasch_dialog(plugin_handle pl, meas_handle mh);
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_dialog(const rasch_dialog &);
	LIBRAPPAPI ~rasch_dialog();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_dialog &operator=(const rasch_dialog &);

	LIBRAPPAPI bool is_valid() { return (_dlg != NULL ? true : false); }

	LIBRAPPAPI long get_result_idx(const char *res_ascii_id);

	LIBRAPPAPI int set_option(const char *name, rasch_value &value);
	LIBRAPPAPI rasch_value get_option(const char *name);

	LIBRAPPAPI int exec(vector<rasch_value> &results);

 private:
	plugin_handle _pl;
	meas_handle _meas;
	proc_handle _dlg;
};  // class rasch_dialog


class rasch_plugin
{
 public:
	LIBRAPPAPI rasch_plugin(ra_handle ra, plugin_handle pl);
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch_plugin(const rasch_plugin &);
	LIBRAPPAPI ~rasch_plugin();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch_plugin &operator=(const rasch_plugin &);

	LIBRAPPAPI bool is_valid() { return (_pl != NULL ? true : false); }

	LIBRAPPAPI rasch_value get_info(int id, long num=0);
	LIBRAPPAPI rasch_value get_info(const char *name, long num=0);
	LIBRAPPAPI rasch_value get_info_by_idx(int index, long num=0);

	LIBRAPPAPI int use_it();
	LIBRAPPAPI int dont_use_it();

	LIBRAPPAPI int get_result_info(long res_num);

	LIBRAPPAPI rasch_proc get_proc(rasch_meas &meas, void (*callback) (const char *, int ) = NULL)
		{ return rasch_proc(_pl, meas.get_handle(), callback); }

	LIBRAPPAPI rasch_view get_view(rasch_meas &meas, void *parent) { return rasch_view(_pl, meas.get_handle(), parent); }

	LIBRAPPAPI rasch_dialog get_dialog(rasch_meas &meas) { return rasch_dialog(_pl, meas.get_handle()); }
	LIBRAPPAPI rasch_dialog get_dialog() { return rasch_dialog(_pl, NULL); }

 private:
	ra_handle _ra;
	plugin_handle _pl;
};  // class rasch_plugin



class rasch
{
 public:
	/**
	 * Dummy copy constructor with no implementation -> not allow the usage of the copy constructor.
	 */
	LIBRAPPAPI rasch(const rasch &);
	LIBRAPPAPI rasch();
	LIBRAPPAPI ~rasch();

	/**
	 * A dummy copy operator with no implementation -> not allow copying.
	 */
	LIBRAPPAPI rasch &operator=(const rasch &);

	LIBRAPPAPI bool is_valid() { return (_ra != NULL ? true : false); }

	LIBRAPPAPI long get_error(string &err_string);

	LIBRAPPAPI int use_plugin(int plugin_index, bool use_it);

	LIBRAPPAPI rasch_plugin get_plugin(int index, bool search_all=false);
	LIBRAPPAPI rasch_plugin get_plugin(const char *name, bool search_all=false);

	LIBRAPPAPI vector<rasch_find> find_meas(const char *dir);

	LIBRAPPAPI rasch_meas open_meas(const char *fn, const char *eval_fn=NULL, bool fast=false)
		{ return rasch_meas(_ra, fn, eval_fn, fast); }
	LIBRAPPAPI rasch_meas new_meas(const char *fn)
		{ return rasch_meas(_ra, fn, NULL, false, true); }

	LIBRAPPAPI rasch_value get_info(int id);
	LIBRAPPAPI rasch_value get_info(const char *name);
	LIBRAPPAPI rasch_value get_info_by_idx(int info_type, int index);

//	LIBRAPPAPI int set_info(int id, rasch_value &value);

 private:
	ra_handle _ra;
};  // class rasch


#endif  // RA_PP_H
