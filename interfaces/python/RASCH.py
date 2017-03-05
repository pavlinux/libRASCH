#
# libRASCH Python Interface
#
# Author: Raphael Schneider (librasch@gmail.com)
#
# Copyright (C) 2002-2007, Raphael Schneider
# See the file COPYING for information on usage and redistribution.
# 
# $Id$
#

import RASCH_if


########## RAvalue namespace
class RAvalue:
    def __init__(self, val=None, na=None, de=None, modif=None, inf=-1, num=None):
        self.value_var = val
        self.name_var = na
        self.desc_var = de
        self.info_id_var = inf
        self.number_var = num
        self.modifiable_var = modif
        
    def is_ok(self):
        if self.value_var is None:
            return False
        return True
    
    def info(self, val=None):
        if val is not None:
            self.info_id_var = val
        return self.info_id_var
    
    def name(self, val=None):
        if val is not None:
            self.name_var = val
        return self.name_var
    
    def desc(self, val=None):
        if val is not None:
            self.desc_var = val
        return self.desc_var
    
    def number(self, val=None):
        if val is not None:
            self.number_var = val
        return self.number_var

    def modifiable(self, val=None):
        if val is not None:
            self.modifiable_var = val
        return self.modifiable_var
    
    def value(self, val=None):
        if val is not None:
            self.value_var = val
        return self.value_var
    
    def value_utf8(self, val=None):
        if val is not None:
            self.value_utf8_var = val
        return self.value_utf8_var
    
    def reset(self):
        self.value_var = None
        self.value_utf8_var = None
        self.name_var = None
        self.desc_var = None
        self.info_id_var = -1
        self.number_var = None

##### end of RAvalue namspace


########## RAfind namespace
class RAfind:
    def __init__(self):
        self.fn_var = None
        self.pl_var = None

    def __cmp__(self, other):
	return cmp(self.fn_var, other.fn_var)

    def __str__(self):
	return str(self.fn_var)

    def filename(self, fn=None):
        if fn is not None:
            self.fn_var = fn
        return self.fn_var

    def plugin(self, pl=None):
        if pl is not None:
            self.pl_var = pl
        return self.pl_var

##### end of RAfind namespace


########## RAattribute namespace
class RAattribute:
    def __init__(self, h=None, n=None, v=None):
        self.handle_var = h
        self.name_var = n
        self.value_var = v

    def value(self, val_type=None, val=None):
        if val is not None:
            RASCH_if._if_eval_attribute_set(self.handle, self.name, val_type, val)
            self.value_var = val
        return self.value_var

    def name(self):
        return self.name_var

    def unset(self):
        RASCH_if._if_eval_attribute_unset(self.handle, self.name)

##### end of RAattribute namespace


def get_all_attributes(h):
    if h is None:
        return None
    
    attrib_list = RASCH_if._if_eval_attribute_list(h)

    attribs = []
    for a in attrib_list:
        val = RASCH_if._if_eval_attribute_get(h, a)
        curr = RAattribute(h, a, val)
        attribs.append(curr)

    return attribs



########## RASCH namespace
class RASCH:
    def __init__(self):
        self.h = RASCH_if._if_lib_init()

    def close(self):
        if (self.h):
            RASCH_if._if_lib_close(self.h)
        self.h = None
        
    def get_handle(self):
        return self.h

    def get_error(self):
        return RASCH_if._if_lib_get_error(self.h)

    def use_plugin(self, pl_idx, use_it):
        return RASCH_if._if_lib_use_plugin(self.h, pl_idx, use_it)
    
    def get_plugin_by_name(self, name, search_all=0):
        plh = RASCH_if._if_plugin_get_by_name(self.h, name, search_all)
        if not plh:
            return None
        return RAplugin(plh)

    def get_plugin_by_num(self, num, search_all=0):
        plh = RASCH_if._if_plugin_get_by_num(self.h, num, search_all)
        if not plh:
            return None
        return RAplugin(plh)

    def find_meas(self, path):
        ret = RASCH_if._if_meas_find(self.h, path)
        fn_list = ret[0]
        pl_list = ret[1]
        all = []
        cnt = 0
        for fn in fn_list:
            curr = RAfind()
            curr.filename(fn)
            curr.plugin(pl_list[cnt])
            all.append(curr)
            cnt = cnt + 1
	all.sort()
        return all

    def open_meas(self, fn, fn_eval='', open_fast=0):
        mh = RASCH_if._if_meas_open(self.h, fn, fn_eval, open_fast)
        if not mh:
            return None
        return  RAmeas(mh)

    def new_meas(self, dir, fn):
        mh = RASCH_if._if_meas_new(self.h, dir, fn)
        if not mh:
            return None
        return  RAmeas(mh)

    def get_info(self, **var):
        if (not var.has_key('info')):
            return None
        ret = RASCH_if._if_lib_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

##### end of RASCH namespace



########## RAmeas namespace
class RAmeas:
    def __init__(self, meas_handle):
        self.h = meas_handle
        
    def __del__(self):
        if self.h is not None:
            RASCH_if._if_meas_close(self.h)
        self.h = None

    def close(self):
	ret = 0
	if self.h is not None:
            ret = RASCH_if._if_meas_close(self.h)
            self.h = None
        return ret
        
    def get_handle(self):
        return self.h
    
    def get_info(self, **var):
        if (not var.has_key('info')):
            return None
        ret = RASCH_if._if_meas_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def get_info_idx(self, **var):
        if (not var.has_key('index')):
            return None
        ret = RASCH_if._if_info_get_by_idx(self.h, "meas", var['index'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def set_info(self, info, type, val):
        ret = RASCH_if._if_info_set(self.h, info, type, val)
        return ret

    def move(self, dir):
        self.meas = RASCH_if._if_meas_move(self.h, dir)

    def copy(self, dir):
        return RASCH_if._if_meas_copy(self.h, dir)

    def delete(self):
        RASCH_if._if_meas_delete(self.h)
        self.h = 0
        
    def save(self):
        RASCH_if._if_meas_save(self.h)

    def load_eval(self, fn=0):
        return None # TODO: not yet implemented
    
    def get_ra_handle(self):
        return RASCH_if._if_ra_lib_handle_from_any_handle(self.h)

    def get_first_rec(self, session_num=0):
        rec = RASCH_if._if_rec_get_first(self.h, session_num)
        if not rec:
            return None
        return RArec(rec)

    def get_eval(self, eval_num=0):
        eva = RASCH_if._if_eval_by_num(self.h, eval_num)
        if not eva:
            return None
        return RAeval(eva)

    def get_default_eval(self):
        eva = RASCH_if._if_eval_get_default(self.h)
        if not eva:
            return None
        return RAeval(eva)

    def get_original_eval(self):
        eva = RASCH_if._if_eval_get_original(self.h)
        if not eva:
            return None
        return RAeval(eva)

    def get_eval_all(self):
        all = RASCH_if._if_eval_get_all(self.h)
        if not all:
            return None
        evals = []
        for eva in all:
            curr = RAeval(eva)
            evals.append(curr)
        return evals

    def add_eval(self, name=None, desc=None, original=0):
        if name is None:
            name = 'python-script'
        if desc is None:
            desc = 'added using the Python interface'
        eva = RASCH_if._if_eval_add(self.h, name, desc, original)
        if not eva:
            return None
        return RAeval(eva)

##### end of RAmeas namespace



########## RArec namespace
class RArec:
    def __init__(self, rec):
        self.h = rec

    def get_handle(self):
        return self.h

    def get_next(self):
        rec = RASCH_if._if_rec_get_next(self.h)
        if not rec:
            return None
        return RArec(rec)
    
    def get_child(self):
        rec = RASCH_if._if_rec_get_first_child(self.h)
        if not rec:
            return None
        return RArec(rec)
    
    def get_info(self, **var):
        if (not var.has_key('info')):
            return None

        if (var.has_key('dev')):
            ret = RASCH_if._if_dev_info_get(self.h, var['dev'], var['info'])
        elif (var.has_key('ch')):
            ret = RASCH_if._if_ch_info_get(self.h, var['ch'], var['info'])
        else:
            ret = RASCH_if._if_rec_info_get(self.h, var['info'])

        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def set_info(self, info, type, val):
        return RASCH_if._if_info_set(self.h, info, type, val)

    def get_meas_handle(self):
        return RASCH_if._if_meas_handle_from_any_handle(self.h)

    def get_raw(self, ch=0, start=-1, num=-1):
        return RASCH_if._if_raw_get(self.h, ch, start, num)

    def get_raw_unit(self, ch=0, start=-1, num=-1):
        return RASCH_if._if_raw_get_unit(self.h, ch, start, num)

##### end of RArec namespace


    
########## RAeval namespace
class RAeval:
    def __init__(self, eva):
        self.h = eva

    def get_handle(self):
        return self.h

    def get_attributes(self):
        return get_all_attributes(self.h)

    def get_info(self, **var):
        if (not var.has_key('info')):
            return None
        ret = RASCH_if._if_eval_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val
    
    def delete(self):
        ret = RASCH_if._if_eval_delete(self.h)
        self.h = None
        return ret
    
    def get_meas_handle(self):
        return RASCH_if._if_meas_handle_from_any_handle(self.h)

    def set_default(self):
        return RASCH_if._if_eval_set_default(self.h);

    def add_class(self, id, name='', desc=''):
        if id is None:
            return None
        clh = RASCH_if._if_class_add(self.h, id, name, desc)
        if not clh:
            return None
        return RAclass(clh)

    def add_class_predef(self, id):
        if id is None:
            return None
        clh = RASCH_if._if_class_add_predef(self.h, id)
        if not clh:
            return None
        return RAclass(clh)

    def get_class(self, id=''):
        clhs = RASCH_if._if_class_get(self.h, id)
        all = []
        if not clhs:
            return all        
        for c in clhs:
            curr = RAclass(c)
            all.append(curr)
        return all

    def save(self, fn='', save_ascii=0):
        return RASCH_if._if_eval_save(RASCH_if._if_meas_handle_from_any_handle(self.h), fn, save_ascii)

##### end of RAeval namespace



    
########## RAclass namespace
class RAclass:
    def __init__(self, clh):
        self.h = clh

    def get_handle(self):
        return self.h

    def get_attributes(self):
        return get_all_attributes(self.h)

    def get_info(self, **var):
        if (not var.has_key('info')):
            return None;
        ret = RASCH_if._if_class_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def delete(self):
        ret = RASCH_if._if_class_delete(self.h)
        self.h = None
        return ret

    def add_event(self, start, end):
        return RASCH_if._if_class_add_event(self.h, start, end)

    def del_event(self, ev_id):
        return RASCH_if._if_class_del_event(self.h, ev_id)

    def set_event_pos(self, ev_id, start, end):
        return RASCH_if._if_class_set_event_pos(self.h, ev_id, start, end)

    def get_event_pos(self, ev_id):
        return RASCH_if._if_class_get_event_pos(self.h, ev_id)

    def get_events(self, start=0, end=-1, complete=0, sort=1):
        return RASCH_if._if_class_get_events(self.h, start, end, complete, sort)

    def get_prev_event(self, ev_id):
        return RASCH_if._if_class_get_prev_event(self.h, ev_id)

    def get_next_event(self, ev_id):
        return RASCH_if._if_class_get_next_event(self.h, ev_id)

    def add_prop(self, id, val_type=None, num_val=0, name='', desc='', unit='',
                 min=None, max=None, ign_value=None):
        use_minmax = 0
        if min is not None and max is not None:
            use_minmax = 1
        has_ign_value = 0
        if ign_value is not None:
            has_ign_value = 1
        prop = RASCH_if._if_prop_add(self.h, id, val_type, num_val, name, desc, unit,
                                     use_minmax, min, max, has_ign_value, ign_value)
        if not prop:
            return None
        return RAprop(prop)

    def add_prop_predef(self, id):
        prop = RASCH_if._if_prop_add_predef(self.h, id)
        if not prop:
            return None
        return RAprop(prop)

    def get_prop(self, id):
        prop = RASCH_if._if_prop_get(self.h, id)
        if not prop:
            return None
        return RAprop(prop)

    def get_prop_all(self):
        all = RASCH_if._if_prop_get_all(self.h)
        props = []
        for p in all:
            curr = RAprop(p)
            props.append(curr)
        return props

    def add_sum(self, id, name='', desc='', dim_names=None, dim_units=None):
        if len(dim_names) != len(dim_units):
            return None
        sum = RASCH_if._if_sum_add(self.h, id, name, desc, len(dim_names), dim_names, dim_units)
        if not sum:
            return None
        return RAsum(sum)

    def get_sum(self, id):
        sum = RASCH_if._if_sum_get(self.h, id)
        if not sum:
            return None
        return RAsum(sum)

##### end of RAclass namespace



    
########## RAprop namespace
class RAprop:
    def __init__(self, prop):
        self.h = prop
        
    def get_handle(self):
        return self.h

    def get_attributes(self):
        return get_all_attributes(self.h)

    def get_info(self, **var):
        if (not var.has_key('info')):
            return None;
        ret = RASCH_if._if_prop_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def delete(self):
        ret = RASCH_if._if_prop_delete(self.h)
        self.h = None
        return ret

    def set_value(self, ev_id, ch, val_type, value):
        return RASCH_if._if_prop_set_value(self.h, ev_id, ch, val_type, value)

    def set_value_mass(self, ev_ids, chs, val_type, values):
        return RASCH_if._if_prop_set_value_mass(self.h, ev_ids, chs, val_type, values)

    def get_ch(self, ev_id):
        return RASCH_if._if_prop_get_ch(self.h, ev_id)

    def get_value(self, ev_id, ch):
        return RASCH_if._if_prop_get_value(self.h, ev_id, ch)

    def get_value_all(self):
        return RASCH_if._if_prop_get_values_all(self.h)

    # TODO: check if function works correct if no information is given; code in eval_prop.xml
    # has to be overworked
    def get_events(self, minmax_type='', min=None, max=None, ch=-1):
        return RASCH_if._if_prop_get_events(self.h, minmax_type, min, max, ch)

##### end of RAprop namespace




########## RAsum namespace
class RAsum:
    def __init__(self, sum):
        self.h = prop
        
    def get_handle(self):
        return self.h

    def get_attributes(self):
        return get_all_attributes(self.h)

    def get_info(self, **var):
        if (not var.has_key('info')):
            return None;
        ret = RASCH_if._if_sum_info_get(self.h, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def delete(self):
        ret = RASCH_if._if_sum_delete(self.h)
        self.h = None
        return ret

    def add_ch(self, ch=None, fiducial_offset=0):
        return RASCH_if._if_sum_add_ch(self.h, ch, fiducial_offset)

    def add_part(self, ev_ids):
        return RASCH_if._if_sum_add_part(self.h, ev_ids)

    def del_part(self, part_id):
        return RASCH_if._if_sum_del_part(self.h, part_id)

    def get_part_all(self):
        return RASCH_if._if_sum_get_part_all(self.h)

    def set_part_data(self, part_id, ch_ids, dim, val_type, value):
        return RASCH_if._if_sum_set_part_data(self.h, part_id, ch_idx, dim,
                                              val_type, value)

    def get_part_data(self, part_id, ch_idx, dim):
        return RASCH_if._if_sum_get_part_data(self.h, part_id, ch_idx, dim)

    def get_part_events(self, part_id):
        return RASCH_if_sum_get_part_events(self.h, part_id)

    def set_part_events(self, part_id, ev_ids):
        return RASCH_if_sum_set_part_events(self.h, part_id, ev_ids)

##### end of RAsum namespace



    
########## RAplugin namespace
class RAplugin:
    def __init__(self, pl):
        self.h = pl
        
    def get_handle(self):
        return self.h
        
    def get_info(self, **var):
        if (not var.has_key('info')):
            return None;
	num = 0
        if (var.has_key('num')):
            num = var['num']
        ret = RASCH_if._if_plugin_info_get(self.h, num, var['info'])
        if not ret:
            return None
        val = RAvalue(ret[0], ret[1], ret[2], ret[3])
        if len(ret) > 4:
            val.value_utf8(ret[4])
        return val

    def use_it(self):
        RASCH_if._if_use_it(self.h, 1)
        
    def dont_use_it(self):
        RASCH_if._if_use_it(self.h, 0)

    def get_result_info(self, res_num):
        name = RASCH_if._if_result_get_info(self.plugin, res_num, 'res_name')
        desc = RASCH_if._if_result_get_info(self.plugin, res_num, 'res_desc')
        default = RASCH_if._if_result_get_info(self.plugin, res_num, 'res_default')
        return [name[0], desc[0], default[0]]

    def get_process(self, meas):
        proc = RASCH_if._if_proc_get(meas.get_handle(), self.h)
        if not proc:
            return None
        return RAproc(proc, self.h)

##### end of RAplugin namespace



    

########## RAproc namespace
class RAproc:
    def __init__(self, proc, plugin):
        self.h = proc
        self.plugin = plugin
        
    def get_handle(self):
        return self.h

    def get_result_idx(self, res_ascii_id):
        return RASCH_if._if_proc_get_result_idx(self.h, res_ascii_id)
    
    def set_option(self, opt, type, val):
        return RASCH_if._if_lib_set_option(self.h, opt, type, val)

    def get_option(self, opt):
        return RASCH_if._if_lib_get_option(self.h, opt)
    
    def process(self):
        if RASCH_if._if_proc_do(self.h) != 0:
            return None
        
        res = RASCH_if._if_proc_get_result(self.h, self.plugin)
        vals = []
        for r in res:
            curr = RAvalue(r[0], r[1], r[2], 0)
            vals.append(curr)
        return vals

##### end of RAproc namespace
    
