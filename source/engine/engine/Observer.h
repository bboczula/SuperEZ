#pragma once
#include <vector>
#include <algorithm>

// Forward declarations (to get rid of Windows.h dependency)
struct HWND__;
using HWND = HWND__*;

// Define your WinMessageEvent struct
struct WinMessageEvent
{
      HWND hwnd;
      uint32_t msg;
      std::uintptr_t wParam;
      std::intptr_t lParam;
};

// Generic observer interface
template<typename Event>
class IObserver
{
public:
      virtual void OnNotify(Event& event) = 0;
      virtual ~IObserver() = default;
      virtual void Initialize() = 0;
      virtual void PostFrame() = 0;
};

// Subject class that manages observers
template<typename Event>
class Subject
{
      std::vector<IObserver<Event>*> observers;

public:
      void Subscribe(IObserver<Event>* observer)
      {
            observers.push_back(observer);
      }

      void Unsubscribe(IObserver<Event>* observer)
      {
            observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
      }

      void Notify(Event& event)
      {
            for (auto* obs : observers) {
                  obs->OnNotify(event);
            }
      }

	void RunPostFrame()
      {
		for (auto* obs : observers) {
			obs->PostFrame();
		}
	}
};