#pragma once
#include <vector>
#include <algorithm>
#include <Windows.h>

// Define your WinMessageEvent struct
struct WinMessageEvent {
      HWND hwnd;
      UINT msg;
      WPARAM wParam;
      LPARAM lParam;
};

// Generic observer interface
template<typename Event>
class IObserver {
public:
      virtual void OnNotify(Event& event) = 0;
      virtual ~IObserver() = default;
      virtual void Initialize() = 0;
      virtual void PostFrame() = 0;
};

// Subject class that manages observers
template<typename Event>
class Subject {
      std::vector<IObserver<Event>*> observers;

public:
      void Subscribe(IObserver<Event>* observer) {
            observers.push_back(observer);
      }

      void Unsubscribe(IObserver<Event>* observer) {
            observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
      }

      void Notify(Event& event) {
            for (auto* obs : observers) {
                  obs->OnNotify(event);
            }
      }

	void RunPostFrame() {
		for (auto* obs : observers) {
			obs->PostFrame();
		}
	}
};