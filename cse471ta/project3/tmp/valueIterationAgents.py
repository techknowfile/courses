# valueIterationAgents.py
# -----------------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


import mdp, util

from learningAgents import ValueEstimationAgent


class ValueIterationAgent(ValueEstimationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A ValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs value iteration
        for a given number of iterations using the supplied
        discount factor.
    """

    def __init__(self, mdp, discount=0.9, iterations=100):
        """
          Your value iteration agent should take an mdp on
          construction, run the indicated number of iterations
          and then act according to the resulting policy.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state, action, nextState)
              mdp.isTerminal(state)
        """
        self.mdp = mdp
        self.discount = discount
        self.iterations = iterations
        self.values = util.Counter()  # A Counter is a dict with default 0

        # Write value iteration code here
        "*** YOUR CODE HERE ***"

        #get number of iterations
        iterations = self.iterations
        for i in range(0, iterations, 1):
            print("This iterative:", i)
            #use Counter method for dictionary mapping
            dict = util.Counter()
            #mdp states
            states = self.mdp.getStates()

            #iterate through and set value to 0 if terminal, if not compare and add value
            #iteration formula
            for state in states:
                #is this terminal state
                if self.mdp.isTerminal(state):
                    dict[state] = 0
                    continue

                else:
                    #large negative value so it's always lesser at first iterative
                    compare_value = -999999
                    for action in self.mdp.getPossibleActions(state):
                        value = 0

                        for nextState, prob in self.mdp.getTransitionStatesAndProbs(state, action):
                            reward = self.mdp.getReward(state, action, nextState)
                            discount = self.discount
                            next_values = self.values[nextState]
                            value = value + (prob * (reward + (discount * next_values)))

                        compare_value = max(value, compare_value)
                        dict[state] = compare_value
            self.values = dict


    def getValue(self, state):
        """
          Return the value of the state (computed in __init__).
        """
        return self.values[state]

    def computeQValueFromValues(self, state, action):
        """
          Compute the Q-value of action in state from the
          value function stored in self.values.
        """
        "*** YOUR CODE HERE ***"

        #I believe Q Value is implemented correctly. No Need for debugging here.

        #set initial val
        q_value = 0
        #grab the transition functionality
        transition_values = self.mdp.getTransitionStatesAndProbs(state, action)

        #loop through each state in grid cell for reward and discount
        for next_state, probability in transition_values:
            reward = self.mdp.getReward(state, action, next_state)
            discount = self.discount

            #TD_Q-Learning iteration equation
            q_value = q_value + (probability * ((reward) + (discount * self.values[next_state])))

        #return Q Value
        return q_value


    def computeActionFromValues(self, state):
        """
          The policy is the best action in the given state
          according to the values currently stored in self.values.

          You may break ties any way you see fit.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return None.
        """
        "*** YOUR CODE HERE ***"
        terminal = self.mdp.isTerminal(state)
        actions = self.mdp.getPossibleActions(state)
        print(terminal)

        #check for terminal state, return None at exit
        if terminal is True:
            return None
        #continue exploring when not terminal
        else:
            #max_value is a comparator which starts off at -inf
            max_value = -9999999
            #choose maximum value for the action
            best_action = None

            #iterate through possible directions
            for index_action in actions:
                #get qvalue to compare
                q_value = self.computeQValueFromValues(state, index_action)
                #create temp as changing q_value is not appropriate
                temp = q_value
                #temp will be greater at first iteration
                if temp >= max_value:
                    #swap values, select best action at greatest indexed value
                    max_value = temp
                    best_action = index_action

        return best_action




        #util.raiseNotDefined()

    def getPolicy(self, state):
        return self.computeActionFromValues(state)

    def getAction(self, state):
        "Returns the policy at the state (no exploration)."
        return self.computeActionFromValues(state)

    def getQValue(self, state, action):
        return self.computeQValueFromValues(state, action)

