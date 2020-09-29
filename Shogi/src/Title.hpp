
#pragma once
#include "Kyokumen.hpp"

// タイトルシーン
class Title : public MyApp::Scene {
private:
    s3d::Rect m_soloMatchButton;
    s3d::Transition m_soloMatchTransition;

    s3d::Rect m_twoPlayerMatchButton;
    s3d::Transition m_twoPlayerMatchTransition;

    s3d::Rect m_exitButton;
    s3d::Transition m_exitTransition;

public:
    Title(const InitData& init);

    void update() override;

    void draw() const override;
};


class Select : public MyApp::Scene {
private:
    size_t firstMoveIndx = 0;
    size_t handicapIndx = 0;
    size_t eleganceIndx = 0;
    size_t depthMaxIndx = 0;

    const s3d::Array<s3d::String> handicaps
        = (getData().gameState == State::PlayAlone
               ? s3d::Array<s3d::String>{U"平手", U"角落ち", U"飛車落ち", U"2枚落ち", U"4枚落ち", U"6枚落ち", U"8枚落ち", U"10枚落ち", U"カスタム"}
               : s3d::Array<s3d::String>{U"平手", U"角落ち", U"飛車落ち", U"2枚落ち", U"4枚落ち", U"6枚落ち", U"8枚落ち", U"10枚落ち"});

    bool m_isCustom;

    bool m_isNifu;
    bool m_isFirstOute;

    bool m_isReadCsv;

    bool m_isSaveFile;

    // カスタムを選んだ時に使うメンバ変数

    // 将棋盤
    s3d::RectF m_shogiBan;

    s3d::RectF m_enemyDai, m_selfDai;

    // 敵の駒台
    s3d::RectF m_komaDaiEnemy;
    // 自分の駒台
    s3d::RectF m_komaDaiSelf;

    // 持ち駒（プレイヤー側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingSelfKoma;
    // 持ち駒（敵側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingEnemyKoma;

    // 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    s3d::Array<KomaSquare> m_boardSquares;

    // 選ぶ駒（プレイヤー側）
    s3d::Array<KomaSquare> m_selectSelfKomas;
    // 選ぶ駒（敵側）
    s3d::Array<KomaSquare> m_selectEnemyKomas;

    // マウスで駒を保持
    s3d::Optional<KomaSquare> m_holdHand;

    s3d::CSVData m_csv;
    s3d::String m_saveFileName;

    s3d::Optional<s3d::String> m_loadFileName;

    void ParseInit();

    void SetUp();

    void Custom();

    void DisconnectReturn() override {
        s3d::Print(U"切断しました1");
        changeScene(State::Title);
    }

    void LeaveRoomEventAction(int playerNr, bool isInactive) override {
        s3d::Print(U"対戦相手が退室しました。");
        Disconnect();
    }

public:
    Select(const InitData& init);

    void update() override;

    /// <summary>
    /// フェードインの時に描画されないので仕方なく
    /// </summary>
    void updateFadeIn(double) override;

    void draw() const override;
};
