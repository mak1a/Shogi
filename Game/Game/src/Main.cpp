
//
// OpenSiv3D v0.4.1 ゲームテンプレート
//

//
// macOS では README.md と同じフォルダに
// include と lib をコピーしてください。
//

# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"

void Main()
{
	// （ウィンドウの閉じるボタンを押したときだけ終了する場合はコメントを外す）
	//System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	// タイトルを設定
	Window::SetTitle(U"将棋ゲーム");
    
    // ウィンドウの大きさを設定
    Window::Resize(1280, 720);

	// 背景色を設定
	Scene::SetBackground(ColorF(0.2, 0.8, 0.4));

	// 使用するフォントアセットを登録
	FontAsset::Register(U"Title", 120, U"font/AnnyantRoman/AnnyantRoman.ttf");
	FontAsset::Register(U"Menu", 30, Typeface::Regular);
	FontAsset::Register(U"Score", 36, Typeface::Bold);
    
    TextureAsset::Register(U"Fu", U"textures/komas/syougi14_fuhyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"To", U"textures/komas/syougi15_tokin.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Ou", U"textures/komas/syougi01_ousyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Gyoku", U"textures/komas/syougi02_gyokusyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Hi", U"textures/komas/syougi03_hisya.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Ryu", U"textures/komas/syougi04_ryuuou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kaku", U"textures/komas/syougi05_gakugyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Uma", U"textures/komas/syougi06_ryuuma.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kin", U"textures/komas/syougi07_kinsyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Gin", U"textures/komas/syougi08_ginsyou.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariGin", U"textures/komas/syougi09_narigin.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Keima", U"textures/komas/syougi10_keima.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariKeima", U"textures/komas/syougi11_narikei.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"Kyosha", U"textures/komas/syougi12_kyousya.png", AssetParameter::LoadImmediately());
    TextureAsset::Register(U"NariKyosha", U"textures/komas/syougi13_narikyou.png", AssetParameter::LoadImmediately());

	// シーンと遷移時の色を設定
	MyApp manager;
	manager
		.add<Title>(State::Title)
		.add<Game>(State::Game)
		.setFadeColor(ColorF(1.0));

	// （ゲームシーンから開始する場合はコメントを外す）
	manager.init(State::Game);

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
	}
}

//
// = クロスプラットフォーム開発の注意 =
//
// Xcode でソースコードを保存すると UTF-8 形式 (BOM 無し) になります。
// MSVC で　作業するときはソースコードを UTF-8 (BOM 付き) に直す必要があります。
//
//
// = アドバイス =
// Debug ビルドではプログラムの最適化がオフになります。
// 実行速度が遅いと感じた場合は Release ビルドを試しましょう。
// アプリをリリースするときにも、Release ビルドにするのを忘れないように！
//
// 思ったように動作しない場合は「デバッグの開始」でプログラムを実行すると、
// 出力ウィンドウに詳細なログが表示されるので、エラーの原因を見つけやすくなります。
//
//
// = お役立ちリンク =
//
// OpenSiv3D リファレンス
// https://siv3d.github.io/ja-jp/
//
// チュートリアル
// https://siv3d.github.io/ja-jp/tutorial/basic/
//
// よくある間違い
// https://siv3d.github.io/ja-jp/articles/mistakes/
//
// サポートについて
// https://siv3d.github.io/ja-jp/support/support/
//
// Siv3D Slack (ユーザコミュニティ) への参加
// https://siv3d.github.io/ja-jp/community/community/
//
// 新機能の提案やバグの報告
// https://github.com/Siv3D/OpenSiv3D/issues
//
