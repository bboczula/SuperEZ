#pragma once

#include <vector>

enum class RenderPassSettingType
{
	Bool,
	Int,
	UInt,
	Float,
	Combo,
	ColorLegend,
	Text
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

	const char* const* comboItems = nullptr;
	int comboItemCount = 0;

	const char* const* legendLabels = nullptr;
	const unsigned int* legendColors = nullptr;
	int legendItemCount = 0;

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

	void AddCombo(
		const wchar_t* passName,
		const char* name,
		const char* label,
		int* value,
		const char* const* items,
		int itemCount,
		void (*onChanged)(void*) = nullptr,
		void* userData = nullptr);

	void AddColorLegend(
		const wchar_t* passName,
		const char* name,
		const char* label,
		const char* const* labels,
		const unsigned int* colors,
		int itemCount);

	// Read-only text block. 'text' must point to a buffer owned by the pass
	// that stays alive and can be rewritten every frame (e.g. a member char array).
	void AddText(
		const wchar_t* passName,
		const char* name,
		const char* label,
		const char* text);

	const std::vector<RenderPassSettingsGroup>& GetGroups() const
	{
		return groups;
	}
private:
	std::vector<RenderPassSettingsGroup> groups;
};
