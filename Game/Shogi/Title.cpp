
# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene(init)
	, m_soloMatchButton(Arg::center(Scene::Center().movedBy(0, 0)), 300, 60)
	, m_twoPlayerMatchButton(Arg::center(Scene::Center().movedBy(0, 100)), 300, 60)
	, m_exitButton(Arg::center(Scene::Center().movedBy(0, 200)), 300, 60)
	, m_soloMatchTransition(0.4s, 0.2s)
	, m_twoPlayerMatchTransition(0.4s, 0.2s)
	, m_exitTransition(0.4s, 0.2s) {}

void Title::update() {
	m_soloMatchTransition.update(m_soloMatchButton.mouseOver());
	m_twoPlayerMatchTransition.update(m_twoPlayerMatchButton.mouseOver());
	m_exitTransition.update(m_exitButton.mouseOver());

	if (m_soloMatchButton.mouseOver() || m_exitButton.mouseOver() || m_twoPlayerMatchButton.mouseOver()) {
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (m_soloMatchButton.leftClicked()) {
		changeScene(State::GameAI);
	}

	if (m_twoPlayerMatchButton.leftClicked()) {
		changeScene(State::Game);
	}

	if (m_exitButton.leftClicked()) {
		System::Exit();
	}
}

void Title::draw() const {
	const String titleText = U"è´ä˚";
	const Vec2 center(Scene::Center().x, 120);
	FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), ColorF(0.0, 0.5));
	FontAsset(U"Title")(titleText).drawAt(center);

	m_soloMatchButton.draw(ColorF(1.0, m_soloMatchTransition.value())).drawFrame(2);
	m_twoPlayerMatchButton.draw(ColorF(1.0, m_twoPlayerMatchTransition.value())).drawFrame(2);
	m_exitButton.draw(ColorF(1.0, m_exitTransition.value())).drawFrame(2);

	FontAsset(U"Menu")(U"Ç–Ç∆ÇËÇ≈").drawAt(m_soloMatchButton.center(), ColorF(0.25));
	FontAsset(U"Menu")(U"Ç”ÇΩÇËÇ≈").drawAt(m_twoPlayerMatchButton.center(), ColorF(0.25));
	FontAsset(U"Menu")(U"Ç®ÇÌÇÈ").drawAt(m_exitButton.center(), ColorF(0.25));

	Rect(0, 500, Scene::Width(), Scene::Height() - 500)
		.draw(Arg::top = ColorF(0.0, 0.0), Arg::bottom = ColorF(0.0, 0.5));

}
