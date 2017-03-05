function [OK] = ra_class_del_event(class_handle, event_id)
% Delete the event with the id 'event_id'.
%     Input parameters:
%     class_handle
%     event_id
% 
%   Output parameters:
%     OK

	[OK] = ra_matlab('ra_class_del_event', class_handle, event_id);
