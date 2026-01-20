#pragma once
#include <string>

      class IGame
      {
      public:
            virtual ~IGame() = default;
      
            // Return the scene name to load (e.g. "sponza", "chess", etc.)
            // This maps to your existing LoadSceneAssets(sceneName).
            virtual std::string GetStartupSceneName() const = 0;
      };