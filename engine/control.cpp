#include "../global_root.h"
#include "control.h"


namespace Engine
{
	ControlCluster::~ControlCluster()
	{
		if (!application_shutting_down)
		{
			for (unsigned i = 0; i < control_stack.count(); i++)
			{
				control_stack[i]->onUninstall();
				control_stack[i]->cluster = NULL;
			}
		}
		control_stack.flush();
	}
	
	
	bool		ControlCluster::onKeyDown(Key::Name key)
	{
		for (index_t i = control_stack.count()-1; i < control_stack.count(); i--)
			if (control_stack[i]->onKeyDown(key))
				return true;
		return false;
	}
	
	bool		ControlCluster::onKeyUp(Key::Name key)
	{
		for (index_t i = control_stack.count()-1; i < control_stack.count(); i--)
			if (control_stack[i]->onKeyUp(key))
				return true;
		return false;
	}
	
	bool		ControlCluster::onMouseWheel(short delta)
	{
		for (index_t i = control_stack.count()-1; i < control_stack.count(); i--)
			if (control_stack[i]->onMouseWheel(delta))
				return true;
		return false;
	}
	
	
	void		ControlCluster::advance(float delta)
	{
		for (index_t i = 0; i < control_stack.count(); i++)
			control_stack[i]->advance(delta);
	}
	
			
	void		ControlCluster::renderShaded(const Aspect<>&aspect)
	{
		for (index_t i = 0; i < control_stack.count(); i++)
			control_stack[i]->renderShaded(aspect);
	}

	void		ControlCluster::renderShadow(const Aspect<>&aspect)
	{
		for (index_t i = 0; i < control_stack.count(); i++)
			control_stack[i]->renderShadow(aspect);
	}

	void		ControlCluster::renderSchematics(const Aspect<>&aspect)
	{
		for (index_t i = 0; i < control_stack.count(); i++)
			control_stack[i]->renderSchematics(aspect);
	}
	
	void		ControlCluster::renderHUD()
	{
		for (index_t i = 0; i < control_stack.count(); i++)
			control_stack[i]->renderHUD();
	}
	
	
	void		ControlCluster::install(Control*control)
	{
		if (!this || !control || control->cluster == this)
			return;
		if (control->cluster)
			control->onUninstall();
		control->cluster = this;
		control->onInstall();
		control_stack.append(control);
	}
	
	void		ControlCluster::uninstall(Control*control)
	{
		if (!this || !control || control->cluster != this)
			return;
		control->onUninstall();
		control->cluster = NULL;
		control_stack.drop(control);
	}

	bool		ControlCluster::detectNearestGroundHeight(const TVec3<>&reference_position,float&out_height)
	{
		out_height = std::numeric_limits<float>::min();
		bool result = false;
		for (index_t i = 0; i < control_stack.count(); i++)
		{
			Control*control = control_stack[i];
			result |= control->detectNearestGroundHeight(reference_position,out_height);
		}
		return result;
	}

	void		ControlCluster::signalResolutionChange()
	{
		for (index_t i = 0; i < control_stack.count(); i++)
		{
			control_stack[i]->onResolutionChange();
		}
	}


}
