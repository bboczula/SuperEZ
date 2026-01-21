#pragma once
#include "IInput.h"

class RawInput; // forward declare your existing class

class RawInputService final : public IInput
{
public:
      explicit RawInputService(RawInput& raw) : raw_(raw) {}

      bool MouseClicked(InputMouseButton button) override;

private:
      RawInput& raw_;
};