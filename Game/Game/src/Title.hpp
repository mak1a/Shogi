
# pragma once
# include "Common.hpp"

// タイトルシーン
class Title : public MyApp::Scene
{
private:

	Rect m_soloMatchButton = Rect(Arg::center = Scene::Center().movedBy(0, 0), 300, 60);
	Transition m_soloMatchTransition = Transition(0.4s, 0.2s);

	Rect m_twoPlayerMatchButton = Rect(Arg::center = Scene::Center().movedBy(0, 100), 300, 60);
	Transition m_twoPlayerMatchTransition = Transition(0.4s, 0.2s);

	Rect m_exitButton = Rect(Arg::center = Scene::Center().movedBy(0, 200), 300, 60);
	Transition m_exitTransition = Transition(0.4s, 0.2s);

public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;
};
