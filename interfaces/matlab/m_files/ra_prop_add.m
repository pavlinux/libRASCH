function [prop_handle] = ra_prop_add(class_handle, id, value_type, name, desc, unit, use_minmax, min, max, has_ignore_value, ignore_value)
% Returns the handle of the added event-property.
%     Input parameters:
%     class_handle
%     id
%     value_type
%     name
%     desc
%     unit
%     use_minmax
%     min
%     max
%     has_ignore_value
%     ignore_value
% 
%   Output parameters:
%     prop_handle

	[prop_handle] = ra_matlab('ra_prop_add', class_handle, id, value_type, name, desc, unit, use_minmax, min, max, has_ignore_value, ignore_value);
