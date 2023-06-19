#ifndef FSM_H
#define FSM_H

#include <map>
#include <vector>
#include <functional>

class FSM {
    private:
        std::map<int, std::map<int, int>> transitions;
        std::vector<std::function<void(void)>> states;

        int totalStates;
        int totalEvents;
        int currentState;
        int lastEvent;

        unsigned long loopTiming;
        unsigned long totalLoopTiming;

    public:
        FSM(int totalStates, int totalEvents);

        void addState       (int state, std::function<void(void)> loopMethod);
        void addTransition  (int state, int event, int nextState);
        void raiseEvent     (int event);

        void setup          (int state, int eventStateExecuted);
        void loop           ();

        unsigned long getLoopTime();
};

#endif