#ifndef engine_controlH
#define engine_controlH

#include "textout.h"
#include "aspect.h"
#include "../container/lvector.h"

namespace Engine
{

	class ControlCluster;

	
	
	/**
		@brief Pure abstract control module class for sequential game cores
		
		A Control instance describes an input hook for user input reaction as well as standardized rendering methods.
	*/
	class Control
	{
	private:
		friend class		ControlCluster;			
	protected:
		ControlCluster		*cluster;
	public:
		Control()			:cluster(NULL){}
		virtual				~Control(){}
							
		virtual	bool		onKeyDown(Key::Name)						{return false;};	//!< Sends a key down event to the local control
		virtual	bool		onKeyUp(Key::Name)							{return false;};	//!< Sends a key up event to the local control 
		virtual	bool		onMouseWheel(short delta)					{return false;};	//!< Sends a mouse wheel event to the local control
	
		virtual	void		advance(float delta)						{};					//!< Advances the frame. @param Last frames frame length. The control is encouraged to use this delta value rather than the global timing variable if slowmotion effects should be possible
	
		virtual	void		renderShaded(const Aspect<>&)				{};					//!< Triggers fully shaded rendering. No shader is bound at this time but layer0 textures may be bound. Lighting and depth test are enabled
		virtual	void		renderSchematics(const Aspect<>&)			{};					//!< Triggers schematic rendering. No shader or texture is bound at this time. Lighting and depth test are disabled
		virtual	void		renderShadow(const Aspect<>&)				{};					//!< Triggers shadow rendering. No shader or texture is bound at this time. Lighting is disabled, depth test is enabled
		virtual	void		renderHUD()									{};					//!< Triggers HUD rendering. The bound aspect is always orthographic from 0,0 to 1,1. Layer0 textures may be bound, shaders and depth test are disabled
		virtual	bool		detectNearestGroundHeight(const TVec3<>&reference_position,float&out_height)	{return false;};
	
		virtual	void		onInstall()									{};					//!< Invoked once this control module is installed on the specified control cluster
		virtual	void		onUninstall()								{};					//!< Invoked once this control module is installed on the specified control cluster
	};
	
	/**
		@brief Sequential collection of Control instances
		
		ControlCluster manages a series of controls.
	*/
	class ControlCluster
	{
	protected:
		List::ReferenceVector<Control>		control_stack;
	public:
		VirtualTextout		*textout;
	
		ControlCluster()	:textout(NULL)	{}
		virtual				~ControlCluster();
	
		bool				onKeyDown(Key::Name);					//!< Invokes onKeyDown() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
		bool				onKeyUp(Key::Name);						//!< Invokes onKeyUp() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
		bool				onMouseWheel(short delta);				//!< Invokes onMouseWheel() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
	
		void				advance(float delta);					//!< Invokes advance() methods of all contained control instances. Walks forwards through the list
			
		void				renderShadow(const Aspect<>&);
		void				renderShaded(const Aspect<>&);			//!< Invokes renderShaded() methods of all contained control instances. Walks forwards through the list
		void				renderSchematics(const Aspect<>&);		//!< Invokes renderSchematics() methods of all contained control instances. Walks forwards through the list
		void				renderHUD();							//!< Invokes renderHUD() methods of all contained control instances. Walks forwards through the list
	
		void				install(Control*);
		void				uninstall(Control*);
		bool				detectNearestGroundHeight(const TVec3<>&reference_position,float&out_height);
	};






}



#endif
