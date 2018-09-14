# multiAgents.py
# --------------
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


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent


class ReflexAgent(Agent):
    """
      A reflex agent chooses an action at each choice point by examining
      its alternatives via a state evaluation function.

      The code below is provided as a guide.  You are welcome to change
      it in any way you see fit, so long as you don't touch our method
      headers.
    """

    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {North, South, West, East, Stop}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices)  # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        "*** YOUR CODE HERE ***"
        curFood = currentGameState.getFood()
        curPos = currentGameState.getPacmanPosition()

        def inDirections(pos1, pos2):
            dir = [False] * 5
            if pos1[0] > pos2[0]:
                dir[1] = True
            if pos1[0] < pos2[0]:
                dir[2] = True
            if pos1[1] > pos2[1]:
                dir[3] = True
            if pos1[1] < pos2[1]:
                dir[4] = True
            if pos1[0] == pos2[0] and pos1[1] == pos2[1]:
                dir[0] = True
            return dir

        foodlist = curFood.asList()
        closestfood = [foodlist[0]]
        for foodpos in foodlist[1:]:
            if util.manhattanDistance(foodpos, curPos) < util.manhattanDistance(closestfood[0], curPos):
                closestfood = [foodpos]
            elif util.manhattanDistance(foodpos, curPos) == util.manhattanDistance(closestfood[0], curPos):
                closestfood.append(foodpos)

        curGhost = {util.manhattanDistance(ghost, curPos): ghost for ghost in successorGameState.getGhostPositions()}
        closeGhostPos = curGhost[min(curGhost.keys())]

        ghostDirs = inDirections(closeGhostPos, curPos)
        newDirs = inDirections(newPos, curPos)
        closeFoodDirs = [reduce(lambda a, b: a or b, list) for list in
                         zip(*[inDirections(food, curPos) for food in closestfood])]

        index = newDirs.index(True)
        ghostInDir = ghostDirs[index]
        foodInDir = closeFoodDirs[index]

        stay = index == 0

        # if we're moving away from ghost and we're going towards food
        if not (ghostInDir) and not stay and foodInDir:
            return 70
        # if we're moving towards a ghost but it's far away and there's food
        elif ghostInDir and not stay and min(curGhost.keys()) > 5 and foodInDir:
            return 60
        # if we're moving away from ghost but there's no food
        elif not (ghostInDir) and not stay and not (foodInDir):
            return 50
        # if we're moving towards a ghost but its far, but there's no food
        elif ghostInDir and not stay and min(curGhost.keys()) > 5 and not (foodInDir):
            return 40
        # we're not moving and the ghost is far
        elif stay and min(curGhost.keys()) > 5:
            return 30
        # we're not moving and the ghost is close
        elif stay:
            return 20
        # if the ghost is close and we're moving towards it
        elif ghostInDir and not stay and min(curGhost.keys()) > 2:
            return 10
        # if the ghost will kill us
        else:
            return 0


def scoreEvaluationFunction(currentGameState):
    """
      This default evaluation function just returns the score of the state.
      The score is the same one displayed in the Pacman GUI.

      This evaluation function is meant for use with adversarial search agents
      (not reflex agents).
    """
    return currentGameState.getScore()


class MultiAgentSearchAgent(Agent):
    """
      This class provides some common elements to all of your
      multi-agent searchers.  Any methods defined here will be available
      to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

      You *do not* need to make any changes here, but you can if you want to
      add functionality to all your adversarial search agents.  Please do not
      remove anything, however.

      Note: this is an abstract class: one that should not be instantiated.  It's
      only partially specified, and designed to be extended.  Agent (game.py)
      is another abstract class.
    """

    def __init__(self, evalFn='scoreEvaluationFunction', depth='2'):
        self.index = 0  # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)


class MinimaxAgent(MultiAgentSearchAgent):
    """
      Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
          Returns the minimax action from the current gameState using self.depth
          and self.evaluationFunction.

          Here are some method calls that might be useful when implementing minimax.

          gameState.getLegalActions(agentIndex):
            Returns a list of legal actions for an agent
            agentIndex=0 means Pacman, ghosts are >= 1

          gameState.generateSuccessor(agentIndex, action):
            Returns the successor game state after an agent takes an action

          gameState.getNumAgents():
            Returns the total number of agents in the game
        """
        "*** YOUR CODE HERE ***"
        return self.value(gameState, 0, 0)[-1]

    def value(self, gameState, agent, depth):
        if depth >= self.depth:  # Base case occurs when the depth is at max or no moves can be created
            return [self.evaluationFunction(gameState)]
        else:  # If the base case is not satisfied
            # All of the possible new states for a given agent and the move that generated them
            newStates = [(gameState.generateSuccessor(agent, action), action) for action in gameState.getLegalActions(agent)]
            if not newStates:
                newStates = [(gameState, None)]
            nextAgent = (agent + 1) % gameState.getNumAgents()
            delta = nextAgent == 0

            if agent == 0:  # if Pacman is up
                # The best move is the max of all of the next agents move values
                return max([self.value(state, nextAgent, depth)+[action] for state, action in newStates])
            else:  # if a ghost is up
                # The best move is the min of all of the next agents move values
                return min([self.value(state, nextAgent, depth + delta)+[action] for state, action in newStates])

    # Working value function but how lists in recursion is handled was bad
    # def value(self, gameState, agent, depth):
    #     # Base case occurs when the depth is at max or no moves can be created
    #     if depth >= self.depth:
    #         return self.evaluationFunction(gameState)
    #
    #     # If the base case is not satisfied
    #     else:
    #         # All of the possible new states for a given agent and the move that generated them
    #         newStates = [(gameState.generateSuccessor(agent, action), action) for action in gameState.getLegalActions(agent)]
    #         if not newStates:
    #             newStates = [(gameState, None)]
    #         nextAgent = (agent + 1) % gameState.getNumAgents()
    #
    #         delta = nextAgent == 0
    #
    #         #if pacman is up
    #         if agent == 0:
    #             # The best move is the max of all of the next agents move values
    #             return max([self.value(state, nextAgent, depth), action] for state, action in newStates)
    #         #if a ghost is up
    #         else:
    #             # The best move is the min of all of the next agents move values
    #             return min([self.value(state, nextAgent, depth + delta), action] for state, action in newStates)

    # failed value function
    # def value(self, gameState, agent, depth):
    #     newStates = [(gameState.generateSuccessor(agent, action), action) for action in gameState.getLegalActions(agent)]
    #     if depth < self.depth:
    #         if newStates:
    #             if agent == 0:
    #                 return max([(self.value(state[0], (agent + 1) % gameState.getNumAgents(), depth+1), state[1]) for state in newStates])
    #             else:
    #                 return min([(self.value(state[0], (agent+1) % gameState.getNumAgents(), depth), state[1]) for state in newStates])
    #         else:
    #             if agent == 0:
    #                 return [self.value(gameState, (agent+1)%gameState.getNumAgents(), depth+1), None]
    #             else:
    #                 return [self.value(gameState, (agent+1)%gameState.getNumAgents(), depth), None]
    #     else:
    #         best = max([(self.evaluationFunction(state), action) for state, action in newStates])
    #         while type(best)==list:
    #             best = best[0]
    #             print best
    #         return best

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
      Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
          Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        return self.value(gameState, 0, 0, float("-inf"), float("inf"))[-1]

    def value(self, gameState, agent, depth, alpha, beta):
        if depth >= self.depth or gameState.isWin() or gameState.isLose():
            return [self.evaluationFunction(gameState)]
        else:
            actions = gameState.getLegalActions(agent)
            if not actions: actions = (gameState, None)
            nextAgent = (agent + 1) % gameState.getNumAgents()
            delta = nextAgent == 0

            if agent == 0:
                # Algorithm from question for max
                bestMove = [float("-inf"), None]
                for action in actions:
                    newState = gameState.generateSuccessor(agent, action)
                    moveValue = self.value(newState, nextAgent, depth, alpha, beta)+[action]
                    bestMove = max(bestMove, moveValue)
                    if (bestMove)[0] > beta:
                        return bestMove
                    alpha = max(alpha, bestMove[0])
            else:
                # Algorithm from question for min
                bestMove = [float("inf"), None]
                for action in actions:
                    newState = gameState.generateSuccessor(agent, action)
                    moveValue = self.value(newState, nextAgent, depth + delta, alpha, beta)+[action]
                    bestMove = min(bestMove, moveValue)
                    if bestMove[0] < alpha:
                        return bestMove
                    beta = min(beta, bestMove[0])
            return bestMove


class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
          Returns the expectimax action using self.depth and self.evaluationFunction

          All ghosts should be modeled as choosing uniformly at random from their
          legal moves.
        """
        "*** YOUR CODE HERE ***"
        return self.value(gameState, 0, 0)[-1]

    def value(self, gameState, agent, depth):
        if depth >= self.depth:
            return [self.evaluationFunction(gameState)]
        else:
            newStates = [(gameState.generateSuccessor(agent, action), action) for action in
                         gameState.getLegalActions(agent)]
            if not newStates:
                newStates = [(gameState, None)]
            nextAgent = (agent + 1) % gameState.getNumAgents()
            delta = nextAgent == 0

            if agent == 0:
                return max([self.value(state, nextAgent, depth) + [action] for state, action in newStates])
            else:  # The move that happens is the average move (we lose info about move here)
                moves = ([self.value(state, nextAgent, depth + delta) + [action] for state, action in newStates])
                average = sum([move[0] for move in moves])/float(len(moves))
                return [average]

def betterEvaluationFunction(currentGameState):
    """
      Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
      evaluation function (question 5).

      DESCRIPTION: Stay close to the ghosts and eat the capsules. While doing so maximize score
    """
    "*** YOUR CODE HERE ***"
    pacman = currentGameState.getPacmanPosition()

    newScaredTimes = [ghostState.scaredTimer for ghostState in currentGameState.getGhostStates()]
    ghostDist = [util.manhattanDistance(ghost, pacman) for ghost in currentGameState.getGhostPositions()]
    closeGhostDist = 1.0/(min(ghostDist) if min(ghostDist)!=0 else 1)

    score = currentGameState.getScore()

    numCapEaten = 1.0 / (len(currentGameState.getCapsules())+1)

    return closeGhostDist * 100 + score + numCapEaten * 1000


# Abbreviation
better = betterEvaluationFunction
