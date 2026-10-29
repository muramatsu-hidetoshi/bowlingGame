#pragma once

// アニメーションの変化の仕方を表すイージング関数の種類
// In：ゆっくり始まる
// Out：ゆっくり終わる
// InOut：始まりと終わりがゆっくり変化する
enum easing_functions
{
    EaseInSine,
    EaseOutSine,
    EaseInOutSine,

    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,

    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,

    EaseInQuart,
    EaseOutQuart,
    EaseInOutQuart,

    EaseInQuint,
    EaseOutQuint,
    EaseInOutQuint,

    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,

    EaseInCirc,
    EaseOutCirc,
    EaseInOutCirc,

    EaseInBack,
    EaseOutBack,
    EaseInOutBack,

    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,

    EaseInBounce,
    EaseOutBounce,
    EaseInOutBounce
};

// イージング関数への関数ポインタ型
// double型の値を受け取り、変化後のdouble型の値を返す
typedef double(*easingFunction)(double);

// 指定した種類に対応するイージング関数を取得する
easingFunction getEasingFunction(easing_functions function);