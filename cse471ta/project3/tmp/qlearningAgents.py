# qlearningAgents.py
# ------------------
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


from game import *
from learningAgents import ReinforcementAgent
from featureExtractors import *

import random, util, math


class QLearningAgent(ReinforcementAgent):
    """
      Q-Learning Agent

      Functions you should fill in:
        - computeValueFromQValues
        - #computeActionFromQValues
        - #getQValue
        - getAction
        - update

      Instance variables you have access to
        - self.epsilon (exploration prob)
        - self.alpha (learning rate)
        - self.discount (discount rate)

      Functions you should use
        - self.getLegalActions(state)
          which returns legal actions for a state
    """

    def __init__(self, **args):
        "You can initialize Q-values here..."
        ReinforcementAgent.__init__(self, **args)

        "*** YOUR CODE HERE ***"

        self.dict = util.Counter()
        self.queue = util.Queue()
        # self.pair = [(state, action)]

    def getQValue(self, state, action):
        """
          Returns Q(state,action)
          Should return 0.0 if we have never seen a state
          or the Q node value otherwise
        """
        "*** YOUR CODE HERE ***"
        """
        """
        qnode = self.dict[(state, action)]

        if (state, action) not in self.dict:
            qnode = 0.0

        return qnode

    def computeValueFromQValues(self, state):
        """
          Returns max_action Q(state,action)
          where the max is over legal actions.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return a value of 0.0.
        """
        "*** YOUR CODE HERE ***"
        # get the possible actions
        legal_actions = self.getLegalActions(state)
        # instantiate to zero
        value, max_action = 0.0, 0.0
        # create an accounting list
        action_list = []
        # if there aren't any legal actions don't take any
        if legal_actions is None:
            return value
        else:
            for each_action in legal_actions:
                """move through each legal_action and add to list, then find max """

                value = self.getQValue(state, each_action)

                action_list.append(value)

                # print("each_action:", each_action, value)

        max_action = max(action_list)
        # print("max_action_value:", max_action)
        return max_action

    def computeActionFromQValues(self, state):
        """
          Compute the best action to take in a state.  Note that if there
          are no legal actions, which is the case at the terminal state,
          you should return None.
        """
        "*** YOUR CODE HERE ***"

        """For computeActionFromQValues, 
        you should break ties randomly for better behavior. The random.choice() function will help. 
        In a particular state, 
        actions that your agent hasn't seen before still have a Q-value, specifically a Q-value of zero, 
        and if all of the actions that your agent has seen before have a 
        negative Q-value, an unseen action may be optimal."""

        # get the possible actions
        legal_actions = self.getLegalActions(state)
        # instantiate to zero
        value = 0.0
        best_action = None
        # create an accounting list action_list
        action_list = []

        # if there aren't any legal actions don't take any
        if legal_actions is None:
            return value
        else:
            #create a tiebreak list in order to prevent assuming unseen is max
            tiebreak = []
            best_action = self.computeValueFromQValues(state)

            #iterate through each legal move
            for each_action in legal_actions:

                """move through each legal_action and add to list, then find max """
                value = self.getQValue(state, each_action)

                #only ammend tiebreak if there is a tie
                if value == best_action:
                    tiebreak.append(each_action)
                    best_action = random.choice(tiebreak)


        return best_action

    def getAction(self, state):
        """
          Compute the action to take in the current state.  With
          probability self.epsilon, we should take a random action and
          take the best policy action otherwise.  Note that if there are
          no legal actions, which is the case at the terminal state, you
          should choose None as the action.

          HINT: You might want to use util.flipCoin(prob)
          HINT: To pick randomly from a list, use random.choice(list)
        """
        legalActions = self.getLegalActions(state)
        action = None
        # Pick Action
        "*** YOUR CODE HERE ***"

        explore_prob = self.epsilon
        #flipcoin returns a True or false based on probability as an argument
        #print("flipcoin:", util.flipCoin(explore_prob))

        #check to make sure there is something to calculate
        if legalActions is None:
            action = None

        #randomize based on epsilon
        elif util.flipCoin(explore_prob) == True:

            action = random.choice(legalActions)

        #most of the time keep regular policy
        else:
            action = self.getPolicy(state)

        return action

    def update(self, state, action, nextState, reward):
        """
          The parent class calls this to observe a
          state = action => nextState and reward transition.
          You should do your Q-Value update here

          NOTE: You should never call this function,
          it will be called on your behalf
        """
        "*** YOUR CODE HERE ***"

        """ Q(s,a) <-- Q(s,a) + alpha * (R(s) + gamma*Q(s',a') - Q(s,a)) """



        q_value = self.dict[(state, action)]
        #next_q_value = self.dict[(nextState, nextAction)]
        nextActions =  self.getLegalActions(nextState)
        alpha = self.alpha
        discount = self.discount

        #default
        q_update = reward

        #check if there are available actions
        if len(nextActions) != 0:
            #get the sample to update
            q_update = reward + (discount * max([self.getQValue(nextState, nextAction)
                for nextAction in nextActions]))
        #evaluate with added sample.
        self.dict[(state, action)] = ((1 - alpha) * self.getQValue(state, action) + (alpha * q_update))




    def getPolicy(self, state):
        return self.computeActionFromQValues(state)

    def getValue(self, state):
        return self.computeValueFromQValues(state)


class PacmanQAgent(QLearningAgent):
    "Exactly the same as QLearningAgent, but with different default parameters"

    def __init__(self, epsilon=0.05, gamma=0.8, alpha=0.2, numTraining=0, **args):
        """
        These default parameters can be changed from the pacman.py command line.
        For example, to change the exploration rate, try:
            python pacman.py -p PacmanQLearningAgent -a epsilon=0.1

        alpha    - learning rate
        epsilon  - exploration rate
        gamma    - discount factor
        numTraining - number of training episodes, i.e. no learning after these many episodes
        """
        args['epsilon'] = epsilon
        args['gamma'] = gamma
        args['alpha'] = alpha
        args['numTraining'] = numTraining
        self.index = 0  # This is always Pacman
        QLearningAgent.__init__(self, **args)

    def getAction(self, state):
        """
        Simply calls the getAction method of QLearningAgent and then
        informs parent of action for Pacman.  Do not change or remove this
        method.
        """
        action = QLearningAgent.getAction(self, state)
        self.doAction(state, action)
        return action


class ApproximateQAgent(PacmanQAgent):
    """
       ApproximateQLearningAgent

       You should only have to overwrite getQValue
       and update.  All other QLearningAgent functions
       should work as is.
    """

    def __init__(self, extractor='IdentityExtractor', **args):
        self.featExtractor = util.lookup(extractor, globals())()
        PacmanQAgent.__init__(self, **args)
        self.weights = util.Counter()

    def getWeights(self):
        return self.weights

    def getQValue(self, state, action):
        """
          Should return Q(state,action) = w * featureVector
          where * is the dotProduct operator
        """
        "*** YOUR CODE HERE ***"
        util.raiseNotDefined()

    def update(self, state, action, nextState, reward):
        """
           Should update your weights based on transition
        """
        "*** YOUR CODE HERE ***"

    def final(self, state):
        "Called at the end of each game."
        # call the super-class final method
        PacmanQAgent.final(self, state)

        # did we finish training?
        if self.episodesSoFar == self.numTraining:
            # you might want to print your weights here for debugging
            "*** YOUR CODE HERE ***"
            pass
