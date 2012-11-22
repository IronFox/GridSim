#include "../global_root.h"
#include "structure_simulation.h"

/******************************************************************

Experimental structure-simulator.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


/* file-structure:

0x4	u32	version = 0.1.0.0
0x4	u16[2]	object-count,layers_per_material
0x4	u32	commands
[object] // repeated
	0x8	u64	name
	0x80	f64[16]	matrix
	0x18	f64[3]	bary_center
	0x8	f64	weight
	0x4	u16[2]	visual_detail_cnt,children
	0x10	u16[4]	field_cnt,wheel_cnt,accelerator_cnt,struct_cnt
	[visual_detail] // repeated
		0x4	u32	visual_sheet_cnt
		[visual_sheet] // repeated
			0x4	u32	gl_data_type
			0x4	u32	vertex_cnt
			0x44	f32[17]	material_settings
			[layer] // repeated
				0x8	u64	texture_name
				0x10	f64[2]	boundaries
				0x4	bool[4]	bound,cube,active,mirror_map
			[vertex] // repeated
				0xC	f32[3]	coords
				0xC	f32[3]	normal
				0x8*	f32[2*]	texcoords
	[field] // repeated
		0x48	f64[9]	coords
		0x8	f64	intensity
		0x1	u8	channel
	[wheel] // repeated
		0x18	f64[3]	position
		0x18	f64[3]	root
		0x18	f64[3]	direction
		0x8	f64	radius
		0x8	f64	width
		0x8	f64	bumper_strength
		0x8	f64	air_consumption
		0x8	f64	power
		0x8	f64	weight
		[animator] // bumper
		[animator] // rotation
	[accelerator] // repeated
		0x18	f64[3]	position
		0x18	f64[3]	direction
		0x8	f64	air_consumption
		0x8	f64	power
	[structure] // repeated
		0x4	u32	wheel_cnd
		0x200	u16[0x100]	wheel_index
		[animator] // rotation
	[object] // one for each child
[animator] // repeated
	0x8	u64	name
	0x6	u16[3]	object_traces,acceleration_traces,wheel_traces
	[object_trace] // repeated
		0x8	u64	target_name
		0x4	u32	step_cnt
		[step] // repeated
			0x8	f64	length
			0x20	f64[4]	range
	[acceleration_trace] // repeated
		0x4	u32	target_index
		0x4	u32	step_cnt
		[astep] // repeated
			0x8	f64	length
			0x8	f64	range
	[wheel_trace] // repeated
		0x4	u32	target_index
		0x4	u32	step_cnt
		[wstep] // repeated
			0x8	f64	length
			0x10	f64[2]	range
*/

/* mass-sphere-effect-approximation

               v = 0.22/radius,
               average distance = radius/(center_dist*v+1)^5 + center_dist;
*/



