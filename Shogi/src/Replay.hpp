#pragma once
#include "Kyokumen.hpp"

class Replay : public MyApp::Scene {
private:
    // 将棋盤
    s3d::RectF m_shogiBan;
    // 敵の駒台
    s3d::RectF m_komaDaiEnemy;
    // 自分の駒台
    s3d::RectF m_komaDaiSelf;

    // 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    s3d::Array<KomaSquare> m_boardSquares;
    // 持ち駒（プレイヤー側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingSelfKoma;
    // 持ち駒（敵側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingEnemyKoma;
    // 駒を置いた部分の色をちょっと赤くするための変数
    s3d::Optional<KomaSquare> m_placedPart;

    // マウスで駒を保持
    s3d::Optional<KomaSquare> m_holdHand;

    /// <summary>
    /// セーブボタン
    /// </summary>
    s3d::RectF m_buttonSave;

    /// <summary>
    /// 戻るボタン
    /// </summary>
    s3d::RectF m_buttonQuit;

    s3d::RectF m_firstButton;
    s3d::RectF m_prevButton;
    s3d::RectF m_nextButton;
    s3d::RectF m_lastButton;

    s3d::Transition m_firstButtonTransition;
    s3d::Transition m_prevButtonTransition;
    s3d::Transition m_nextButtonTransition;
    s3d::Transition m_lastButtonTransition;

    uint32 m_moveNum;
    uint32 m_saveInitNum;

    s3d::CSVData m_csv;
    s3d::String m_saveFileName;

    bool m_isSaveFile;

    void MoveNext() {
        if (m_moveNum >= (getData().stackBoradSquares.size() - 1)) {
            return;
        }

        ++m_moveNum;
        m_boardSquares.assign(getData().stackBoradSquares[m_moveNum].begin(), getData().stackBoradSquares[m_moveNum].end());
        m_havingSelfKoma.assign(getData().stackHavingSelf[m_moveNum].begin(), getData().stackHavingSelf[m_moveNum].end());
        m_havingEnemyKoma.assign(getData().stackHavingEnemy[m_moveNum].begin(), getData().stackHavingEnemy[m_moveNum].end());
        m_placedPart = getData().stackPlacedPart[m_moveNum];
        m_isSaveFile = false;
    }

    void MovePrevious() {
        if (m_moveNum <= 0) {
            return;
        }

        --m_moveNum;
        m_boardSquares.assign(getData().stackBoradSquares[m_moveNum].begin(), getData().stackBoradSquares[m_moveNum].end());
        m_havingSelfKoma.assign(getData().stackHavingSelf[m_moveNum].begin(), getData().stackHavingSelf[m_moveNum].end());
        m_havingEnemyKoma.assign(getData().stackHavingEnemy[m_moveNum].begin(), getData().stackHavingEnemy[m_moveNum].end());
        m_placedPart = getData().stackPlacedPart[m_moveNum];
        m_isSaveFile = false;
    }

    void MoveFirst() {
        m_moveNum = 0;
        m_boardSquares.assign(getData().stackBoradSquares[m_moveNum].begin(), getData().stackBoradSquares[m_moveNum].end());
        m_havingSelfKoma.assign(getData().stackHavingSelf[m_moveNum].begin(), getData().stackHavingSelf[m_moveNum].end());
        m_havingEnemyKoma.assign(getData().stackHavingEnemy[m_moveNum].begin(), getData().stackHavingEnemy[m_moveNum].end());
        m_placedPart = getData().stackPlacedPart[m_moveNum];
        m_isSaveFile = false;
    }

    void MoveLast() {
        m_moveNum = getData().stackBoradSquares.size() - 1;
        m_boardSquares.assign(getData().stackBoradSquares[m_moveNum].begin(), getData().stackBoradSquares[m_moveNum].end());
        m_havingSelfKoma.assign(getData().stackHavingSelf[m_moveNum].begin(), getData().stackHavingSelf[m_moveNum].end());
        m_havingEnemyKoma.assign(getData().stackHavingEnemy[m_moveNum].begin(), getData().stackHavingEnemy[m_moveNum].end());
        m_placedPart = getData().stackPlacedPart[m_moveNum];
        m_isSaveFile = false;
    }

    void SaveFile(s3d::StringView str_);

public:
    Replay(const InitData& init, const double shogiBan_ = 540.f, const double komaDai_ = 240.f);

    void update() override;

    void draw() const override;
};