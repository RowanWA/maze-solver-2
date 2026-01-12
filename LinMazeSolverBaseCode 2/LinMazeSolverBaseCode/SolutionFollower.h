#ifndef SOLUTION_H
#define SOLUTION_H

class SolutionFollower {
private:
  State state;

  int count = 0;

  void displayPath();

  void followLine();

  void checkIfJunction();

  void identifyJunction();

  void turnLeft();

  void turnRight();


  public:
    int totalLength;
    Decisions path[50];
    
    void loop();


};

#endif