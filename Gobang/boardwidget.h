#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "gomoku.h"

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    Gomoku* game;      // 棋盘逻辑指针
    int cell = 36;     // 每格大小
    int margin = 30;   // 边距

    explicit BoardWidget(Gomoku* g, QWidget* parent = nullptr): QWidget(parent), game(g)
    {
        setMinimumSize(margin*2 + BOARD_SIZE*cell, margin*2 + BOARD_SIZE*cell);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.fillRect(rect(), QColor(245,222,179));  // 棋盘背景
        p.setRenderHint(QPainter::Antialiasing, true);

        int w = width() - margin*2;
        int h = height() - margin*2;
        cell = qMin(w/BOARD_SIZE, h/BOARD_SIZE);

        p.setPen(QPen(Qt::black, 1));

        // 画格子线
        for(int i=0; i<BOARD_SIZE; i++){
            int x = margin + cell/2 + i*cell;
            p.drawLine(x, margin + cell/2, x, margin + (BOARD_SIZE-1)*cell + cell/2);
            p.drawLine(margin + cell/2, margin + cell/2 + i*cell,
                       margin + (BOARD_SIZE-1)*cell + cell/2, margin + cell/2 + i*cell);
        }

        // 画棋子
        for(int r=0; r<BOARD_SIZE; r++){
            for(int c=0; c<BOARD_SIZE; c++){
                int v = game->board[r][c];
                if(v == EMPTY) continue;
                int cx = margin + cell/2 + c*cell;
                int cy = margin + cell/2 + r*cell;
                int rad = cell*0.4;
                if(v == BLACK) p.setBrush(Qt::black);
                else p.setBrush(Qt::white);
                p.setPen(QPen(Qt::black, 1));
                p.drawEllipse(QPoint(cx, cy), rad, rad);
            }
        }
    }

    void mousePressEvent(QMouseEvent* e) override {
        int x = e->x(), y = e->y();
        int c = (x - margin) / cell;
        int r = (y - margin) / cell;
        if(inBounds(r, c)) emit clicked(r, c);
    }

signals:
    void clicked(int r, int c);
};

#endif // BOARDWIDGET_H
