#include "Replay.hpp"

Replay::Replay(const InitData& init, const double shogiBan_, const double komaDai_)
: IScene(init)
, m_shogiBan(s3d::Arg::center(s3d::Scene::CenterF()), shogiBan_)
, m_komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, (shogiBan_ / 2 - komaDai_) + komaDai_ / 2)), komaDai_)
, m_komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(shogiBan_ / 2 + 10 + komaDai_ / 2), -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), komaDai_)
, m_moveNum(0) {
    m_boardSquares.assign(getData().stackBoradSquares.front().begin(), getData().stackBoradSquares.front().end());
    m_havingSelfKoma.assign(getData().stackHavingSelf.front().begin(), getData().stackHavingSelf.front().end());
    m_havingEnemyKoma.assign(getData().stackHavingEnemy.front().begin(), getData().stackHavingEnemy.front().end());
    m_placedPart = getData().stackPlacedPart.front();
}

void Replay::update() {
    if (s3d::KeyLeft.down()) {
        MovePrevious();
        return;
    }

    if (s3d::KeyRight.down()) {
        MoveNext();
        return;
    }

    if (s3d::MouseL.down()) {
        changeScene(State::Title);
    }
}

void Replay::draw() const {
    m_shogiBan.draw(s3d::Palette::Burlywood);
    m_komaDaiSelf.draw(s3d::Palette::Burlywood);
    m_komaDaiEnemy.draw(s3d::Palette::Burlywood);

    // 駒を置いた場所を少し赤くする
    if (m_placedPart.has_value()) {
        m_placedPart.value().draw(s3d::ColorF(s3d::Palette::Red, 0.5f));
    }

    for (const auto& square : m_boardSquares) {
        square.drawFrame(2, s3d::Palette::Black);

        square.Draw();
    }

    for (const auto& selfDai : m_havingSelfKoma) {
        for (const auto [i, koma] : s3d::IndexedRefReversed(selfDai)) {
            koma.Draw(s3d::Vec2(i * 10, 0));
        }
    }
    for (const auto& enemyDai : m_havingEnemyKoma) {
        for (const auto [i, koma] : s3d::IndexedRefReversed(enemyDai)) {
            koma.Draw(s3d::Vec2(static_cast<int32>(i) * -10, 0));
        }
    }
}