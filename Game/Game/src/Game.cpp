
# include "Game.hpp"

Ban::Ban(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const double shogiBan_, const double komaDai_) noexcept
    : m_shogiBan(Arg::center(Scene::CenterF()), shogiBan_)
    , m_komaDaiSelf(Arg::center
        (Scene::CenterF()
            .movedBy(shogiBan_/2+10+komaDai_/2, (shogiBan_/2-komaDai_)+komaDai_/2))
        , komaDai_)
    , m_komaDaiEnemy(Arg::center
        (Scene::CenterF()
            .movedBy(-(shogiBan_/2+10+komaDai_/2), -((shogiBan_/2-komaDai_)+komaDai_/2)))
        , komaDai_)
    , m_turn(Turn::Player)
{
    // １マスの大きさ
    const double squareSize = shogiBan_ / 9;
    
    // 盤面に駒を設置
    for (size_t y = 0; y < 9; ++y) {
        for (size_t x = 0; x < 9; ++x) {
            m_boardSquares << KomaSquare(
                m_shogiBan.tl().x + x * squareSize
                , m_shogiBan.tl().y + y * squareSize
                , squareSize
                , static_cast<KomaType>(iniKyokumen_[y][x])
                , Vec2(x+1, y+1)
            );
        }
    }
}

// GameクラスのUpdate()で呼び出すメンバ関数
void Ban::Update() {
    // 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を選んで手に持つ処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                continue;
            }
            if (square.GetKomaType() == Empty) {
                break;
            }
            
            // もしも自分の駒でないもしくは自分のターンでない場合、手に持つ事はできない
            if ((GetTurn() == Turn::Player && square.GetKomaType() > Enemy)
                || (GetTurn() == Turn::Enemy && square.GetKomaType() <= Enemy)) {
                    break;
            }

            m_holdHand.emplace(square);
            square.ChangeKomaType(Emp);
            break;
        }
        
        // クリックしてない場合は次の処理に
        if (!square.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            continue;
        }
        
        // 置く場所に駒が存在してた場合、
        // その駒を駒台に置くかどうかを見極める
        if (square.GetKomaType() != Empty) {
            AddHoldKoma(square);
            break;
        }
        
        if (m_holdHand.value().IsChangeCoodinate(square)) {
            ChangeCurrentTurn();
        }
        
        // 置く場所に何もなかったら、持ってる駒を置く
        square.ChangeKomaType(m_holdHand.value().GetKomaType());
        m_holdHand.reset();
        break;
    }
    
    // 駒台から取った駒を元の駒台の位置に戻す処理
    // いわゆるキャンセル処理
    if (m_holdHand.has_value()) {
        // 持ってる駒が盤面から取った駒なら、早期リターン
        if (m_holdHand.value().GetKomaState() == KomaState::Ban) {
            return;
        }
        // 何もクリックしてなかったら、処理リターン
        if (!m_komaDaiSelf.leftClicked()
            && !m_komaDaiEnemy.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            return;
        }
        
        if (m_komaDaiSelf.leftClicked()) {
            m_havingSelfKoma << KomaSquare(
                KomaPos::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1]
                , m_komaDaiSelf.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Vec2(10, 10)
            );
        }
        else {
            m_havingEnemyKoma << KomaSquare(
                KomaPos::enemyDaiPoses[m_holdHand.value().GetKomaType() - Enemy - 1]
                , m_komaDaiEnemy.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Vec2(10, 10)
            );
        }
        
        m_holdHand.reset();
        return;
    }
    
    // プレイヤーの駒台から駒を取る処理
    for (auto i : step(m_havingSelfKoma.size())) {
        if (!m_havingSelfKoma[i].leftClicked()) {
            continue;
        }
        m_holdHand.emplace(m_havingSelfKoma[i]);
        m_havingSelfKoma.remove_at(i);
        return;
    }
    
    // 敵の駒台から駒を取る処理
    for (auto i : step(m_havingEnemyKoma.size())) {
        if (!m_havingEnemyKoma[i].leftClicked()) {
            continue;
        }
        m_holdHand.emplace(m_havingEnemyKoma[i]);
        m_havingEnemyKoma.remove_at(i);
        return;
    }
}

void Ban::Draw() const {
    m_shogiBan.draw(Palette::Burlywood);
    m_komaDaiSelf.draw(Palette::Burlywood);
    m_komaDaiEnemy.draw(Palette::Burlywood);
    
    for (auto &square : m_boardSquares) {
        square.drawFrame(2, Palette::Black);
        if (square.mouseOver()) {
            square.draw(ColorF(Palette::White, 0.5f));
        }
        
        square.Draw();
    }
    
    for (auto& selfDai : m_havingSelfKoma) {
        selfDai.Draw();
    }
    for (auto& enemyDai : m_havingEnemyKoma) {
        enemyDai.Draw();
    }
    
    if (m_holdHand.has_value()) {
        m_holdHand.value().Draw();
    }
}

void Ban::AddHoldKoma(KomaSquare& koma_) {
    if ((koma_.GetKomaType() <= Enemy && m_holdHand.value().GetKomaType() <= Enemy)
        || (koma_.GetKomaType() > Enemy && m_holdHand.value().GetKomaType() > Enemy)) {
        return;
    }
    
    uint32 komaType = koma_.GetKomaType();
    koma_.ChangeKomaType(m_holdHand.value().GetKomaType());
    m_holdHand.reset();
    ChangeCurrentTurn();
    
    // 敵駒にする場合
    if (komaType <= Enemy) {
        komaType = komaType - Self + Enemy;
        m_havingEnemyKoma << KomaSquare(
            KomaPos::enemyDaiPoses[komaType - Enemy - 1]
            , m_komaDaiEnemy.w / 4
            , static_cast<KomaType>(komaType)
            , KomaState::Dai
            , Vec2(10, 10)
        );
        return;
    }
    
    // 自駒にする場合
    komaType = komaType - Enemy + Self;
    m_havingEnemyKoma << KomaSquare(
        KomaPos::selfDaiPoses[komaType - Self - 1]
        , m_komaDaiSelf.w / 4
        , static_cast<KomaType>(komaType)
        , KomaState::Dai
        , Vec2(10, 10)
    );
    return;
}

Game::Game(const InitData& init)
	: IScene(init)
    , m_ban(HirateBan) {}

void Game::update()
{
    m_ban.Update();
}

void Game::draw() const
{
    m_ban.Draw();
}
