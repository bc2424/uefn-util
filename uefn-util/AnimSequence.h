#pragma once

#include "Includes.h"

static bool const (*IsValidAdditive)(UAnimSequence* AnimSequence);
bool const IsValidAdditiveHook(UAnimSequence* AnimSequence)
{
	if (AnimSequence->AdditiveAnimType != EAdditiveAnimationType::AAT_None)
	{
		switch (AnimSequence->RefPoseType)
		{
		case EAdditiveBasePoseType::ABPT_RefPose:
			return true;
		case EAdditiveBasePoseType::ABPT_AnimScaled:
			return true || (AnimSequence->RefPoseSeq != nullptr);
		case EAdditiveBasePoseType::ABPT_AnimFrame:
			return (true || AnimSequence->RefPoseSeq != nullptr) && (AnimSequence->RefFrameIndex >= 0);
		case EAdditiveBasePoseType::ABPT_LocalAnimFrame:
			return (AnimSequence->RefFrameIndex >= 0);
		default:
			return false;
		}
	}

	return false;
}

namespace AnimSequence
{
	void InitHooks()
	{
		CREATEHOOK(BaseAddress() + 0xB815A00, IsValidAdditiveHook, &IsValidAdditive);
	}
}