#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "gomoku.h"
#include "boardwidget.h"
#include "settingsdialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    Gomoku game;
    BoardWidget* board;
    QLabel* status;

    GameMode mode = HUMAN_AI;
    int aiDepth = 2;
    bool aiThinking = false;
    int currentPlayer = BLACK;
    int humanPlayer = BLACK;
    bool gameOver = false;

    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        QWidget* central = new QWidget;
        setCentralWidget(central);

        board = new BoardWidget(&game, this);
        connect(board, &BoardWidget::clicked, this, &MainWindow::playerMove);

        status = new QLabel;
        status->setAlignment(Qt::AlignCenter);
        status->setStyleSheet("font-size:24px;"); // 默认空白

        QPushButton* newGameBtn = new QPushButton("再挑戦");
        QPushButton* settingsBtn = new QPushButton("設定");
        connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::startNewGame);
        connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::openSettings);

        QVBoxLayout* rightLayout = new QVBoxLayout;
        rightLayout->addStretch();
        rightLayout->addWidget(status, 0, Qt::AlignCenter);
        rightLayout->addStretch();
        rightLayout->addWidget(newGameBtn);
        rightLayout->addWidget(settingsBtn);
        rightLayout->addStretch();

        QWidget* rightWidget = new QWidget;
        rightWidget->setLayout(rightLayout);
        rightWidget->setFixedWidth(180);

        QHBoxLayout* mainLayout = new QHBoxLayout;
        mainLayout->addWidget(board, 1);
        mainLayout->addWidget(rightWidget);
        central->setLayout(mainLayout);

        setWindowTitle("五目並べ");
        resize(900,700);

        if(mode == AI_AI){
            QTimer::singleShot(500, this, [=](){ autoMove(BLACK); });
        }
    }

public slots:
    void playerMove(int r, int c){
        if(aiThinking || gameOver || game.board[r][c]!=EMPTY) return;
        if(mode==HUMAN_AI && currentPlayer != humanPlayer) return;
        if(mode==AI_AI) return;

        game.makeMove(r, c, currentPlayer);
        if(checkWinAndHandle(r,c,currentPlayer)) return;

        if(mode==HUMAN_HUMAN){
            currentPlayer = (currentPlayer==BLACK?WHITE:BLACK);
            return;
        }

        if(mode==HUMAN_AI){
            aiThinking = true;
            int ai = (humanPlayer==BLACK?WHITE:BLACK);
            QTimer::singleShot(300, this, [=](){ autoMove(ai); });
        }
    }

    void autoMove(int player){
        if(gameOver) return;
        aiThinking = true;

        QTimer::singleShot(500, this, [=](){
            pair<int,int> move(-1,-1);

            // 检查是否能直接获胜
            move = game.bestMove(player, aiDepth);
            if(game.checkWin(move.first, move.second, player)){
                game.makeMove(move.first, move.second, player);
                checkWinAndHandle(move.first, move.second, player);
                aiThinking=false;
                return;
            }

            // 检查是否必须堵住对方四连
            int opponent = (player==BLACK?WHITE:BLACK);
            vector<pair<int,int>> cand = game.genCandidates();
            for(auto &p : cand){
                game.board[p.first][p.second] = opponent;
                if(game.checkWin(p.first, p.second, opponent)){
                    move = p; // 对方马上连五，堵住
                    game.board[p.first][p.second] = EMPTY;
                    break;
                }
                game.board[p.first][p.second] = EMPTY;
            }

            // 如果没有紧急堵四，就按 bestMove
            if(move.first == -1){
                move = game.bestMove(player, aiDepth);
            }

            // 落子
            game.makeMove(move.first, move.second, player);
            if(checkWinAndHandle(move.first, move.second, player)){
                aiThinking=false;
                return;
            }

            currentPlayer = (player==BLACK?WHITE:BLACK);
            board->update();

            if(mode==AI_AI){
                autoMove(currentPlayer);
            } else {
                aiThinking=false;
            }
        });
    }

    void startNewGame(){
        game.reset();
        board->update();
        aiThinking=false;
        gameOver=false;
        currentPlayer = BLACK;
        status->clear(); // 新游戏清空显示

        if(mode==HUMAN_AI){
            if(humanPlayer == BLACK){
                currentPlayer = humanPlayer;
            }
            else{
                aiThinking=true;
                QTimer::singleShot(300, this, [=](){ autoMove(BLACK); });
            }
        }

        if(mode==AI_AI){
            QTimer::singleShot(300, this, [=](){ autoMove(BLACK); });
        }
    }

    void openSettings(){
        SettingsDialog dlg(this);
        dlg.aiDepthSlider->setValue(aiDepth);

        if(dlg.exec() == QDialog::Accepted){
            mode = dlg.selectedMode();
            aiDepth = dlg.selectedAIDepth();
            humanPlayer = dlg.selectedHumanPlayer();
            startNewGame();
        }
    }

private:
    bool checkWinAndHandle(int r,int c,int player){
        if(game.checkWin(r,c,player)){
            board->update();
            gameOver = true;
            QString msg = (player==BLACK?"黑の勝ち":"白の勝ち");
            status->setText(msg);
            status->setStyleSheet("font-size:30px; color:blue; font-weight:bold;");
            status->setAlignment(Qt::AlignCenter);
            aiThinking = true; // 胜利后禁止继续落子
            return true;
        }
        board->update();
        return false;
    }

    void updateStatus(){
        // 不再显示任何轮次或 AI 思考状态
        board->update();
    }
};

#endif // MAINWINDOW_H
