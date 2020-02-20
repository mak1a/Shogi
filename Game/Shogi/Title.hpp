
# pragma once
# include "Common.hpp"

// タイトルシーン
class Title : public MyApp::Scene {
private:
	Rect m_soloMatchButton;
	Transition m_soloMatchTransition;

	Rect m_twoPlayerMatchButton;
	Transition m_twoPlayerMatchTransition;

	Rect m_exitButton;
	Transition m_exitTransition;
public:
	Title (const InitData& init);

	void update () override;

	void draw () const override;
};
