#include "checkersgame.h"
#include <ctime>
#include <vector>

CheckersGame::CheckersGame()
{
	setGameType(RUSSIAN);
    srand(time(NULL));
	cleared = 0;
	created = 0;
	first = NULL;
	current = NULL;
}

CheckersGame::~CheckersGame() {
	endGame();
}

bool CheckersGame::setGameType(uint8 type) {
    if (type == RUSSIAN) {
		this->type = type;
		n = 8; k_start = 3;
		return true;
	}
	return false;
}

void CheckersGame::setMaxLevel(int level) {
    if (level >=3 && level <=7)
		maxlevel = level;
	else
		maxlevel = 3;
}

void CheckersGame::startNewGame(uint8 color) {
	computercolor = color;
    if (color == WHITE)
		humancolor = BLACK;
	else
		humancolor = WHITE;
	first = new CheckersState(n);
	created++;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k_start; j++) {
            if ( i % 2 == j % 2 )
				first->at(i,j) = WHITE;
		}
        for (int j = n - k_start; j < n; j++) {
            if ( i % 2 == j % 2)
				first->at(i,j) = BLACK;
		}
	}
    if (current)
		delete current;
	current = new CheckersState(first);
	historyStates.push_back(first);
	created++;
	click = 0;
	historynum = 0;
    // setting go_round direction
	ix[0] = -1; ix[1] = 1; ix[2] = -1; ix[3] = 1;
	jx[0] = 1; jx[1] = 1; jx[2] = -1; jx[3] = -1;
	gamerunning = true;
    if (computercolor == WHITE) {
        go();
	} else {
		emit stateChanged(current);
        genMoves(current,humancolor);
	}
}

void CheckersGame::endGame() {
	cleared = 0;
	created = 0;
	gamerunning = false;
    if (current) {
        clearTree(current, true, false);
        current = NULL;
    }

    first = NULL;
    history.clear();
    for (unsigned i = 0; i < historyStates.size(); i++)
        delete historyStates.at(i);
    historyStates.clear();
}

// generate possible moves and result states for current state and player
void CheckersGame::genMoves(CheckersState * state, uint8 color) {
	std::vector <point> vp;
    std::vector <CheckersState *> movesearch;
    bool capturefound = false;

    for (unsigned i = 0; i < n; i++) {
        for (unsigned j = 0; j < n; j++) {
            if (i%2 == j%2 && state->color(i,j) == color) {
                searchMove(state, i, j, vp, movesearch, capturefound);
                clearTree(state, false, true); // clear tree and leaving only root and leaves
			}
		}
	}
    // if exists move with killing, eliminate other moves
    if (capturefound) {
        for (unsigned i = 0; i < movesearch.size(); i++) {
            if ( movesearch.at(i)->move().size() == 2 ) {
                delete movesearch.at(i);
				movesearch.erase( movesearch.begin()+i );
				i--;
			} else {
				break;
			}
		}
	}
    // make each leave found a child of our root
    for (unsigned i = 0; i < movesearch.size(); i++) {
        for (auto it = movesearch.at(i)->move().begin() ; it != movesearch.at(i)->move().end(); it++ ) {
            if ( it->type == DELETED )
				movesearch.at(i)->at( it->x, it->y ) = 0;
		}
		movesearch.at(i)->setParent(state);
		state->childs().push_back( movesearch.at(i) );
	}
	if(!movesearch.empty())
		movesearch.clear();
}

int CheckersGame::movesCount(CheckersState * state, int i, int j) {
	uint8 color = state->color(i,j);
	int moves = 0;
    if (!color)
		return 0;
    int sidx, eidx, pidx = 1; // go-round number i, -//- j, max number of steps for checker
    if (color == WHITE) {
		sidx = 0; eidx = 1;
	}
    if (color == BLACK) {
		sidx = 2; eidx = 3;
	}
    if ( state->isKing(i,j) ) {
        sidx = 0; eidx = 3; pidx = n;
	}
    // check the possibility of move
    for (char k = sidx; k <= eidx; k++) {
        for (char pk = 1; pk <= pidx; pk++) {
			char xi = i + pk*ix[(int)k];
			char xj = j + pk*jx[(int)k];
            if ( !checkCoordinate(xi) || !checkCoordinate(xj) )
				break;
            if (state->isNull(xi,xj)) {
				moves ++;
			} else {
				break;
			}
		}
	}
	bool captureflag;
    for (int k = 0; k <= 3; k++) {
		captureflag = false;
        for (char pk = 1; pk <= pidx+1; pk++) {
			char xi = i + pk*ix[k];
			char xj = j + pk*jx[k];
            if ( !checkCoordinate(xi) || !checkCoordinate(xj) )
				break;
            if ( !captureflag && state->isNull(xi,xj) )
				continue;
            if ( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
				break;
            if ( !captureflag && state->color(xi, xj)!= color ) {
				captureflag = true;
				continue;
			}
            if (captureflag) {
                if ( !state->isNull(xi,xj) ) {
					captureflag = false;
					break;
				}
				moves++;
			}
		}
	}
	return moves;
}


int CheckersGame::searchMove(CheckersState *state, int i, int j, std::vector <point> & vp,
                             std::vector <CheckersState *> & movesearch, bool &capturefound) {
    std::vector < std::vector <point> > vpp; // array of simple moves
	std::vector <point> tmp_vp;
	point cp;
	int normmoves = 0, delmoves = 0;

	uint8 color = state->color(i,j);
	bool isking = state->isKing(i,j);
    if (!color)
		return false;

    int sidx, eidx, pidx = 1; // go_round No i, -//- j, max steps in depth for checker
    if (color == WHITE) {
		sidx = 0; eidx = 1;
	}
    if (color == BLACK) {
		sidx = 2; eidx = 3;
	}
    if ( state->isKing(i,j) ) {
        sidx = 0; eidx = 3; pidx = n;
	}
    // check possibility of move
    for (char k = sidx; k <= eidx && !capturefound; k++) {
        for (char pk = 1; pk <= pidx; pk++) {
			char xi = i + pk*ix[(int)k];
			char xj = j + pk*jx[(int)k];
            if ( !checkCoordinate(xi) || !checkCoordinate(xj) )
				break;
            if (state->isNull(xi,xj)) {
                // creating new state
				tmp_vp.clear();
				tmp_vp.push_back(point(i,j,MOVEDFROM));
                if ( (color == BLACK && xj == 0 && !isking) || (color == WHITE && xj == n-1 && !isking)  ) {
					tmp_vp.push_back(point(xi,xj,TOKING));
				} else {
					tmp_vp.push_back(point(xi,xj,MOVEDTO));
				}
				vpp.push_back(tmp_vp);
				normmoves ++;
			} else {
				break;
			}
		}
	}
	bool captureflag;
    // check killing possibility
    for (int k = 0; k <= 3; k++) {
		captureflag = false;
        for (char pk = 1; pk <= pidx+1; pk++) {
			char xi = i + pk*ix[k];
			char xj = j + pk*jx[k];
            if ( !checkCoordinate(xi) || !checkCoordinate(xj) )
				break;
            if ( !captureflag && state->isNull(xi,xj) )
				continue;
            // meet the same color on the way, impossible to go round
            if ( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
				break;
            // no killed checkers on the way and found a checker of opposite color
            if ( !captureflag && state->color(xi, xj)!= color ) {
				captureflag = true;
				cp.x = xi; cp.y = xj; cp.type = MARKDELETED;
				continue;
			}
            // if found a ckecker we can possibly kill
            if (captureflag) {
                    // if the next cell isn't empty, we can't kill
                if ( !state->isNull(xi,xj) ) {
					captureflag = false;
					break;
				}

                // building new state, searching for possible kill from this state
                capturefound = true;
				delmoves++;
                // prepear array of moves
				tmp_vp.clear();
				tmp_vp.push_back(point(i,j,MOVEDFROM));
				tmp_vp.push_back(cp);
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==n-1 && !isking)  ) // check if becomes king
					tmp_vp.push_back(point(xi,xj,TOKING));
				else
                    tmp_vp.push_back(point(xi,xj,MOVEDTO));

                CheckersState *tmpstate = state->genNextState(tmp_vp);		// genereting new state from array of moves
				created++;

                // prepearing moves' history array
				std::vector <point> history_vector = vp;
                if ( vp.size() ) {
					(history_vector.end()-1)->type = MOVEDTHROUGH;
					(history_vector.end()-2)->type = DELETED;
					tmpstate->deletedAtMove() = state->deletedAtMove()+1;
				} else {
					history_vector.push_back( tmp_vp.at(0) );
					tmpstate->deletedAtMove() = 1;
				}
				cp.type = DELETED;
				history_vector.push_back( cp );
				history_vector.push_back( tmp_vp.at(2) );
				//
				tmpstate->move() = history_vector;
                tmpstate->setParent(state);
				state->childs().push_back(tmpstate);
                if (! searchMove(tmpstate, xi, xj, history_vector, movesearch, capturefound ))
					movesearch.push_back(tmpstate);
			}
		}
	}

    //if there are states, from which we can kill, eliminate all simple moves as not corresponding rules
    if (capturefound) {
		normmoves = 0;
	} else {
        for (unsigned i = 0; i < vpp.size(); i++) {
			CheckersState * tmpstate = state->genNextState(vpp.at(i));
			created++;
			tmpstate->setParent(state);
			state->childs().push_back(tmpstate);
			movesearch.push_back(tmpstate);
		}
	}
	return normmoves + delmoves;
}

void CheckersGame::setScore(CheckersState* state)
{
    state->score() = -1 * goRecursive(state,humancolor,2,-9999,9999);
}

// make best computer's move from current state
void CheckersGame::go() {
    genMoves(current, computercolor);
    int xmax = -9999;
	int id = 0;
	std::vector <CheckersState *> tmp;

    /* for (unsigned i = 0; i < current->childs().size(); i++) {
        current->childs().at(i)->score() = -goRecursive(current->childs().at(i), humancolor, 2, -9999, +9999);
    } */


    QtConcurrent::blockingMap(current->childs(), [this] (CheckersState* state) { setScore(state); });


    for (unsigned i = 0; i < current->childs().size(); i++) {
        if ( current->childs().at(i)->score() > xmax ) {
                xmax = current->childs().at(i)->score();
        }
    }
    for (unsigned i = 0; i < current->childs().size(); i++) {
        if ( current->childs().at(i)->score() == xmax ) {
                tmp.push_back( current->childs().at(i) );
        }
    }
	id = rand() % tmp.size();
	move( tmp.at(id)->move().front(), tmp.at(id)->move().back() );
    genMoves(current, humancolor);
}

// minimax algorithm with alpha-beta pruning
int CheckersGame::goRecursive(CheckersState *state, uint8 color, int level, int alpha, int beta) { // THREAD

    if (level == maxlevel || checkTerminatePosition(state)) {
        state->score() = evaluation(state, color);
		return state->score();
	}
    genMoves(state, color);
	unsigned i;
    for (i = 0; i < state->childs().size(); i++) {
        uint8 oppositeColor = (color == WHITE ? BLACK : WHITE);
        alpha = std::max( alpha, - goRecursive( state->childs().at(i), oppositeColor, level + 1 , -beta, -alpha ) );
		if ( beta < alpha ) {
			break;
		}
	}

	return alpha;
}

bool CheckersGame::checkCoordinate(char x) {
    if (x >= 0 && x < n)
		return true;
	return false;
}

/*     0              1             2               3              4             5
white_checkers | white_kings | white_moves | black_checkers | black_kings | black_moves
*/
void CheckersGame::calcCounts(CheckersState * state) {
	std::vector <CheckersState *> tmp;
	state->counts().clear();
    state->counts().resize(6,0);

	int movescount;
    for (unsigned i = 0; i < n; i++) {
        for (unsigned j = 0; j < n; j++) {
            if (i%2 != j%2)
				continue;
			movescount = movesCount(state, i, j);
            if (state->at(i,j) == WHITE) {
				state->counts()[0]++;
				state->counts()[2] += movescount;
			}
            if (state->at(i,j) == WHITEKING) {
				state->counts()[1]++;
				state->counts()[2] += movescount;
			}
            if (state->at(i,j) == BLACK) {
                state->counts()[3]++;
                state->counts()[5] += movescount;
			}
            if (state->at(i,j) == BLACKKING) {
                state->counts()[4]++;
                state->counts()[5] += movescount;
			}
		}
	}
    if (tmp.size()) {
		state->childs() = tmp;
	}
}

//  rating procedure
int CheckersGame::evaluation(CheckersState * state, uint8 color) {
    if (state->counts().empty())
		calcCounts(state);
	int evaluation = 0;
    evaluation += 4 * ( state->counts()[0] - state->counts()[3] );
    evaluation += 6 * ( state->counts()[1] - state->counts()[4] );
    evaluation += ( state->counts()[2] - state->counts()[5] );
    if(color == WHITE) return evaluation;
    return -1 * evaluation;
}

bool CheckersGame::checkTerminatePosition(CheckersState * state) {
    if (!state->counts().size())
		calcCounts(state);
    // all checkers of some side are killed
	if ( !(state->counts()[0]+state->counts()[1]) ||
         !(state->counts()[3]+state->counts()[4]) )
		return true;
    // no moves possible
    if ( !state->counts()[2] || !state->counts()[5] )
		return true;
	return false;
}

uint8 CheckersGame::whoWin(CheckersState *state) {
	if ( !(state->counts()[0]+state->counts()[1]) || !state->counts()[2] )
        return BLACK;	// black win
    if ( !(state->counts()[3]+state->counts()[4]) || !state->counts()[5] )
        return WHITE;	// white win
	return -1;
}


void CheckersGame::clearTree(CheckersState * state, bool clearlists, bool onlychilds) {
    if (onlychilds) {		// delete only children, leaving the root
        for (unsigned i = 0; i < state->childs().size(); i++) {
            clearTreeRecursive( state->childs().at(i), clearlists ); // delete all children
        }
        if(! state->childs().empty())
            state->childs().clear();
	} else {
        clearTreeRecursive( state, clearlists ); // delete all
	}
}

void CheckersGame::clearTreeRecursive(CheckersState * state, bool clearlists) {
    // if current node has no children => it's leaf
    if ( state->childs().empty() == 0 && clearlists ) {
		cleared ++;
		delete state;
	} else {
        for (unsigned i =0; i < state->childs().size(); i++) {
			clearTreeRecursive( state->childs().at(i), clearlists );
		}
        if(! state->childs().empty())
            state->childs().clear();
        if (clearlists) {
			cleared ++;
			delete state;
		}
	}
}

void CheckersGame::setClicked(int i, int j) {
    if (i >= 0 && i < n && j >= 0 && j < n && i%2 == j%2 && gamerunning) {
        if (click == 0)
			firstClick(i, j);
		else
			secondClick(i, j);
	} else {
		emit vectorDeleted();
		click = 0;
	}
}

void CheckersGame::firstClick(int i, int j) {
    if ( (humancolor == current->color(i,j)) ) {
		tmppoint.x = i; tmppoint.y = j; tmppoint.type = MOVEDFROM;
		tmpvector.clear();

        for (unsigned ii = 0; ii < current->childs().size(); ii++ ) {
            if ( current->childs().at(ii)->move().at(0) == tmppoint ) {
                for ( unsigned jj = 0; jj < current->childs().at(ii)->move().size(); jj++ ) {
					tmpvector.push_back( current->childs().at(ii)->move().at(jj) );
				}
			}
		}
        if (!tmpvector.empty()) {
			click = 1;
			emit vectorChanged(tmpvector);
			return;
		}
	}
	emit vectorDeleted();
}

void CheckersGame::secondClick(int i, int j) {
	bool move = false;
	if ( (tmppoint.x == i && tmppoint.y == j) ) {
		return;
	}
    if ( current->isNull(i,j) )
		move = this->move( tmppoint , point(i,j,MOVEDTO) );
    if ( !move ) {
		click =0;
		firstClick(i, j);
	} else {
        // succesfull move
        if (gamerunning)
            go(); // make computer's move
		click = 0;
	}
}

bool CheckersGame::move(point p1, point p2) {

    for (unsigned i = 0; i < current->childs().size(); i++ ) {
        if ( current->childs().at(i)->move().front() == p1 &&
			current->childs().at(i)->move().back().x == p2.x &&
			current->childs().at(i)->move().back().y == p2.y )
		{
            // pass to another state
			CheckersState * tmpstate = current->childs().at(i);
			current->childs().erase(current->childs().begin()+i);
			clearTree(current,true);
			clearTree(tmpstate,true,true);
			current = tmpstate;
            history.push_back( current->move() );	// save history
            CheckersState* tmp = new CheckersState(current);
            if(!tmp->childs().empty())
                tmp->childs().clear();
			historyStates.push_back( tmp );
			emit stateChanged(current);
			historynum = historyStates.size()-1;
            if (checkTerminatePosition(current)) {
				emit gameEnded( whoWin(current) );
				gamerunning = false;
			}
			return true;
		}
	}
	return false;
}

void CheckersGame::historyShowFirst() {
	historynum = 0;
	historyShow();
}

void CheckersGame::historyShowLast() {
    historynum = historyStates.size() - 1;
	historyShow();
}

void CheckersGame::historyShowPrev() {
    historynum--;
	historyShow();
}

void CheckersGame::historyShowNext() {
    historynum++;
	historyShow();
}

void CheckersGame::historyShow() {
	gamerunning = false;
    if (historynum < 0)
        historynum = 0;
    if (historynum > (int)historyStates.size()-1)
		historynum = historyStates.size()-1;
    if ( historynum == (int)historyStates.size()-1 ) {
		gamerunning = true;
		click = 0;
		emit stateChanged( current );
		return;
	}
	emit stateChanged( historyStates.at(historynum) );
}
