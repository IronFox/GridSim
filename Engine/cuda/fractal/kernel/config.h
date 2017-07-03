#ifndef kernel_configH
#define kernel_configH

#define FRACTAL_VERSION	"v4.37"


//#define COLOR_CHANNELS	4		//!< Overrides standard channel to texel conversion and simply outputs a section of the available channel data (0-3)



#define FULL_KERNEL		1		//!< Uses all 5 general purpose noise variables
#define REDUCED_KERNEL	2		//!< 4 general purpose noise variables
#define MINIMAL_KERNEL	3		//!< 3 general purpose noise variables

#define FRACTAL_KERNEL	REDUCED_KERNEL


//#define __USE_SHARED_MEMORY__		//!< Define to enable shared memory usage

#define SELECTIVE_WEIGHTING	0		//!< Manipulates the influence of secondary neighbors based on their constellation. 0 for disabled, 1 for enabled


#define SECONDARY_PARENT_WEIGHT	0.25f	//!< Influence of secondary parents

#define TWO_PARENTS_BALANCED		0	//!< Non age sensitive two parent system for illustration
#define TWO_PARENTS_AGE_SENSITIVE	1	//!< Age sensitive two parent system for illustration
#define SIMPLE_BALANCED				2	//!< Non age sensitive four parent system for illustration
#define FIXED_WEIGHT				3	//!< Fixed weight based on SECONDARY_PARENT_WEIGHT
#define HALF_AGE_SENSITIVE			4	//!< Only ages of secondary parents are read. Primary weights are the remainder of 1
#define BALANCED_AGE_SENSITIVE		5	//!< All age values are read but primary parents are balanced
#define FULL_AGE_SENSITIVE			6	//!< All age values are read. No balancing is performed
#define SELECTIVE_AGE_SENSITIVE		7	//!< Binarily decides whether to include all or just one vertex based on their age

#define WEIGHTING				FULL_AGE_SENSITIVE	//!< Current weighting strategy

#define AGE_BASE			1.1f//1.1f	//1.2f	//!< Age exponential base

#define CONTINENTAL_NOISE	0.06f


#define SIMPLE_NORMAL_MERGE		0	//!< Only one normal per vertex, simply sum upon edge merge. Edges are merged only once, then flagged, later copied. Should be consistent.
#define DOUBLE_NORMAL_STORAGE	1	//!< Two normals per vertex, one the base normal, the other the final one

#define NORMAL_GENERATION		SIMPLE_NORMAL_MERGE



#define	WATER_GAIN_DOWN 	1.1		//!< Water factor applied to water from a superior parent vertex 
#define	WATER_GAIN_UP		0.9		//!< Water factor applied to water from an infirior parent vertex
#define	WATER_NOISE			0.05	//!< Water noise level

//custom noise factors appending during noise application. each must include the operator sign (eg. *0.5f). Leave plank if no modification should take place (100%) :
#define C0_CUSTOM_NOISE_FACTOR	*0.9f
#define C1_CUSTOM_NOISE_FACTOR	*0.8f
#define C2_CUSTOM_NOISE_FACTOR				//canyon channel (in addition to other functions)
#define C3_CUSTOM_NOISE_FACTOR	*1.2f
#define C4_CUSTOM_NOISE_FACTOR



#define TEXTURE_SEAM_EXTENSION	2	//!< Additional separation texels on both sides of the diagonal of a segment texture. Filled with clamped vertex data

//#define	RIVERS						//!< Define to simulate rivers




#endif
