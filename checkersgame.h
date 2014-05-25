#ifndef CHECKERSGAME_H
#define CHECKERSGAME_H

#include <QtCore>
#include <QtConcurrent>
#include <QObject>
#include <QVector>
#include <vector>
#include "checkersstate.h"

// game rules
enum GameType{
	RUSSIAN = 1,
};

class CheckersGame : public QObject
{
	Q_OBJECT

public:
    CheckersGame();
	~CheckersGame();
    bool setGameType(uint8 type);
	void setMaxLevel(int level);
    void startNewGame(uint8 color);
	void endGame();

public slots:
	void setClicked(int i, int j);
	void historyShowFirst();
	void historyShowLast();
	void historyShowPrev();
	void historyShowNext();

signals:
    void stateChanged(CheckersState *);
	void vectorChanged(std::vector <point> & v);
	void vectorDeleted();
	void gameEnded(uint8);

private:
    void firstClick(int i, int j);
    void secondClick(int i, int j);

// AI methods:
    void go();	// make computer's move from current state
    int goRecursive(CheckersState * state, uint8 color, int level, int alpha, int beta); // minimax algorithm with alpha-beta pruning
    void setScore(CheckersState*); // find rating of the state
    void calcCounts(CheckersState * state);             // calculate checkers of diffrent types and possible moves
    int evaluation(CheckersState * state, uint8 color);	// rating of currrent position for player 'colot'
    void genMoves(CheckersState * state, uint8 color);        // build tree of possible states and moves from one state to another
    bool checkTerminatePosition(CheckersState * state);	// is this position terminal
    int movesCount(CheckersState * state, int i, int j);// number of moves from cell [i][j] in current state
    int searchMove(CheckersState * state, int i, int j, std::vector <point> & vp,
                   std::vector <CheckersState *> & movesearch, bool& capturefound); // possible moves for chcker [i][j]
    bool checkCoordinate(char x);                       // checks if coordinate is inside the field
	void clearTree(CheckersState * state, bool clearlists = true, bool onlychilds = false);
	void clearTreeRecursive(CheckersState * state, bool clearlists = false);
    bool move(point p1, point p2);						// makes move from p1 to p2 if possible, if impossible - returns false
	uint8 whoWin(CheckersState * state);

    // history
    std::vector <std::vector <point> > history;		// hystory of moves, starting from first
    std::vector <CheckersState *> historyStates;    // history of states
	int historynum;
	void historyShow();

    // temporary
    std::vector < point > tmpvector;
	point tmppoint;

    CheckersState * first;			// first state
    CheckersState * current;		// current state
    uint8 n;						// field size is n*n
    uint8 k_start;                  // number of rows of checkers on start
    uint8 type;						// game type
    uint8 computercolor;
    uint8 humancolor;

    uint8 click;					// number of click
    char ix[4];						// directions of round:
    char jx[4];						// used when searching for possible moves

    // flags
    bool gamerunning;
    int maxlevel;				    // max depth of search tree (level of root == 0)

    // counters (debug)
	int cleared;
	int created;
};

#endif // CHECKERSGAME_H
