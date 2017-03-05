function [ok] = ra_eval_attribute_unset(handle, attrib_name)
% 
%     Unset the value of the attribute 'attrib_name'.
%     Input parameters:
%     handle
%     attrib_name
% 
%   Output parameters:
%     ok

	[ok] = ra_matlab('ra_eval_attribute_unset', handle, attrib_name);
