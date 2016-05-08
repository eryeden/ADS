#include <iostream>
#include <vector>
#include <memory>

#include <segments.hpp>


using namespace ADS;
using namespace std;


template <typename T>
Segment::Segment()
        : is_enable(true)
        , id(0)
        , no_segments(0)
{
  ;
}

template <typename T>
Segment::Segment(unsigned int no_)
        : Segment()
        , no_segments(no_)
{
    ;
}


template <typename T>
AdsManager::AdsManager()
        : t(0) //経過時間
{
    ;
}

template <typename T>
unsigned long AdsManager::AddSegment(std::shared_ptr<Segment<T>> segment_) {
    //セグメントの挿入
    segments.push_back(segment_);

    //状態量ベクトルの確保 rvalue タイプ セグメントの個数分だけTを確保、それをメインストレージに代入
    R.push_back(vector<T>(segment_->GetNo()));

    //パラメータベクトルの確保 セグメントの個数分だけパラメータベクトルを確保
    P.push_back(vector<vector<double>>(segment_->GetNo()));

    //pushbackなので要素数だけ返す
    return segments.size();
}

template <typename T>
unsigned long AdsManager::AddSegment(std::shared_ptr<Segment<T>> segment_, unsigned int no_segments_) {
    //要素数の設定だけやって、あと一緒
    segment_->SetNo(no_segments_);

    return AddSegment(segment_);
}



//積分計算ルンゲクッタ
/*

ルンゲクッタメモ

k1 = f(xn, tn)
k2 = f(xn + dt/2 * k1, tn + dt/2)
k3 = f(xn + dt/2 * k2, tn + dt/2)
k4 = f(xn + dt   * k3, tn + dt  )

xn+1 = xn + dt/6 * (k1 + k2 + k3 + k4)

処理の流れとしては

まず、セグメントシステムごとのループとする
for(seg = segments){

#1 auto z = g(t, P, R) としてグローバルエフェクトの計算
#2 セグメント要素分だけループする
>for(elem = no_segments){
>カウンタiを使って
>
>}

}



*/

template <typename T>
void AdsManager::Update() {
  
  

}

