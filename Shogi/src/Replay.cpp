#include "Replay.hpp"

Replay::Replay(const InitData& init, const double shogiBan_, const double komaDai_)
: IScene(init)
, m_shogiBan(s3d::Arg::center(s3d::Scene::CenterF()), shogiBan_)
, m_komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, (shogiBan_ / 2 - komaDai_) + komaDai_ / 2)), komaDai_)
, m_komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(shogiBan_ / 2 + 10 + komaDai_ / 2), -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), komaDai_)
, m_moveNum(0)
, m_saveInitNum(0)
, m_isSaveFile(false)
, m_firstButton(m_komaDaiEnemy.x - 20, m_komaDaiSelf.bl().y - (komaDai_ / 4), komaDai_ / 4)
, m_prevButton(m_firstButton.tr().movedBy(5, 0), komaDai_ / 4)
, m_nextButton(m_prevButton.tr().movedBy(5, 0), komaDai_ / 4)
, m_lastButton(m_nextButton.tr().movedBy(5, 0), komaDai_ / 4)
, m_firstButtonTransition(s3d::SecondsF(0.1), s3d::SecondsF(0.1))
, m_prevButtonTransition(s3d::SecondsF(0.1), s3d::SecondsF(0.1))
, m_nextButtonTransition(s3d::SecondsF(0.1), s3d::SecondsF(0.1))
, m_lastButtonTransition(s3d::SecondsF(0.1), s3d::SecondsF(0.1))
, m_csv(U"")
, m_buttonSave(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2) - 1.5 * (shogiBan_ / 9))),
               s3d::Vec2(komaDai_, shogiBan_ / 12))
, m_buttonQuit(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))),
               s3d::Vec2(komaDai_, shogiBan_ / 12)) {
    if (getData().gameState == State::Title) {
        MoveFirst();
        return;
    }

    MoveLast();
}

void Replay::update() {
    m_firstButtonTransition.update(m_firstButton.leftPressed());
    m_prevButtonTransition.update(m_prevButton.leftPressed());
    m_nextButtonTransition.update(m_nextButton.leftPressed());
    m_lastButtonTransition.update(m_lastButton.leftPressed());

    if (m_firstButton.mouseOver() || m_prevButton.mouseOver() || m_nextButton.mouseOver() || m_lastButton.mouseOver() || m_buttonSave.mouseOver() || m_buttonQuit.mouseOver()) {
        s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
    }

    if (m_prevButton.leftClicked()) {
        MovePrevious();
        return;
    }

    if (m_nextButton.leftClicked()) {
        MoveNext();
        return;
    }

    if (m_firstButton.leftClicked()) {
        MoveFirst();
        return;
    }

    if (m_lastButton.leftClicked()) {
        MoveLast();
        return;
    }

    if (m_buttonQuit.leftClicked()) {
        m_saveInitNum = 0;
        if (getData().gameState != State::Title) {
            SaveFile(U"match");
        }
        changeScene(State::Title);
        return;
    }

    if (m_buttonSave.leftClicked()) {
        m_saveInitNum = m_moveNum;
        SaveFile(U"kifu");
        m_isSaveFile = true;
        return;
    }
}

void Replay::SaveFile(s3d::StringView str_) {
    // テキストデータのString型をuint32型にするための変換器
    s3d::HashTable<uint32, s3d::String> toStrData;

    toStrData.emplace(Sfu, U"Sfu");
    toStrData.emplace(Sto, U"Sto");
    toStrData.emplace(Sky, U"Sky");
    toStrData.emplace(Sny, U"Sny");
    toStrData.emplace(Ske, U"Ske");
    toStrData.emplace(Snk, U"Snk");
    toStrData.emplace(Sgi, U"Sgi");
    toStrData.emplace(Sng, U"Sng");
    toStrData.emplace(Ski, U"Ski");
    toStrData.emplace(Ska, U"Ska");
    toStrData.emplace(Sum, U"Sum");
    toStrData.emplace(Shi, U"Shi");
    toStrData.emplace(Sry, U"Sry");
    toStrData.emplace(Sou, U"Sou");

    toStrData.emplace(Efu, U"Efu");
    toStrData.emplace(Eto, U"Eto");
    toStrData.emplace(Eky, U"Eky");
    toStrData.emplace(Eny, U"Eny");
    toStrData.emplace(Eke, U"Eke");
    toStrData.emplace(Enk, U"Enk");
    toStrData.emplace(Egi, U"Egi");
    toStrData.emplace(Eng, U"Eng");
    toStrData.emplace(Eki, U"Eki");
    toStrData.emplace(Eka, U"Eka");
    toStrData.emplace(Eum, U"Eum");
    toStrData.emplace(Ehi, U"Ehi");
    toStrData.emplace(Ery, U"Ery");
    toStrData.emplace(Eou, U"Eou");

    m_saveFileName = U"";
    m_csv = s3d::CSVData(m_saveFileName);
    if (!m_csv) {
        m_saveFileName = U"data/" + str_ + U"/" + s3d::Date::Today().format(U"yyyy_M_d_") + s3d::ToString(1) + U".csv";
        for (uint32 num{2}; s3d::FileSystem::Exists(m_saveFileName); ++num) {
            m_saveFileName = U"data/" + str_ + U"/" + s3d::Date::Today().format(U"yyyy_M_d_") + s3d::ToString(num) + U".csv";
        }

        s3d::Serializer<s3d::BinaryWriter>{m_saveFileName}(0);
        m_csv = s3d::CSVData(m_saveFileName);
        if (!m_csv) {
            s3d::Print << U"読み込めませんでした。";
            return;
        }
    }

    m_csv.clear();

    for (uint32 tesu{m_saveInitNum}; tesu < getData().stackBoradSquares.size(); ++tesu) {
        for (uint32 y{}; y < 9; ++y) {
            for (uint32 x{}; x < 9; ++x) {
                const auto komaStr = toStrData.find(getData().stackBoradSquares[tesu][9 * y + x].GetKomaType());
                if (komaStr == toStrData.end()) {
                    m_csv.write(U"Emp");
                    continue;
                }

                m_csv.write(komaStr.value());
            }

            m_csv.newLine();
        }

        s3d::Array<s3d::String> motigomas;

        for (const auto [i, havingSelfKoma] : s3d::IndexedRef(getData().stackHavingSelf[tesu])) {
            for (const auto& motigoma : havingSelfKoma) {
                const auto komaStr = toStrData.find((i + 1) + Self);
                if (komaStr == toStrData.end()) {
                    continue;
                }
                motigomas.emplace_back(komaStr.value());
            }
        }
        for (const auto [i, havingEnemyKoma] : s3d::IndexedRef(getData().stackHavingEnemy[tesu])) {
            for (const auto& motigoma : havingEnemyKoma) {
                const auto komaStr = toStrData.find((i + 1) + Enemy);
                if (komaStr == toStrData.end()) {
                    continue;
                }
                motigomas.emplace_back(komaStr.value());
            }
        }

        s3d::String writeStr;
        for (const auto [i, str] : s3d::IndexedRef(motigomas)) {
            writeStr += str;
            if (i < motigomas.size() - 1) {
                writeStr += U",";
            }
        }
        m_csv.write(writeStr);
        m_csv.newLine();

        if (!getData().stackPlacedPart[tesu].has_value() || tesu == m_saveInitNum) {
            m_csv.newLine();
            continue;
        }

        m_csv.write(9 - getData().stackPlacedPart[tesu].value().GetKomaCoodinate().x);
        m_csv.write(getData().stackPlacedPart[tesu].value().GetKomaCoodinate().y - 1);

        m_csv.newLine();
    }

    m_csv.save(m_saveFileName);
}

void Replay::draw() const {
    m_shogiBan.draw(s3d::Palette::Burlywood);
    m_komaDaiSelf.draw(s3d::Palette::Burlywood);
    m_komaDaiEnemy.draw(s3d::Palette::Burlywood);

    m_firstButton.draw(s3d::Palette::White).draw(s3d::ColorF(s3d::Palette::Aqua, m_firstButtonTransition.value())).drawFrame(1, s3d::Palette::Black);
    m_prevButton.draw(s3d::Palette::White).draw(s3d::ColorF(s3d::Palette::Aqua, m_prevButtonTransition.value())).drawFrame(1, s3d::Palette::Black);
    m_nextButton.draw(s3d::Palette::White).draw(s3d::ColorF(s3d::Palette::Aqua, m_nextButtonTransition.value())).drawFrame(1, s3d::Palette::Black);
    m_lastButton.draw(s3d::Palette::White).draw(s3d::ColorF(s3d::Palette::Aqua, m_lastButtonTransition.value())).drawFrame(1, s3d::Palette::Black);

    m_buttonSave.draw(s3d::Palette::White);
    m_buttonSave.drawFrame(1.0, s3d::Palette::Black);
    m_buttonQuit.draw(s3d::Palette::White);
    m_buttonQuit.drawFrame(1.0, s3d::Palette::Black);

    s3d::FontAsset(U"Menu")(U"今の局面を保存").drawAt(m_buttonSave.center(), s3d::Palette::Black);
    s3d::FontAsset(U"Menu")(U"タイトルに戻る").drawAt(m_buttonQuit.center(), s3d::Palette::Black);

    s3d::TextureAsset(U"firstButton").drawAt(m_firstButton.center(), s3d::Palette::Black);
    s3d::TextureAsset(U"prevButton").drawAt(m_prevButton.center(), s3d::Palette::Black);
    s3d::TextureAsset(U"nextButton").drawAt(m_nextButton.center(), s3d::Palette::Black);
    s3d::TextureAsset(U"lastButton").drawAt(m_lastButton.center(), s3d::Palette::Black);

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

    if (m_isSaveFile) {
        s3d::FontAsset(U"Explain")(U"保存しました。").drawAt(s3d::Scene::CenterF().movedBy(0, 300), s3d::Palette::Black);
    }
}