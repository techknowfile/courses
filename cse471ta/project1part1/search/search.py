# search.py
# ---------
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


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print "Start:", problem.getStartState()
    print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    print "Start's successors:", problem.getSuccessors(problem.getStartState())
    """
    "*** YOUR CODE HERE ***"
    from util import Stack
    
    # Declare lists and Stacks
    visitedList = []
    successors = []
    finalPath = []
    tempPath = []
    pathStack = Stack()
    fringeStack = Stack()
    
    # Get Start state and initialize stack with Start node
    currentNode = (problem.getStartState(), "START", 0)
    currentPosition = currentNode[0]
    
    while not problem.isGoalState(currentPosition): 
        
        # Main sequence to process a new node
        # ********************************************************************
        if currentPosition not in visitedList:
            # Add current node to the list of already visited nodes
            # (so that loops are accounted for)
            visitedList.append(currentPosition)
            
            # Get successors 
            # ****************************************************************            
            successors = problem.getSuccessors(currentPosition)
            """
            # Validate Successors
            validSuccessors = []
            for node in successors:
                successorPosition = node[0]
                # Checks the path to currentPosition
                if successorPosition not in visitedList: 
                    validSuccessors.append(node)
            """       
            # process each node in the returned successors list
            for node in successors:
                fringeStack.push(node)
                tempPath = finalPath + [node[1]]
                pathStack.push(tempPath)
                
        # Determine next node values
        currentNode = fringeStack.pop()
        currentPosition = currentNode[0] 
        
        # Update finalPath to show the path to currentPosition
        finalPath = pathStack.pop()
    
    # Return directions to get from START to GOAL
    return finalPath


    

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    from util import Queue
    
    # Declare lists and Queues
    visitedList = []
    successors = []
    finalPath = []
    tempPath = []
    pathQueue = Queue()
    fringeQueue = Queue()
    
    # Get Start state and initialize stack with Start node
    currentNode = (problem.getStartState(), "START", 0)
    currentPosition = currentNode[0]
    
    while not problem.isGoalState(currentPosition): 
        
        # Main sequence to process a new node
        # ********************************************************************
        if currentPosition not in visitedList:
            # Add current node to the list of already visited nodes
            # (so that loops are accounted for)
            visitedList.append(currentPosition)
            
            # Get successors 
            # ****************************************************************            
            successors = problem.getSuccessors(currentPosition)
            
            # Validate Successors
            validSuccessors = []
            for node in successors:
                successorPosition = node[0]
                # Checks the path to currentPosition
                if successorPosition not in visitedList: 
                    validSuccessors.append(node)
            # process each node in the returned successors list
            for node in validSuccessors:
                fringeQueue.push(node)
                tempPath = finalPath + [node[1]]
                pathQueue.push(tempPath)
                
        # Determine next node values
        currentNode = fringeQueue.pop()
        currentPosition = currentNode[0] 
        
        # Update finalPath to show the path to currentPosition
        finalPath = pathQueue.pop()
    
    # Return directions to get from START to GOAL
    return finalPath

def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"
    from util import PriorityQueue
    
    
    # Declare lists and Queues
    visitedList = []
    successors = []
    finalPath = []
    tempPath = []
    pathPQueue = PriorityQueue()
    fringePQueue = PriorityQueue()
    
    
    # Get Start state and initialize stack with Start node
    currentNode = (problem.getStartState(), "START", 0)
    currentPosition = currentNode[0]
    
    
    
    
    while not problem.isGoalState(currentPosition): 
        
        # Main sequence to process a new node
        # ********************************************************************
        if currentPosition not in visitedList:
            # Add current node to the list of already visited nodes
            # (so that loops are accounted for)
            visitedList.append(currentPosition)
            
            # Get successors 
            # ****************************************************************            
            successors = problem.getSuccessors(currentPosition)
            
            # Validate Successors
            validSuccessors = []
            for node in successors:
                successorPosition = node[0]
                # Checks the path to currentPosition
                if successorPosition not in visitedList: 
                    validSuccessors.append(node)
                    
            # process each node in the returned successors list
            for node in validSuccessors:
                tempPath = finalPath + [node[1]]
                costOfPath = problem.getCostOfActions(tempPath)
                pathPQueue.push(tempPath,costOfPath)
                fringePQueue.push(node, costOfPath)
                
                
        # Determine next node values
        currentNode = fringePQueue.pop()
        currentPosition = currentNode[0] 
        
        # Update finalPath to show the path to currentPosition
        finalPath = pathPQueue.pop()
    
    # Return directions to get from START to GOAL
    return finalPath

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    from util import PriorityQueue
    
    
    # Declare lists and Queues
    visitedList = []
    successors = []
    finalPath = []
    tempPath = []
    pathPQueue = PriorityQueue()
    fringePQueue = PriorityQueue()
    
    
    # Get Start state and initialize stack with Start node
    currentNode = (problem.getStartState(), "START", 0)
    currentPosition = currentNode[0]
    
    
    
    
    while not problem.isGoalState(currentPosition): 
        
        # Main sequence to process a new node
        # ********************************************************************
        if currentPosition not in visitedList:
            # Add current node to the list of already visited nodes
            # (so that loops are accounted for)
            visitedList.append(currentPosition)
            
            # Get successors 
            # ****************************************************************            
            successors = problem.getSuccessors(currentPosition)
            
            # Validate Successors
            validSuccessors = []
            for node in successors:
                successorPosition = node[0]
                # Checks the path to currentPosition
                if successorPosition not in visitedList: 
                    validSuccessors.append(node)
                    
            # process each node in the returned successors list
            for node in validSuccessors:
                tempPath = finalPath + [node[1]]
                costOfPath = problem.getCostOfActions(tempPath)
                # Determine the value of f(n) which includes the heuristic
                costWithHeur = costOfPath + heuristic(node[0],problem)
                pathPQueue.push(tempPath,costWithHeur)
                fringePQueue.push(node, costWithHeur)
                
                
        # Determine next node values
        currentNode = fringePQueue.pop()
        currentPosition = currentNode[0] 
        
        # Update finalPath to show the path to currentPosition
        finalPath = pathPQueue.pop()
    
    # Return directions to get from START to GOAL
    return finalPath


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
