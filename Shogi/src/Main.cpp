
#include "Common.hpp"
#include "Title.hpp"
#include "Match.hpp"
#include "OnlineMatch.hpp"
#include "PlayAlone.hpp"
#include "Replay.hpp"

void Main() {
    // タイトルを設定
    s3d::Window::SetTitle(U"将棋");

    // ウィンドウの大きさを設定
    s3d::Window::Resize(1280, 720);

    // 背景色を設定
    s3d::Scene::SetBackground(s3d::ColorF(0.2, 0.8, 0.4));

    // 使用するフォントアセットを登録
    s3d::FontAsset::Register(U"Title", 120, s3d::Typeface::Heavy);
    s3d::FontAsset::Register(U"Menu", 30, s3d::Typeface::Regular);
    s3d::FontAsset::Register(U"Warning", 36, s3d::Typeface::Light);
    s3d::FontAsset::Register(U"Explain", 20, s3d::Typeface::Regular);
    s3d::FontAsset::Register(U"Result", 150, s3d::Typeface::Regular);
    s3d::FontAsset::Register(U"YesNo", 20, s3d::Typeface::Light);

    s3d::TextureAsset::Register(U"Fu", s3d::Resource(U"textures/komas/syougi14_fuhyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"To", s3d::Resource(U"textures/komas/syougi15_tokin.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Ou", s3d::Resource(U"textures/komas/syougi01_ousyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Gyoku", s3d::Resource(U"textures/komas/syougi02_gyokusyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Hi", s3d::Resource(U"textures/komas/syougi03_hisya.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Ryu", s3d::Resource(U"textures/komas/syougi04_ryuuou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Kaku", s3d::Resource(U"textures/komas/syougi05_gakugyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Uma", s3d::Resource(U"textures/komas/syougi06_ryuuma.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Kin", s3d::Resource(U"textures/komas/syougi07_kinsyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Gin", s3d::Resource(U"textures/komas/syougi08_ginsyou.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"NariGin", s3d::Resource(U"textures/komas/syougi09_narigin.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Keima", s3d::Resource(U"textures/komas/syougi10_keima.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"NariKeima", s3d::Resource(U"textures/komas/syougi11_narikei.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"Kyosha", s3d::Resource(U"textures/komas/syougi12_kyousya.png"), s3d::AssetParameter::LoadImmediately());
    s3d::TextureAsset::Register(U"NariKyosha", s3d::Resource(U"textures/komas/syougi13_narikyou.png"), s3d::AssetParameter::LoadImmediately());
    // s3d::TextureAsset::Register(U"Standby", s3d::Resource(U"textures/match/kiban.jpg"), s3d::AssetParameter::LoadImmediately());

    s3d::AudioAsset::Register(U"Piece", s3d::Resource(U"audios/piece1.m4a"), s3d::AssetParameter::LoadImmediately());

    const s3d::INIData ini(s3d::Resource(U"config/appIDString.ini"));

    // シーンと遷移時の色を設定
    MyApp manager(shogi::ChangeAppIDString(ini[U"Photon.appID"]), L"1.80");
    manager.add<Title>(State::Title)
        .add<Match>(State::Match)
        .add<Select>(State::Select)
        .add<OnlineMatch>(State::OnlineMatch)
        .add<PlayAlone>(State::PlayAlone)
        .add<Replay>(State::Replay)
        .setFadeColor(s3d::ColorF(1.0));

    // manager.init(State::Select);

    // ハッシュ値の初期化
    for (const size_t i : s3d::step(48)) {
        for (const size_t pos : s3d::Range(11, 99)) {
            HashSeeds[i][pos] = s3d::RandomUint64();
        }
    }
    for (const size_t i : s3d::Range(17, 39)) {
        for (const size_t maisuu : s3d::step(19)) {
            HashHandSeeds[i][maisuu] = s3d::RandomUint64();
        }
    }

    while (s3d::System::Update()) {
        if (!manager.update()) {
            break;
        }
    }
}
