
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
	  void getLocInMap(int& i, int& j, int node_Id);
	  void mapMazeToGraph(graph &g);
      bool findPathRecursive(int i, int j, int desti, int destj);
	  void findPathNonRecursive(int start, int end, graph& g);

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

void maze::getLocInMap(int& i, int& j, int node_Id)
{
	for (int k = 0; k < map.rows(); k++)
	{
		for (int m = 0; m < map.cols(); m++)
		{
			if (map[k][m] == node_Id)
			{
				i = k;
				j = m;
			}
		}
	}
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
				n.setId(counter);
                g.addNode(n);
                setMap(i,j, counter);
                counter++;
            }
        }
    }

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
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

void maze::findPathNonRecursive(int start, int end, graph& g)
{
	stack<int> pathStack;
	bool solved = false;
	bool path_available = false;
	g.clearVisit();

	pathStack.push(start);
	g.visit(start);

	while (!pathStack.empty())
	{
		int v = pathStack.top();
		if (v == end)
		{
			solved = true;
			while (!directions.empty()) {
				//cout << directions.front();
				directions.erase(directions.begin());  //print directions
			}
			break;
		}
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				if (map[i][j] == v)
				{
					if (isLegal(i, j + 1) && map[i][j + 1] > -1)
					{
						if (!g.isVisited(map[i][j + 1]))
						{
							directions.push_back("right ");
							pathStack.push(map[i][j + 1]);
							g.visit(map[i][j + 1]);
							path_available = true;
						}
					}
					if (isLegal(i + 1, j) && map[i + 1][j] > -1)
					{
						if (!g.isVisited(map[i + 1][j]))
						{
							directions.push_back("down ");
							pathStack.push(map[i + 1][j]);
							g.visit(map[i + 1][j]);
							path_available = true;
						}
					}
					if (isLegal(i - 1, j) && map[i - 1][j] > -1)
					{
						if (!g.isVisited(map[i - 1][j]))
						{
							directions.push_back("up ");
							pathStack.push(map[i - 1][j]);
							g.visit(map[i - 1][j]);
							path_available = true;
						}
					}
					if (isLegal(i, j - 1) && map[i][j - 1] > -1)
					{
						if (!g.isVisited(map[i][j - 1]))
						{
							directions.push_back("left ");
							pathStack.push(map[i][j - 1]);
							g.visit(map[i][j - 1]);
							path_available = true;
						}
					}
					if (!path_available)
					{
						pathStack.pop();
						directions.pop_back();
					}
					path_available = false;
					break;
				}
			}
		}
	}
	if (solved)
		cout << "Solution found" << endl;
	else
		cout << "No solution" << endl;
}

bool maze::findPathRecursive(int i, int j, int desti, int destj) {
    visitedNodes[i][j] = true;
    if(i == desti && j == destj) {  //found bottom right corner
        while (!directions.empty()) {
			cout << directions.front();
			directions.erase(directions.begin());  //print directions
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
        directions.push_back("up");
        if(findPathRecursive(i - 1, j, desti, destj))
            return true;
		else if (!directions.empty())
			directions.pop_back();
    }
    if(isLegal(i,j-1) && !visitedNodes[i][j-1]) {
        directions.push_back("left");
        if(findPathRecursive(i, j-1, desti, destj))
            return true;
		else if(!directions.empty())
			directions.pop_back();
    }
    //no path could be found from this node to the destination
    return false;
}

static void outputPathDirections(vector<int> nodesVisited, maze m, int index){
    if(index < nodesVisited.size()){
        int lastRow, lastCol, currRow, currCol;

        for(int j = 0; j < m.getRows(); j++){
            for(int k = 0; k < m.getCols(); k++){
                if(nodesVisited.at(index) == m.getMap(j,k)){
                    lastRow = j;
                    lastCol = k;
                }
            }
        }

        int i = index + 1;
        for(int j = 0; j < m.getRows(); j++){
            for(int k = 0; k < m.getCols(); k++){
                if(nodesVisited.at(i) == m.getMap(j,k)){
                    currRow = j;
                    currCol = k;

                    if(currRow - lastRow == 0){
                        if(currCol - lastCol > 0) {
                            cout << "down ";
                            outputPathDirections(nodesVisited, m, i);
                            return;
                        }
                        else{
                            cout << "up ";
                            outputPathDirections(nodesVisited, m, i);
                            return;
                        }
                    }
                    else{
                        if(currRow - lastRow > 0 ){
                            cout << "right ";
                            outputPathDirections(nodesVisited, m, i);
                            return;
                        }
                        else{
                            cout << "left ";
                            outputPathDirections(nodesVisited, m, i);
                            return;
                        }
                    }
                }
            }
        }
    }
    return;
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
		vector<int> pathDij = g.shortestPathDijkstra(0,m.getMap(m.getRows()-1, m.getCols()-1));
		bool n = g.findShortestPathBFS(0, 31);
		int i = 31;
        int j = 0;
		int x, y;

        while(j < pathDij.size() && i != 0)
		{
			m.getLocInMap(x, y, i);
			//m.print(7, 9, x, y);
			i = g.getNode(i).getParent();
            m.getLocInMap(x,y,pathDij.at(j));
            m.print(m.getRows()-1,m.getCols()-1,x,y);
            j++;
		}
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
