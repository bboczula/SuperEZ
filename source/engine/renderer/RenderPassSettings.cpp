#include "RenderPassSettings.h"

#include "RenderPassSettings.h"

RenderPassSettingsGroup& RenderPassSettings::GetOrCreateGroup(const wchar_t* passName)
{
	for (RenderPassSettingsGroup& group : groups)
	{
		if (group.passName == passName)
		{
			return group;
		}
	}

	RenderPassSettingsGroup group;
	group.passName = passName;
	groups.push_back(group);

	return groups.back();
}

void RenderPassSettings::AddBool(
	const wchar_t* passName,
	const char* name,
	const char* label,
	bool* value,
	void (*onChanged)(void*),
	void* userData)
{
	RenderPassSettingsGroup& group = GetOrCreateGroup(passName);

	RenderPassSetting setting;
	setting.type = RenderPassSettingType::Bool;
	setting.name = name;
	setting.label = label;
	setting.value = value;
	setting.onChanged = onChanged;
	setting.userData = userData;

	group.settings.push_back(setting);
}

void RenderPassSettings::AddFloat(
	const wchar_t* passName,
	const char* name,
	const char* label,
	float* value,
	float min,
	float max,
	float step,
	void (*onChanged)(void*),
	void* userData)
{
	RenderPassSettingsGroup& group = GetOrCreateGroup(passName);

	RenderPassSetting setting;
	setting.type = RenderPassSettingType::Float;
	setting.name = name;
	setting.label = label;
	setting.value = value;
	setting.min = min;
	setting.max = max;
	setting.step = step;
	setting.onChanged = onChanged;
	setting.userData = userData;

	group.settings.push_back(setting);
}
