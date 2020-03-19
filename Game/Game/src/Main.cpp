
# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"
# include "GameAI.hpp"

void Main() {
	// タイトルを設定
	Window::SetTitle(U"将棋");
    
    // ウィンドウの大きさを設定
    Window::Resize(1280, 720);

	// 背景色を設定
	Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

	// 使用するフォントアセットを登録
	FontAsset::Register(U"Title", 120, Typeface::Heavy);
	FontAsset::Register(U"Menu", 30, Typeface::Regular);
	FontAsset::Register(U"Explain", 36, Typeface::Light);
	FontAsset::Register(U"Result", 150, Typeface::Regular);
    
    TextureAsset::Register(U"Fu", Resource(U"textures/komas/syougi14_fuhyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"To", Resource(U"textures/komas/syougi15_tokin.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Ou", Resource(U"textures/komas/syougi01_ousyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Gyoku", Resource(U"textures/komas/syougi02_gyokusyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Hi", Resource(U"textures/komas/syougi03_hisya.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Ryu", Resource(U"textures/komas/syougi04_ryuuou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kaku", Resource(U"textures/komas/syougi05_gakugyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Uma", Resource(U"textures/komas/syougi06_ryuuma.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kin", Resource(U"textures/komas/syougi07_kinsyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Gin", Resource(U"textures/komas/syougi08_ginsyou.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariGin", Resource(U"textures/komas/syougi09_narigin.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Keima", Resource(U"textures/komas/syougi10_keima.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariKeima", Resource(U"textures/komas/syougi11_narikei.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kyosha", Resource(U"textures/komas/syougi12_kyousya.png"), AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariKyosha", Resource(U"textures/komas/syougi13_narikyou.png"), AssetParameter::LoadImmediately());

	AudioAsset::Register(U"Piece", Resource(U"audios/piece1.m4a"), AssetParameter::LoadImmediately());
	
	// シーンと遷移時の色を設定
	MyApp manager;
	manager
		.add<Title>(State::Title)
		.add<Select>(State::Select)
		.add<Game>(State::Game)
		.add<GameAI>(State::GameAI)
		.setFadeColor(ColorF(1.0));

	//manager.init(State::Select);

	while (System::Update()) {
		if (!manager.update()) {
			break;
		}
	}
}
