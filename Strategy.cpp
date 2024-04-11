#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "Point.h"
#include "Strategy.h"
#include "GameState.h"
#include "Judge.h"

using namespace std;
using namespace std::chrono;

// 定义蒙特卡洛树节点结构体
struct MonteCarloTreeNode
{
	Point move;							   // 落子点
	int visits;							   // 访问次数
	double score;						   // 得分
	MonteCarloTreeNode *parent;			   // 父节点
	vector<MonteCarloTreeNode *> children; // 子节点
	vector<int> positions;				   // 未访问节点
	MonteCarloTreeNode(Point move, MonteCarloTreeNode *parent = nullptr)
		: move(move), visits(0), score(0), parent(parent) {}
};

// 检测是否需要防守
bool hasConsecutive(const GameState &state, Point point)
{
	int M = state.M;
	int N = state.N;
	int **board = state.board;

	// 横向三连检查
	if ((point.y + 3 < N && board[point.x][point.y + 1] != 0
		&&  board[point.x][point.y + 1] == board[point.x][point.y + 2] 
		&& board[point.x][point.y + 1] == board[point.x][point.y + 3])
		||(point.y + 2 < N && point.y - 1 >= 0 && board[point.x][point.y - 1] != 0
		&&  board[point.x][point.y - 1] == board[point.x][point.y + 1] 
		&& board[point.x][point.y - 1] == board[point.x][point.y + 2])
		||(point.y - 3 >= 0 &&  board[point.x][point.y - 1] != 0
		&&  board[point.x][point.y - 1] == board[point.x][point.y - 2] 
		&& board[point.x][point.y - 1] == board[point.x][point.y - 3])
		||(point.y - 2 >= 0 && point.y + 1 < N && board[point.x][point.y + 1] != 0
		&&  board[point.x][point.y + 1] == board[point.x][point.y - 1] 
		&& board[point.x][point.y + 1] == board[point.x][point.y - 2]))
		return true;

	// 纵向三连检查
	if ((point.x + 3 < M && board[point.x + 1][point.y] != 0
		&&  board[point.x + 1][point.y] == board[point.x + 2][point.y] 
		&& board[point.x + 1][point.y] == board[point.x + 3][point.y])
		||(point.x - 3 >= 0 &&  board[point.x - 1][point.y] != 0
		&&  board[point.x - 1][point.y] == board[point.x - 2][point.y] 
		&& board[point.x - 1][point.y] == board[point.x - 3][point.y])
		||(point.x + 2 < M && point.x - 1 >= 0 && board[point.x - 1][point.y] != 0
		&&  board[point.x - 1][point.y] == board[point.x + 1][point.y] 
		&& board[point.x - 1][point.y] == board[point.x + 1][point.y])
		||(point.x - 2 >= 0 && point.x + 1 < M && board[point.x + 1][point.y] != 0
		&&  board[point.x + 1][point.y] == board[point.x - 1][point.y] 
		&& board[point.x + 1][point.y] == board[point.x - 2][point.y]))
		return true;

	// 斜向三连检查
	if ((point.x + 3 < M && point.y + 3 < N &&  board[point.x + 1][point.y + 1] != 0
		&&  board[point.x + 1][point.y + 1] == board[point.x + 2][point.y + 2] 
		&& board[point.x + 1][point.y + 1] == board[point.x + 3][point.y + 3])
		||(point.x - 3 >= 0 && point.y - 3 >= 0 &&  board[point.x - 1][point.y - 1] != 0
		&&  board[point.x - 1][point.y - 1] == board[point.x - 2][point.y - 2] 
		&& board[point.x - 1][point.y - 1] == board[point.x - 3][point.y - 3])
		||(point.x + 2 < M && point.x - 1 >= 0 && point.y + 2 < N && point.y - 1 >= 0 && board[point.x - 1][point.y - 1] != 0
		&&  board[point.x - 1][point.y - 1] == board[point.x + 1][point.y + 1] 
		&& board[point.x - 1][point.y - 1] == board[point.x + 2][point.y + 2])
		||(point.x - 2 >= 0 && point.x + 1 < M && point.y - 2 >= 0 && point.y + 1 < N && board[point.x + 1][point.y + 1] != 0
		&&  board[point.x + 1][point.y + 1] == board[point.x - 1][point.y - 1] 
		&& board[point.x + 1][point.y + 1] == board[point.x - 2][point.y - 2]))
		return true;


	if ((point.x + 3 < M && point.y - 3 >= 0 &&  board[point.x + 1][point.y - 1] != 0
		&&  board[point.x + 1][point.y - 1] == board[point.x + 2][point.y - 2] 
		&& board[point.x + 1][point.y - 1] == board[point.x + 3][point.y - 3])
		||(point.x - 3 >= 0 && point.y + 3 < N &&  board[point.x - 1][point.y + 1] != 0
		&&  board[point.x - 1][point.y + 1] == board[point.x - 2][point.y + 2] 
		&& board[point.x - 1][point.y + 1] == board[point.x - 3][point.y + 3])
		||(point.x + 2 < M && point.x - 1 >= 0 && point.y - 2 >= 0 && point.y + 1 < N &&  board[point.x - 1][point.y + 1] != 0
		&&  board[point.x - 1][point.y + 1] == board[point.x + 1][point.y - 1] 
		&& board[point.x - 1][point.y + 1] == board[point.x + 2][point.y - 2])
		||(point.x - 2 >= 0 && point.x + 1 < M && point.y + 2 < N && point.y - 1 >= 0 && board[point.x + 1][point.y - 1] != 0
		&&  board[point.x + 1][point.y - 1] == board[point.x - 1][point.y + 1] 
		&& board[point.x + 1][point.y - 1] == board[point.x - 2][point.y + 2]))
		return true;

	return false;
}

Point getRandomMove(const GameState &state)
{
	vector<int> legalMoves;
	for (int i = 0; i < state.N; i++)
	{
		if (state.top[i] > 0)
		{
			legalMoves.push_back(i);
			Point test = Point(state.top[i] - 1, i);
			if (hasConsecutive(state, test))
			return test;
		}
	}

	int selectedIndex = rand() % legalMoves.size();
	int selectedColumn = legalMoves[selectedIndex];
	int selectedRow = state.top[selectedColumn] - 1;

	return Point(selectedRow, selectedColumn);
}

// 辅助函数：计算落子点的得分
double calculateScore(int visits, double score, double parentvisits)
{
	// 使用UCB1公式计算得分
	// cout << "visits:" << visits << " score:" << score << " parent:" << parentscore << endl;
	const double explorationFactor = 0.6;
	if (visits)
		return score / visits + explorationFactor * sqrt((2 * log(parentvisits)) / visits);
	else
		return -1;
}

// 辅助函数：选择最佳子节点
MonteCarloTreeNode *selectBestChild(const MonteCarloTreeNode *node)
{
	double bestScore = -1000000000000000;
	MonteCarloTreeNode *bestChild = nullptr;

	for (const auto &child : node->children)
	{
		double score = calculateScore(child->visits, child->score, node->visits);
		if (score > bestScore)
		{
			bestScore = score;
			bestChild = child;
		}
	}

	return bestChild;
}

// 辅助函数：选择最佳落子节点
MonteCarloTreeNode *selectBestPos(const MonteCarloTreeNode *node)
{
	double bestScore = -1000000000000000;
	MonteCarloTreeNode *bestChild = nullptr;

	for (const auto &child : node->children)
	{
		double score = child->score;
		cout << " " << score;
		if (score > bestScore)
		{
			bestScore = score;
			bestChild = child;
		}
	}

	return bestChild;
}

// 辅助函数：释放蒙特卡洛树节点及其子节点
void releaseMonteCarloTree(MonteCarloTreeNode *node)
{
	if (!node)
	{
		return;
	}

	for (const auto &child : node->children)
	{
		releaseMonteCarloTree(child);
	}

	delete node;
}

// 模拟执行游戏并返回最终得分
double simulateGame(const GameState &state, const Point &move)
{
	GameState currentState = GameState(state);
	Point randomMove = move;
	double step = 1, score;

	// 在这里模拟执行游戏，直到有一方胜利或平局
	while (!userWin(randomMove.x, randomMove.y, currentState.M, currentState.N, currentState.board) && !machineWin(randomMove.x, randomMove.y, currentState.M, currentState.N, currentState.board) && !isTie(currentState.N, currentState.top))
	{
		step++;
		randomMove = getRandomMove(currentState);
		currentState.board[randomMove.x][randomMove.y] = currentState.player;
		currentState.top[randomMove.y]--;
		currentState.player = 3 - currentState.player;
		// cout << "正在模拟:" << 3 - currentState.player << "落子于[" << randomMove.x << "," << randomMove.y << "]" << endl;
	}
    
	step = step / 10;
	// cout << "模拟步数" << step << endl;
	// 返回最终得分
	if (machineWin(randomMove.x, randomMove.y, currentState.M, currentState.N, currentState.board))
	{
		// 当前玩家获胜
		if(state.player == 1) score = 1 - step;
		else score = -1 + step;
		// cout << "模拟结果为胜， 得分" << score << endl;
	}
	else if (userWin(randomMove.x, randomMove.y, currentState.M, currentState.N, currentState.board))
	{
		// 对手获胜
		if(state.player == 2) score = 1 - step;
		else score = -1 + step;
		// cout << "模拟结果为负， 得分" << score << endl;
	}
	else
	{
		// 平局
		score = 0;
		// cout << "模拟结果为平， 得分" << score << endl;
	}
	return score;
}

// MCTS算法主函数：选择最佳落子点
Point *selectBestMove(const GameState &state, int numIterations)
{
	MonteCarloTreeNode *root = new MonteCarloTreeNode(Point(-1, -1));
	auto startTime = high_resolution_clock::now();  // 记录开始时间
    auto endTime = startTime + seconds(2);  // 2.9秒后的结束时间
	for (int i = 0; i < state.N; i++)
	{
		if (state.top[i] > 0)
		{
			root->positions.push_back(i);
			Point test = Point(state.top[i] - 1, i);
			if (hasConsecutive(state, test)){
				//cout << "执行防守策略，落子于[" << test.x << "," << test.y << "]" << endl; 
				return new Point(test.x, test.y);
			}
		}
	}
	while(high_resolution_clock::now() < endTime)
	{
		// 选择阶段
		// cout << "正在选择..." << endl;
		MonteCarloTreeNode *node = root;
		GameState currentState = GameState(state);

		// cout << "正在选择子节点..." << endl;
		while (!node->positions.size() && node->children.size())
		{
			node = selectBestChild(node);
			// cout << "正在更新， 位置" << node->move.x << "," << node->move.y << endl;
			currentState.board[node->move.x][node->move.y] = currentState.player;
			currentState.top[node->move.y]--;
			currentState.player = 3 - currentState.player;
			// cout << "完成更新..." << endl;
		}
		if (node->move.x == -1 || (!userWin(node->move.x, node->move.y, currentState.M, currentState.N, currentState.board) && !machineWin(node->move.x, node->move.y, currentState.M, currentState.N, currentState.board) && !isTie(currentState.N, currentState.top)))
		{
			// 扩展阶段
			// cout << "正在扩展..." << endl;
			int column = node->positions.back();
			Point move = Point(currentState.top[column] - 1, column);
			node->positions.pop_back();
			// cout << "完成落子， 位置" << currentState.top[randcolumn] - 1 << "," << randcolumn << endl;
			currentState.board[move.x][move.y] = currentState.player;
			currentState.top[move.y]--;
			currentState.player = 3 - currentState.player;
			// cout << "更新棋盘..." << endl;
			MonteCarloTreeNode *child = new MonteCarloTreeNode(move, node);
			for (int i = 0; i < currentState.N; i++)
			{
				if (currentState.top[i] > 0)
				{
					child->positions.push_back(i);
				}
			}
			node->children.push_back(child);
			node = node->children.back();
			// cout << "更新节点..." << endl;
		}
		// 模拟阶段
		// cout << "正在模拟..." << endl;
		double score = simulateGame(currentState, node->move);

		// 回溯阶段
		// cout << "正在回溯..." << endl;
		while (node != nullptr)
		{
			node->visits++;
			node->score += score;
			score = -score;
			node = node->parent;
		}
	}

	// cout << "正在选择最佳落子点..." << endl;
	//  选择最佳子节点作为落子点
	// cout << "正在从";
	MonteCarloTreeNode *bestChild = selectBestChild(root);
	// cout << "中选择最优" << endl;
	Point *bestMove = new Point(bestChild->move.x, bestChild->move.y);
	//cout << "最优点权重" << bestChild->score << endl;
	//cout << "落子于[" << bestChild->move.x << "," << bestChild->move.y << "]" << endl;

	// 释放内存
	releaseMonteCarloTree(root);

	return bestMove;
}
/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误

	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:涫嫡饫锔?龅膖op已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/
extern "C" Point *getPoint(const int M, const int N, const int *top, const int *_board,
						   const int lastX, const int lastY, const int noX, const int noY)
{
	/*
		不要更改这段代码
	*/
	int x = -1, y = -1; // 最终将你的落子点存到x,y中
	int **board = new int *[M];
	for (int i = 0; i < M; i++)
	{
		board[i] = new int[N];
		for (int j = 0; j < N; j++)
		{
			board[i][j] = _board[i * N + j];
		}
	}

	/*
		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
	*/
	// Add your own code below
	GameState state = GameState(M, N, top, board, 2);

	// 使用MCTS算法选择最佳落子点
	Point *bestMove = selectBestMove(state, 10000);

	x = bestMove->x;
	y = bestMove->y;
	/*
	 //a naive example
	for (int i = N-1; i >= 0; i--) {
		if (top[i] > 0) {
			x = top[i] - 1;
			y = i;
			break;
		}
	}
	*/

	/*
		不要更改这段代码
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}

/*
	getPoint函数返回的Point指针是在本dll模块中声明的，为避免产生堆错误，应在外部调用本dll中的
	函数来释放空间，而不应该在外部直接delete
*/
extern "C" void clearPoint(Point *p)
{
	delete p;
	return;
}

/*
	清除top和board数组
*/
void clearArray(int M, int N, int **board)
{
	for (int i = 0; i < M; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

/*
	添加你自己的辅助函数，你可以声明自己的类、函数，添加新的.h .cpp文件来辅助实现你的想法
*/
