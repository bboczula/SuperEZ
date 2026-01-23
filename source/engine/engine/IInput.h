#pragma once
#include <cstdint>

enum class InputMouseButton : uint8_t { Left, Middle, Right };

class IInput
{
public:
      virtual ~IInput() = default;
      virtual bool MouseClicked(InputMouseButton button) = 0;
	virtual bool IsKeyDown(int keyCode) = 0;
};