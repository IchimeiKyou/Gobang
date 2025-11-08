#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRadioButton>

enum GameMode { HUMAN_HUMAN, HUMAN_AI, AI_AI };

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    QSlider* aiDepthSlider;
    QRadioButton* blackBtn;
    QRadioButton* whiteBtn;
    GameMode selectedModeVal;
    int humanPlayerColor;

    SettingsDialog(QWidget* parent = nullptr) : QDialog(parent), selectedModeVal(HUMAN_HUMAN), humanPlayerColor(1) {
        QLabel* modeLabel = new QLabel("モード選択:");
        QLabel* depthLabel = new QLabel("AI難易度:");
        QLabel* colorLabel = new QLabel("プレーヤー：");

        // AI难度滑块
        aiDepthSlider = new QSlider(Qt::Horizontal);
        aiDepthSlider->setRange(1, 5);
        aiDepthSlider->setValue(5);

        // 模式按钮
        QPushButton* humanHumanBtn = new QPushButton("人対人");
        QPushButton* humanAIBtn = new QPushButton("人対AI");
        QPushButton* aiAiBtn = new QPushButton("AI対AI");

        // 黑/白单选按钮
        blackBtn = new QRadioButton("黑");
        whiteBtn = new QRadioButton("白");
        blackBtn->setChecked(true);

        connect(humanHumanBtn, &QPushButton::clicked, this, [=](){
            selectedModeVal = HUMAN_HUMAN;
            humanPlayerColor = 1; // 默认黑
            accept();
        });
        connect(humanAIBtn, &QPushButton::clicked, this, [=](){
            selectedModeVal = HUMAN_AI;
            humanPlayerColor = blackBtn->isChecked()?1:2; // 根据选择
            accept();
        });
        connect(aiAiBtn, &QPushButton::clicked, this, [=](){
            selectedModeVal = AI_AI;
            humanPlayerColor = 1;
            accept();
        });

        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(modeLabel);
        layout->addWidget(humanHumanBtn);
        layout->addWidget(humanAIBtn);
        layout->addWidget(aiAiBtn);
        layout->addWidget(depthLabel);
        layout->addWidget(aiDepthSlider);
        layout->addWidget(colorLabel);
        layout->addWidget(blackBtn);
        layout->addWidget(whiteBtn);

        setLayout(layout);
        setWindowTitle("ゲーム設定");
        setFixedSize(300, 300);
    }

    GameMode selectedMode() const { return selectedModeVal; }
    int selectedAIDepth() const { return aiDepthSlider->value(); }
    int selectedHumanPlayer() const { return humanPlayerColor; }
};

#endif
