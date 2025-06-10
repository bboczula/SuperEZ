#pragma once

#include "../Observer.h"

class ImGuiHandler : public IObserver<WinMessageEvent>
{
public:
	void OnNotify(WinMessageEvent& event) override;
	void Initialize() override;
	void PostFrame() override;
};