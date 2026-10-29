/*==========================================================================
    ピンの管理者 [pinManager.h]

    複数のピンを管理し、更新・描画・衝突処理などを行うクラスを宣言する。
==========================================================================*/

#pragma once

#include <vector>
#include "pins.h"
#include "bowlingBall.h"

class PinManager
{
public:
    // ピンと効果音を初期化する。
    void Initialize();

    // ピンの更新とボール・ピン同士の衝突判定を行う。
    void Update(float dt, BowlingBall& ball);

    // 管理しているすべてのピンを描画する。
    void Draw();

    // 管理に使用したリソースを解放する。
    void Finalize();

    // 指定位置を基準に10本のピンを配置する。
    void SetupBowlingPins(const DirectX::XMFLOAT3& headPinPos);

    // ピンを初期配置に戻す。
    void ResetPins();

    // 指定位置に新しいピンを追加する。
    void AddPin(const DirectX::XMFLOAT3& pos);

    // ボールとピンの衝突を処理する。
    void ResolveBallPinHit(BowlingBall& ball, Pins& pin);

    // 死亡扱いではないピンの数を取得する。
    int GetRemainingPinCount() const;

    // 死亡扱いのピンを画面外へ移動させる。
    void RemoveDeadPins();

    // すべてのピンを再配置する。
    void DestroyAndRecreatePins();

private:
    // 2本のピン同士の衝突を処理する。
    void ResolvePinPin(Pins& a, Pins& b);

    // 管理対象のピンを格納する配列。
    std::vector<Pins> m_pins;
};