#pragma once

namespace fengine {
    class LoopTime {
    private:
        static float deltaTime_;
        static float time_;

        static void setDeltaTime(float frameTime) { deltaTime_ = frameTime; }
        static void setTime(float time) { time_ = time; }

        friend class App;

    public:
        // Public getter to access the variable
        static float getDeltaTime() {
            return deltaTime_;
        }
        static float getTime() {
            return time_;
        }
    };
}