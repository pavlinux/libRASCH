function [ok] = ra_class_delete(class_handle)
% Delete the event-class given by 'class_handle'.
%     Input parameters:
%     class_handle
% 
%   Output parameters:
%     ok

	[ok] = ra_octave('ra_class_delete', class_handle);
