#ifndef FSM_H
#define FSM_H

#include <map>
#include <vector>
#include <functional>

typedef uint8_t event_t;
typedef uint8_t state_t;

class FSM {
    private:
        std::map<state_t, std::map<event_t, state_t>> transitions;
        std::vector<std::function<void(void)>> states;

        int totalStates;
        int totalEvents;
        int currentState{};

        unsigned long loopTiming{};
        unsigned long totalLoopTiming{};

    public:
        FSM(int totalStates, int totalEvents);

        void addState       (state_t state, std::function<void(void)> loopMethod);
        void addTransition  (state_t state, event_t event, state_t nextState);
        void raiseEvent     (event_t event);

        void setup          (state_t state);
        void loop           ();

        unsigned long getLoopTime();
};

#endif