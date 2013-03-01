#ifndef engine_controlH
#define engine_controlH

#include "textout.h"
#include "aspect.h"
#include "../container/buffer.h"
#include "../math/resolution.h"

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
							
		virtual	bool		OnKeyDown(Key::Name)						{return false;};	//!< Sends a key down event to the local control
		virtual	bool		OnKeyUp(Key::Name)							{return false;};	//!< Sends a key up event to the local control 
		virtual	bool		OnMouseWheel(short delta)					{return false;};	//!< Sends a mouse wheel event to the local control
	
		virtual	void		Advance(float delta)						{};					//!< Advances the frame. @param Last frames frame length. The control is encouraged to use this delta value rather than the global timing variable if slowmotion effects should be possible
	
		virtual	void		RenderShaded(const Aspect<>&, const Resolution&)				{};					//!< Triggers fully shaded rendering. No shader is bound at this time but layer0 textures may be bound. Lighting and depth test are enabled
		virtual	void		RenderShadedReflection(const Aspect<>&aspect, const Resolution&res){RenderShaded(aspect,res);};					//!< Triggers fully shaded rendering. No shader is bound at this time but layer0 textures may be bound. Lighting and depth test are enabled
		virtual	void		RenderSchematics(const Aspect<>&, const Resolution&)			{};					//!< Triggers schematic rendering. No shader or texture is bound at this time. Lighting and depth test are disabled
		virtual	void		RenderShadow(const Aspect<>&, const Resolution&)				{};					//!< Triggers shadow rendering. No shader or texture is bound at this time. Lighting is disabled, depth test is enabled
		virtual	void		RenderHUD(const Resolution&)									{};					//!< Triggers HUD rendering. The bound aspect is always orthographic from 0,0 to 1,1. Layer0 textures may be bound, shaders and depth test are disabled
		virtual	bool		DetectNearestGroundHeight(const TVec3<>&referencePosition,float&outHeight)	{return false;};
		virtual void		OnResolutionChange(const Resolution&newResolution, bool isFinal)		{};
		virtual void		Shutdown()									{};					//!< Signals that the applications is being shut down

		virtual	void		OnInstall()									{};					//!< Invoked once this control module is installed on the specified control cluster
		virtual	void		OnUninstall()								{};					//!< Invoked once this control module is installed on the specified control cluster
		ControlCluster*		GetCluster()	const						{return cluster;}
	};
	
	/**
		@brief Sequential collection of Control instances
		
		ControlCluster manages a series of controls.
	*/
	class ControlCluster
	{
	protected:
		Buffer<Control*,0>	controlStack;
	public:
		VirtualTextout		*textout;
	
		ControlCluster()	:textout(NULL)	{}
		virtual				~ControlCluster();
	
		bool				OnKeyDown(Key::Name);					//!< Invokes onKeyDown() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
		bool				OnKeyUp(Key::Name);						//!< Invokes onKeyUp() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
		bool				OnMouseWheel(short delta);				//!< Invokes onMouseWheel() methods of all contained control instances. Walks backwards through the list and stops at the first that returns true @return true if any contained instance returned true, false otherwise
	
		void				Advance(float delta);					//!< Invokes advance() methods of all contained control instances. Walks forward through the list
			
		void				RenderShadow(const Aspect<>&, const Resolution&);
		void				RenderShaded(const Aspect<>&, const Resolution&);			//!< Invokes renderShaded() methods of all contained control instances. Walks forward through the list
		void				RenderShadedReflection(const Aspect<>&, const Resolution&);			//!< Invokes renderShadedReflection() methods of all contained control instances. Walks forward through the list
		void				RenderSchematics(const Aspect<>&, const Resolution&);		//!< Invokes renderSchematics() methods of all contained control instances. Walks forward through the list
		void				RenderHUD(const Resolution&);							//!< Invokes renderHUD() methods of all contained control instances. Walks forward through the list
		void				Shutdown();								//!< Signals that the application is being shut down

		void				Install(Control*);
		void				Uninstall(Control*);
		bool				DetectNearestGroundHeight(const TVec3<>&referencePosition,float&outHeight);
		void				SignalResolutionChange(const Resolution&newResolution, bool isFinal);
	};






}



#endif
