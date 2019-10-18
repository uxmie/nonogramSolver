#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

enum gridState_t {
    EMPTY,
    FILL,
    UNKNOWN
};

class nonogramGame {
    public:
    nonogramGame(int h, int w) : height(h), width(w) {
        state = vector<vector<gridState_t>>(height, vector<gridState_t>(width, UNKNOWN));
        rowStrategies = vector<vector<long long>>(height);
        columnStrategies = vector<vector<long long>>(width);
    }
    void solve();
    void printState();

    vector<vector<int>> rowConstraints;
    vector<vector<int>> columnConstraints;

    private:
    void generateStrategies();
    void generateOneStrategy(vector<long long> &line, const vector<int> &constraint, const int size);
    bool generateRecursive(const int size, const vector<int> &sum, vector<long long> &stratList, long long &output, int start, int level);

    std::pair<bool, long long> eliminateStrategies();
    bool isValid(const vector<gridState_t> &states, long long strategy);

    bool isSolved();

    bool solveRecursive();

    int height, width;

    vector<vector<gridState_t>> state;
    vector<vector<long long>> rowStrategies;
    vector<vector<long long>> columnStrategies;
    vector<bool> rowSolved;
    vector<bool> columnSolved;
};

bool nonogramGame::isSolved() {
    for(int i = 0; i < height; ++i) {
        if(rowStrategies[i].size() > 1) {
            return false;
        }
    }
    return true;
}

void nonogramGame::printState() {
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            switch(state[i][j]) {
                case EMPTY:
                cout << "。";
                break;
                case FILL:
                cout << "鬱";
                break;
                case UNKNOWN:
                cout << "？";
                break;
            }
        }
        cout << endl;
    }
}

void nonogramGame::solve() {
    cout << "Generating strategies..." << endl;
    generateStrategies();
    cout << "Pruning strategies..." << endl;
    pair<bool, long long> pruneResults = eliminateStrategies();
    if(!pruneResults.first) {
        cout << "The puzzle is impossible." << endl;
        return;
    }

    if(isSolved()) {
        printState();
    } else {
        cout << "Pruned: " << pruneResults.second << " strategies." << endl;
        solveRecursive();
    }
}

bool nonogramGame::solveRecursive() {
    //Pick line with fewest strategies
    bool vertical = false;
    long long unsigned int strategies = UINT64_MAX;
    int useLine = -1;
    for(int i = 0; i < height; ++i) {
        if(!rowSolved[i] && rowStrategies[i].size() < strategies && rowStrategies[i].size() > 1) {
            strategies = rowStrategies[i].size();
            useLine = i;
        }
    }
    for(int i = 0; i < width; ++i) {
        if(!columnSolved[i] && columnStrategies[i].size() < strategies && columnStrategies[i].size() > 1) {
            strategies = columnStrategies[i].size();
            useLine = i;
            vertical = true;
        }
    }
    //Copy state
    vector<vector<gridState_t>> origState(height, vector<gridState_t>(width));
    vector<vector<long long>> origRowStrats(height);
    vector<vector<long long>> origColStrats(width);
    vector<bool> origRowSolved(height);
    vector<bool> origColSolved(width);
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            origState[i][j] = state[i][j];
        }
    }
    for(int i = 0; i < height; ++i) {
        origRowStrats[i].reserve(rowStrategies[i].size());
        for(long long j : rowStrategies[i]) {
            origRowStrats[i].push_back(j);
        }
        origRowSolved[i] = rowSolved[i];
    }
    for(int i = 0; i < width; ++i) {
        origColStrats[i].reserve(columnStrategies[i].size());
        for(long long j : columnStrategies[i]) {
            origColStrats[i].push_back(j);
        }
        origColSolved[i] = columnSolved[i];
    }

    auto restore = [&]() {
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j) {
                state[i][j] = origState[i][j];
            }
        }
        for(int i = 0; i < height; ++i) {
            rowStrategies[i].clear();
            rowStrategies[i].reserve(origRowStrats[i].size());
            for(long long j : origRowStrats[i]) {
                rowStrategies[i].push_back(j);
            }
            rowSolved[i] = origRowSolved[i];
        }
        for(int i = 0; i < width; ++i) {
            columnStrategies[i].clear();
            columnStrategies[i].reserve(origColStrats[i].size());
            for(long long j : origColStrats[i]) {
                columnStrategies[i].push_back(j);
            }
            columnSolved[i] = origColSolved[i];
        }
    };

    if(!vertical) {
        int count = 0;
        //Test numbers
        vector<pair<long long, int>> eliminated;  
        cout << "Using row " << useLine + 1 << endl;
        for(long long strat : origRowStrats[useLine]) {
            cout << "Testing prospect of strategy " << count+1 << "/" << origRowStrats[useLine].size() << endl;
            rowStrategies[useLine].clear();
            rowStrategies[useLine].push_back(strat);
            rowSolved[useLine] = true;
            for(int i = 0; i < width; ++i) {
                state[useLine][i] = (strat & (1LLU << i)) != 0? FILL: EMPTY;
            }
            pair<bool, long long> eliminateResult = eliminateStrategies();
            if(eliminateResult.first) {
                if(isSolved()) {
                    printState();
                    return true;
                } else {
                    cout << "Strategy " << count << "/" << origRowStrats[useLine].size() << " prunes " << eliminateResult.second << " others." << endl;
                    eliminated.push_back({eliminateResult.second, count});
                }
            } else {
                cout << "This strategy cannot work" << endl;
            }
            restore();
            count++;
        }
        sort(eliminated.begin(), eliminated.end(), [&](const pair<long long, int> &a, const pair<long long, int> &b){
            return a.first > b.first;
        });
        
        count = 0;
        for(auto &useStrats : eliminated){
            long long strat = origRowStrats[useLine][useStrats.second];
            cout << "Using row " << useLine + 1 << " Strategy: " << count+1 << "/" << eliminated.size() << ": " << strat << endl;
            rowStrategies[useLine].clear();
            rowStrategies[useLine].push_back(strat);
            rowSolved[useLine] = true;
            for(int i = 0; i < width; ++i) {
                state[useLine][i] = (strat & (1LLU << i)) != 0? FILL: EMPTY;
            }
            eliminateStrategies();
            if(solveRecursive()) {
                return true;
            }
            cout << "No further strategies possible, restoring..." << endl;
            restore();
            count++;
        }
    } else {
        int count = 0;
        vector<pair<long long, int>> eliminated;  
        cout << "Using column " << useLine + 1 << endl;
        for(long long strat : origColStrats[useLine]) {
            cout << "Testing prospect of strategy " << count << "/" << origColStrats[useLine].size() << endl;
            columnStrategies[useLine].clear();
            columnStrategies[useLine].push_back(strat);
            columnSolved[useLine] = true;
            for(int i = 0; i < height; ++i) {
                state[i][useLine] = (strat & (1LLU << i)) != 0? FILL: EMPTY;
            }
            pair<bool, long long> eliminateResult = eliminateStrategies();
            if(eliminateResult.first) {
                if(isSolved()) {
                    printState();
                    return true;
                } else {
                    cout << "Strategy " << count << "/" << origColStrats[useLine].size() << " prunes " << eliminateResult.second << " others." << endl;
                    eliminated.push_back({eliminateResult.second, count});
                }
            } else {
                cout << "This strategy cannot work" << endl;
            }
            restore();
            count++;
        }
        sort(eliminated.begin(), eliminated.end(), [&](const pair<long long, int> &a, const pair<long long, int> &b){
            return a.first > b.first;
        });

        count = 0;
        for(auto &useStrats : eliminated){
            long long strat = origColStrats[useLine][useStrats.second];
            cout << "Using column " << useLine + 1 << " Strategy: " << count << "/" << eliminated.size() << ": " << strat << endl;
            columnSolved[useLine] = true;
            columnStrategies[useLine].clear();
            columnStrategies[useLine].push_back(strat);
            for(int i = 0; i < height; ++i) {
                state[i][useLine] = (strat & (1LLU << i)) != 0? FILL: EMPTY;
            }
            eliminateStrategies();
            if(solveRecursive()) {
                return true;
            }
            cout << "No further strategies possible, restoring..." << endl;
            restore();
            count++;
        }
    }
    return false;
}

std::pair<bool, long long> nonogramGame::eliminateStrategies() {
    rowSolved = vector<bool>(height, false);
    columnSolved = vector<bool>(width, false);

    int eliminated = 0;
    long long totalEliminated = 0;
    do {
        eliminated = 0;
        for(int i = 0; i < height; ++i) {
            if(rowSolved[i]) {continue;}
            vector<long long> strats;
            strats.reserve(rowStrategies[i].size());
            vector<gridState_t> currentState;
            currentState.reserve(width);
            for(int j = 0; j < width; ++j) {
                currentState.push_back(state[i][j]);
            }
            long long mustFill = -1, mustEmpty = -1;
            for(long long it : rowStrategies[i]) {
                if(!isValid(currentState, it)) {
                    ++eliminated;
                } else {
                    strats.push_back(it);
                    mustFill &= it;
                    mustEmpty &= ~(it);
                    ++it;
                }
            }
            if(strats.empty()) {
                return {false, 0};
            }
            if(strats.size() < rowStrategies[i].size()) {
                rowStrategies[i].resize(strats.size());
                for(long long unsigned int j = 0; j < strats.size(); ++j) {
                    rowStrategies[i][j] = strats[j];
                }
            }
            for(int j = 0; j < width; ++j) {
                if(mustFill & (1LLU << j)) {
                    state[i][j] = FILL;
                } else if(mustEmpty & (1LLU << j)) {
                    state[i][j] = EMPTY;
                }
            }
            if(rowStrategies.size() == 1) {
                rowSolved[i] = true;
            }
        }
        for(int j = 0; j < width; ++j) {
            if(columnSolved[j]) {continue;}
            
            vector<long long> strats;
            strats.reserve(columnStrategies[j].size());

            vector<gridState_t> currentState;
            currentState.reserve(width);
            for(int i = 0; i < height; ++i) {
                currentState.push_back(state[i][j]);
            }
            long long mustFill = -1, mustEmpty = -1;
            for(long long it : columnStrategies[j]) {
                if(!isValid(currentState, it)) {
                    ++eliminated;
                } else {
                    strats.push_back(it);
                    mustFill &= it;
                    mustEmpty &= ~(it);
                    ++it;
                }
            }
            if(strats.empty()) {
                return {false, 0};
            }
            if(strats.size() < columnStrategies[j].size()) {
                columnStrategies[j].resize(strats.size());
                for(long long unsigned int i = 0; i < strats.size(); ++i) {
                    columnStrategies[j][i] = strats[i];
                }
            }
            for(int i = 0; i < height; ++i) {
                if(mustFill & (1LLU << i)) {
                    state[i][j] = FILL;
                } else if(mustEmpty & (1LLU << i)) {
                    state[i][j] = EMPTY;
                }
            }
            if(columnStrategies[j].size() == 1) {
                columnSolved[j] = true;
            }
        }
        totalEliminated += eliminated;
    } while(eliminated);
    return {true, totalEliminated};
}

bool nonogramGame::isValid(const vector<gridState_t> &states, long long strategy) {
    size_t size = states.size();
    for(size_t i = 0; i < size; ++i) {
        bool bit = ((strategy & (1LLU << i)) != 0);
        if((bit && states[i] == EMPTY) || (!bit && states[i] == FILL)) {
            return false;
        }
    }
    return true;
}

void nonogramGame::generateStrategies() {
    for(int i = 0; i < height; ++i) {
        generateOneStrategy(rowStrategies[i], rowConstraints[i], width);
    }
    for(int i = 0; i < width; ++i) {
        generateOneStrategy(columnStrategies[i], columnConstraints[i], height);
    }
}

void nonogramGame::generateOneStrategy(vector<long long> &line, const vector<int> &constraint, const int size) {
    bool allZeros = true;
    for(int c : constraint) {
        if(c) {allZeros = false; break;}
    }
    if(allZeros) {
        line.push_back(0);
        return;
    }

    vector<int> sum(constraint.size() + 1, 0);
    sum[constraint.size() - 1] = constraint.back() + 1;
    for(int i = constraint.size() - 2; i >= 0; --i) {
        sum[i] = sum[i+1] + constraint[i] + 1;
    }
    long long output = 0;
    if(!generateRecursive(size, sum, line, output, 0, 0)) {
        cout << "Impossible!" << endl;
    }
}

bool nonogramGame::generateRecursive(const int size, const vector<int> &sum, vector<long long> &stratList, long long &output, int start, int level) {
    if(size_t(level) == sum.size() - 1) {
        stratList.push_back(output);
        return true;
    }

    int end = size - sum[level] + 1;
    if(end < start) { return false; }
    int num = sum[level] - sum[level + 1] - 1;
    long long oldOutput = output;
    long long mask = (1LLU << num) - 1;
    for(int i = start; i <= end; ++i) {
        output = oldOutput | (mask << i);
        if(!generateRecursive(size, sum, stratList, output, i + num + 1, level + 1)) {
            return false;
        }
    }
    output = oldOutput;
    return true;
}

int main() {
	unsigned int height, width;
    do {
        cout << "Input length and width: ";
        cin >> height >> width;
        cin.ignore(256, '\n');
    } while(height > 64 || width > 64 || height == 0 || width == 0);

    nonogramGame game(height, width);

    cout << "Input row constraints:" << endl;
    for(int i = 0; i < height; ++i) {
        string constraintString;
        getline(cin, constraintString);
        
        vector<int> constraint;
        stringstream ss;
        ss << constraintString;
        while(!ss.eof()) {
            int temp;
            ss >> temp;
            constraint.push_back(temp);
        }
        game.rowConstraints.push_back(constraint);
    }
    cout << "Input column constraints:" << endl;
    for(int i = 0; i < width; ++i) {
        string constraintString;
        getline(cin, constraintString);
        
        vector<int> constraint;
        stringstream ss;
        ss << constraintString;
        while(!ss.eof()) {
            int temp;
            ss >> temp;
            constraint.push_back(temp);
        }
        game.columnConstraints.push_back(constraint);
    }
    game.solve();
}