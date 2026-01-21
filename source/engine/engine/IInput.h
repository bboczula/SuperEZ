#pragma once
#include <cstdint>

enum class InputMouseButton : uint8_t { Left, Middle, Right };

class IInput
{
public:
      virtual ~IInput() = default;
      virtual bool MouseClicked(InputMouseButton button) = 0;
};