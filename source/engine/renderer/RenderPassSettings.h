#pragma once

#include <vector>

enum class RenderPassSettingType
{
	Bool,
	Int,
	UInt,
	Float
};

struct RenderPassSetting
{
	RenderPassSettingType type;

	const char* name;   // Stable internal id, e.g. "shadow_bias"
	const char* label;  // UI label, e.g. "Shadow Bias"

	void* value;        // Pointer to the actual variable owned by the pass

	float min = 0.0f;
	float max = 0.0f;
	float step = 1.0f;

	void (*onChanged)(void* userData) = nullptr;
	void* userData = nullptr;
};

struct RenderPassSettingsGroup
{
	const wchar_t* passName; // or const char* if easier for ImGui
	std::vector<RenderPassSetting> settings;
};

class RenderPassSettings
{
public:
	RenderPassSettings() = default;
	~RenderPassSettings() = default;
	// Add any necessary member variables and methods here
	RenderPassSettingsGroup& GetOrCreateGroup(const wchar_t* passName);

	void AddBool(
		const wchar_t* passName,
		const char* name,
		const char* label,
		bool* value,
		void (*onChanged)(void*) = nullptr,
		void* userData = nullptr);

	void AddFloat(
		const wchar_t* passName,
		const char* name,
		const char* label,
		float* value,
		float min,
		float max,
		float step,
		void (*onChanged)(void*) = nullptr,
		void* userData = nullptr);

	const std::vector<RenderPassSettingsGroup>& GetGroups() const
	{
		return groups;
	}
private:
	std::vector<RenderPassSettingsGroup> groups;
};
