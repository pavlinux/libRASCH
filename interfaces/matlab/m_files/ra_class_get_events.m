function [value] = ra_class_get_events(class_handle, start_pos, end_pos, complete, sort)
% 
%     Returns all events which are between 'start_pos' and 'end_pos'. If the flag
%     'complete' is set, only these events are returned which starts AND ends in
%     region. If the flag 'sort' is set, the returned events are sorted according
%     to their position.
%     Input parameters:
%     class_handle
%     start_pos
%     end_pos
%     complete
%     sort
% 
%   Output parameters:
%     value

	[value] = ra_matlab('ra_class_get_events', class_handle, start_pos, end_pos, complete, sort);
