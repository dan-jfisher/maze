
#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <fstream>
#include <cstdlib>
#include "d_matrix.h"
#include "graph.h"
#include <stack>

using namespace std;

class maze
{
   public:
	  maze(ifstream &fin);
	  void print(int,int,int,int);
	  bool isLegal(int i, int j);
      int getRows(){return this->rows;}
      int getCols(){return this->cols;}

	  void setMap(int i, int j, int n);
	  int getMap(int i, int j) const;
	  void mapMazeToGraph(graph &g);
      bool findPathRecursive(int i, int j, int desti, int destj);
      bool findPathNonRecursive(graph &g, int start, int goal);

   private:
	  int rows; // number of rows in the maze
	  int cols; // number of columns in the maze

	  matrix<bool> value;
	  vector<string> directions;
      matrix<bool> visitedNodes;
	  matrix<int> map;      // Mapping from maze (i,j) values to node index values
};

void maze::setMap(int i, int j, int n)
// Set mapping from maze cell (i,j) to graph node n.
{
    map[i][j] = n;
}

int maze ::getMap(int i, int j) const
// Return mapping of maze cell (i,j) in the graph.
{
    return map[i][j];
}

maze::maze(ifstream &fin)
// Initializes a maze by reading values from fin.  Assumes that the
// number of rows and columns indicated in the file are correct.
{
   fin >> rows;
   fin >> cols;

   char x;

   value.resize(rows,cols);
   visitedNodes.resize(rows,cols);
   for (int i = 0; i <= rows-1; i++)
	  for (int j = 0; j <= cols-1; j++)
	  {
          fin >> x;
          if (x == 'O')
              value[i][j] = true;
          else
              value[i][j] = false;

          visitedNodes[i][j] = false;
	  }

   map.resize(rows,cols);
}

void maze::print(int goalI, int goalJ, int currI, int currJ)
// Print out a maze, with the goal and current cells marked on the
// board.
{
   cout << endl;

   if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
	  throw rangeError("Bad value in maze::print");

   if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
	  throw rangeError("Bad value in maze::print");

   for (int i = 0; i <= rows-1; i++)
   {
	  for (int j = 0; j <= cols-1; j++)
	  {
	 if (i == goalI && j == goalJ)
		cout << "*";
	 else
		if (i == currI && j == currJ)
		   cout << "+";
		else
		   if (value[i][j])
		  cout << " ";
		   else
		  cout << "X";
	  }
	  cout << endl;
   }
   cout << endl;
}

bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze.
{
   if (i < 0 || i >= rows || j < 0 || j >= cols) {
       //throw rangeError("Bad value in maze::isLegal");
       return false;
   }
   return value[i][j];
}

void maze::mapMazeToGraph(graph &g)
// Create a graph g that represents the legal moves in the maze m.
{
    int counter = 0;
    for(int i = 0; i < rows; i++){     //map all nodes
        for(int j = 0; j < cols; j++){
            if(isLegal(i,j)){
                node n;
                g.addNode(n);
                setMap(i,j, counter);
                counter++;
            }
        }
    }

    for(int i = 0; i < rows-1; i++)
    {
        for(int j = 0; j < cols-1; j++)
        {
            if(isLegal(i,j))
            {
                int a = getMap(i,j);
                int b = 0;
                if(isLegal(i+1,j))
                {
                    b = getMap(i+1,j);
                    g.addEdge(a,b,1);
                }
                if(isLegal(i,j+1))
                {
                    b = getMap(i,j+1);
                    g.addEdge(a,b,1);
                }
            }
        }
    }
}

bool maze::findPathRecursive(int i, int j, int desti, int destj) {
    visitedNodes[i][j] = true;
    if(i == desti && j == destj) {  //found bottom right corner
        int cRow = 0;
        int cCol = 0;
        while (!directions.empty()) {
			cout << directions.front();
            /*if(directions.front() == "down "){
                cCol++;
            }
            if(directions.front() == "right "){
                cRow++;
            }
            if(directions.front() == "up "){
                cCol--;
            }
            if(directions.front() == "left "){
                cRow--;
            }*/
			directions.erase(directions.begin());  //print directions
            //this->print(desti, destj, cRow, cCol);
            //cout << endl;
        }
		cout << "Found" << endl;

        for(int i = 0; i < rows; i++){      //clear visitedNodes matrix
            for(int j = 0; j < cols; j++){
                visitedNodes[i][j] = false;
            }
        }

        return true;
    }
    //check all four possible directions
    if(isLegal(i+1,j) && !visitedNodes[i+1][j]) {
        directions.push_back("down ");
        if(findPathRecursive(i + 1, j, desti, destj))
            return true;
		else if (!directions.empty())
			directions.pop_back();
    }
    if(isLegal(i,j+1) && !visitedNodes[i][j+1]) {
        directions.push_back("right ");
        if(findPathRecursive(i, j+1, desti, destj))
            return true;
		else if (!directions.empty())
			directions.pop_back();
    }
    if(isLegal(i-1,j) && !visitedNodes[i-1][j]) {
        directions.push_back("up ");
        if(findPathRecursive(i - 1, j, desti, destj))
            return true;
		else if (!directions.empty())
			directions.pop_back();
    }
    if(isLegal(i,j-1) && !visitedNodes[i][j-1]) {
        directions.push_back("left ");
        if(findPathRecursive(i, j-1, desti, destj))
            return true;
		else if(!directions.empty())
			directions.pop_back();
    }
    //no path could be found from this node to the destination
    return false;
}

bool maze::findPathNonRecursive(graph &g, int start, int goal)
{
    g.clearVisit();
    stack<int> s;
    bool found = false;

    s.push(start);
    g.visit(start);
    bool adjacent = false;

    while(!s.empty() && !found)
    {
        int a = s.top();
        if (a == goal)
            found = true;
        for(int i = 0; i < rows; i++) // for 1
        {
            for(int j = 0; j < cols; j++) // for 2
            {
                if(getMap(i,j) == a)
                {
                    print(rows-1,cols-1,i,j);
                    //go right
                    if(j!= cols-1 && getMap(i,j+1) > -1)
                    {
                        if(!g.isVisited(getMap(i,j+1)))
                        {
                            //cout << " right";
                            g.visit(getMap(i,j+1));
                            s.push(getMap(i,j+1));
                            adjacent = true;
                        }
                    }
                    if(i != rows-1 && getMap(i+1,j) > -1)
                    {
                        //go down
                        if(!g.isVisited(getMap(i+1,j)))
                        {
                            //cout << " down";
                            g.visit(getMap(i+1,j));
                            s.push(getMap(i+1,j));
                            adjacent = true;
                        }
                    }
                    if(i != 0 && getMap(i-1,j) > -1)
                    {
                        //go up
                        if(!g.isVisited(getMap(i-1,j)))
                        {
                            //cout << " up";
                            g.visit(getMap(i-1,j));
                            s.push(getMap(i-1,j));
                            adjacent = true;
                        }
                    }
                    if(j != 0 && getMap(i,j-1) > -1)
                    {
                        //go left
                        if(!g.isVisited(getMap(i,j-1)))
                        {
                            //cout << " left";
                            g.visit(getMap(i,j-1));
                            s.push(getMap(i,j-1));
                            adjacent = true;
                        }
                    }
                    if(!adjacent)
                        s.pop();
                        adjacent = false;
                    break;
                }
            } //end for 2
        } //end for 1
    }// end while

    if(found) {
        return true;
    }
    else
        return false;
}



int main()
{
    char x;
    ifstream fin;

    // Read the maze from the file.
    string fileName = "Maze 1.txt";

    fin.open(fileName.c_str());
    if (!fin)
    {
	   cerr << "Cannot open " << fileName << endl;
	   exit(1);
    }

    try
    {

	    graph g;
	    maze m(fin);
        m.mapMazeToGraph(g);
        m.findPathRecursive(0,0,m.getRows()-1, m.getCols()-1);
        m.findPathNonRecursive(g, m.getMap(0,0), m.getMap(m.getRows()-1, m.getCols()-1));
    }
    catch (indexRangeError &ex)
    {
 	   cout << ex.what() << endl; exit(1);
    }
    catch (rangeError &ex)
    {
	   cout << ex.what() << endl; exit(1);
    }

    fin.close();
    cout << endl;


    fileName = "Maze 2.txt";

    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }

    try
    {

        graph g;
        maze m(fin);
        m.mapMazeToGraph(g);
        m.findPathRecursive(0,0,m.getRows()-1, m.getCols()-1);
        m.findPathNonRecursive(g, m.getMap(0,0), m.getMap(m.getRows()-1, m.getCols()-1));
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }

    fin.close();
    cout << endl;

    fileName = "Maze 3.txt";

    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }

    try
    {

        graph g;
        maze m(fin);
        m.mapMazeToGraph(g);
        m.findPathRecursive(0,0,m.getRows()-1, m.getCols()-1);
        m.findPathNonRecursive(g, m.getMap(0,0), m.getMap(m.getRows()-1, m.getCols()-1));
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    system("pause");
}
