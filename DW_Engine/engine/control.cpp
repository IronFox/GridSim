#include "../global_root.h"
#include "control.h"
//#include "../math/graph.h"
#include "../container/sorter.h"

namespace Engine
{
	void		RenderSequence::Insert(FRenderInstruction instruction, index_t orderIndex)
	{
		ASSERT__(!isSealed);
		wrappers.append(RenderInstructionWrapper(orderIndex,instruction));
	}

	void		RenderSequence::Seal()
	{
		ASSERT__(!isSealed);
		isSealed = true;

		Sorting::ByMethod::quickSort(wrappers);
	}





	ControlCluster::~ControlCluster()
	{
		if (!application_shutting_down)
		{
			for (unsigned i = 0; i < controlStack.size(); i++)
			{
				controlStack[i]->OnUninstall();
				controlStack[i]->cluster = NULL;
			}
		}
		controlStack.clear();
	}
	
	
	bool		ControlCluster::OnKeyDown(Key::Name key)
	{
		for (index_t i = controlStack.size()-1; i < controlStack.size(); i--)
			if (controlStack[i]->OnKeyDown(key))
				return true;
		return false;
	}
	
	bool		ControlCluster::OnKeyUp(Key::Name key)
	{
		for (index_t i = controlStack.size()-1; i < controlStack.size(); i--)
			if (controlStack[i]->OnKeyUp(key))
				return true;
		return false;
	}
	
	bool		ControlCluster::OnMouseWheel(short delta)
	{
		for (index_t i = controlStack.size()-1; i < controlStack.size(); i--)
			if (controlStack[i]->OnMouseWheel(delta))
				return true;
		return false;
	}
	
	
	void		ControlCluster::Advance(float delta)
	{
		for (index_t i = 0; i < controlStack.size(); i++)
			controlStack[i]->Advance(delta);

		accumulatedTime += delta;
		while (accumulatedTime > fixedFrameDelta)
		{
			accumulatedTime -= fixedFrameDelta;
			for (index_t i = 0; i < controlStack.size(); i++)
				controlStack[i]->FixedUpdate(fixedFrameDelta);
		}

		for (index_t i = 0; i < controlStack.size(); i++)
			controlStack[i]->PostAdvance();

	}
	
			
	void		ControlCluster::Shutdown()
	{
		for (index_t i = 0; i < controlStack.size(); i++)
			controlStack[i]->Shutdown();
	}
	
	
	void		ControlCluster::Install(Control*control)
	{
		if (!this || !control || control->cluster == this)
			return;
		if (control->cluster)
			control->OnUninstall();
		control->cluster = this;
		control->OnInstall(sequenceMap);
		controlStack.append(control);
	}
	
	void		ControlCluster::Uninstall(Control*control)
	{
		if (!this || !control || control->cluster != this)
			return;
		control->OnUninstall();
		control->cluster = NULL;
		controlStack.findAndErase(control);
	}

	bool		ControlCluster::DetectNearestGroundHeight(const TVec3<>&reference_position,float&out_height, TVec3<>&outNormal)
	{
		out_height = -std::numeric_limits<float>::max();
		bool result = false;
		for (index_t i = 0; i < controlStack.size(); i++)
		{
			Control*control = controlStack[i];
			result |= control->DetectNearestGroundHeight(reference_position,out_height, outNormal);
		}
		return result;
	}

	void		ControlCluster::SignalResolutionChange(const Resolution&newResolution, bool isFinal)
	{
		for (index_t i = 0; i < controlStack.size(); i++)
		{
			controlStack[i]->OnResolutionChange(newResolution, isFinal);
		}
	}


}
