function [OK] = ra_sum_set_part_events(sum_handle, part_id, event_ids)
% Returns for an event-summary part the event-ids which are the part is based on.
%   Input parameters:
%     sum_handle
%     part_id
%     event_ids
% 
%   Output parameters:
%     OK

	[OK] = ra_octave('ra_sum_set_part_events', sum_handle, part_id, event_ids);
