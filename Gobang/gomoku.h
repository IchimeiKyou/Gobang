#ifndef GOMOKU_H
#define GOMOKU_H

#include <vector>
#include <set>
#include <utility>
using namespace std;

// 棋盘大小和棋子类型常量
const int BOARD_SIZE = 15;        // 棋盘 15x15
const int EMPTY = 0;              // 空格
const int BLACK = 1;              // 黑棋
const int WHITE = 2;              // 白棋

// 定义四个方向：横、竖、斜1、斜2
const vector<pair<int,int>> DIRS = { {0,1}, {1,0}, {1,1}, {1,-1} };

// 检查坐标是否在棋盘范围内
inline bool inBounds(int r,int c){
    return r>=0 && r<BOARD_SIZE && c>=0 && c<BOARD_SIZE;
}

// ================== Gomoku 类 ==================
// 核心棋盘逻辑类，管理棋盘状态、落子、胜负判断、AI评分与搜索
class Gomoku {
public:
    vector<vector<int>> board;  // 棋盘数据，二维数组
    int lastR=-1, lastC=-1;     // 记录最后落子位置

    Gomoku(){ reset(); }         // 构造函数初始化棋盘

    // 重置棋盘，清空所有棋子
    void reset(){
        board.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
        lastR = lastC = -1;
    }

    // 落子函数
    bool makeMove(int r,int c,int player){
        if(!inBounds(r,c) || board[r][c]!=EMPTY) return false;
        board[r][c] = player;
        lastR = r; lastC = c;  // 更新最后落子
        return true;
    }

    // 撤销落子
    void undoMove(int r,int c){
        if(inBounds(r,c)) board[r][c] = EMPTY;
    }

    // 判断胜利：沿四个方向查找连续棋子是否 >= 5
    bool checkWin(int r,int c,int player) const {
        for(size_t i=0;i<DIRS.size();i++){
            int dr=DIRS[i].first, dc=DIRS[i].second;
            int cnt=1;                     // 当前连续棋子计数
            int rr=r+dr, cc=c+dc;
            while(inBounds(rr,cc) && board[rr][cc]==player){cnt++; rr+=dr; cc+=dc;}
            rr=r-dr; cc=c-dc;
            while(inBounds(rr,cc) && board[rr][cc]==player){cnt++; rr-=dr; cc-=dc;}
            if(cnt>=5) return true;       // 连子 >=5 判胜
        }
        return false;
    }

    // 生成候选落子点，尽量靠近已有棋子
    vector<pair<int,int>> genCandidates() const {
        set<pair<int,int>> s;
        for(int r=0;r<BOARD_SIZE;r++)
            for(int c=0;c<BOARD_SIZE;c++)
                if(board[r][c]!=EMPTY)        // 以已有棋子为中心
                    for(int dr=-2;dr<=2;dr++)
                        for(int dc=-2;dc<=2;dc++){
                            int nr=r+dr,nc=c+dc;
                            if(inBounds(nr,nc) && board[nr][nc]==EMPTY)
                                s.insert(make_pair(nr,nc));
                        }
        // 如果棋盘空，默认放在中央
        if(s.empty()) return {make_pair(BOARD_SIZE/2, BOARD_SIZE/2)};
        return vector<pair<int,int>>(s.begin(),s.end());
    }

    // 评估某一方棋形得分，用于 AI
    int evaluateOneSide(int player) const {
        int score=0;
        for(int r=0;r<BOARD_SIZE;r++)
            for(int c=0;c<BOARD_SIZE;c++)
                if(board[r][c]==player)
                    for(size_t i=0;i<DIRS.size();i++){
                        int dr=DIRS[i].first, dc=DIRS[i].second;
                        int cnt=1;
                        int rr=r+dr,cc=c+dc;
                        while(inBounds(rr,cc) && board[rr][cc]==player){cnt++; rr+=dr; cc+=dc;}
                        bool f1=inBounds(rr,cc) && board[rr][cc]==EMPTY; // 前端是否开放
                        rr=r-dr; cc=c-dc;
                        while(inBounds(rr,cc) && board[rr][cc]==player){cnt++; rr-=dr; cc-=dc;}
                        bool f2=inBounds(rr,cc) && board[rr][cc]==EMPTY; // 后端是否开放
                        int open=(f1?1:0)+(f2?1:0);

                        // 不同棋型评分
                        if(cnt>=5) score+=1000000;         // 五连
                        else if(cnt==4&&open==2) score+=100000; // 活四
                        else if(cnt==4&&open==1) score+=10000;  // 冲四
                        else if(cnt==3&&open==2) score+=5000;   // 活三
                        else if(cnt==3&&open==1) score+=1000;   // 冲三
                        else if(cnt==2&&open==2) score+=100;    // 活二
                    }
        return score;
    }

    // 评估整体局面：我方分 - 对方分
    int evaluate(int player) const {
        int my=evaluateOneSide(player);
        int opp=evaluateOneSide(player==BLACK?WHITE:BLACK);
        return my-opp;
    }

    // 极大极小搜索（Negamax）
    int negamax(int depth,int player){
        if(depth==0) return evaluate(player);
        int best=-9999999;
        vector<pair<int,int>> cand=genCandidates(); // 获取候选落子点
        for(size_t i=0;i<cand.size();i++){
            int r=cand[i].first,c=cand[i].second;
            board[r][c]=player;
            if(checkWin(r,c,player)){ board[r][c]=EMPTY; return 1000000; } // 胜利剪枝
            int val=-negamax(depth-1,player==BLACK?WHITE:BLACK);
            board[r][c]=EMPTY;
            if(val>best) best=val;
        }
        return best;
    }

    // 计算最佳落子点
    pair<int,int> bestMove(int player,int depth){
        vector<pair<int,int>> cand=genCandidates();
        pair<int,int> best(-1,-1);
        int bestVal=-9999999;
        for(size_t i=0;i<cand.size();i++){
            int r=cand[i].first,c=cand[i].second;
            board[r][c]=player;
            if(checkWin(r,c,player)){board[r][c]=EMPTY;return cand[i];} // 直接胜利
            int val=-negamax(depth-1,player==BLACK?WHITE:BLACK);
            board[r][c]=EMPTY;
            if(val>bestVal){bestVal=val;best=cand[i];}
        }
        if(best.first==-1 && !cand.empty()) best=cand[0]; // 保底返回
        return best;
    }
};

#endif // GOMOKU_H
