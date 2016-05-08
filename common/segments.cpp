#include <iostream>
#include <vector>
#include <memory>

#include <segments.hpp>

using namespace ADS;
using namespace std;

template<typename T>
Segment::Segment()
        :is_enable(true), id(0), no_segments(0)
{
    ;
}

template<typename T>
Segment::Segment(unsigned int no_)
        :Segment(), no_segments(no_)
{
    ;
}

template<typename T>
AdsManager::AdsManager()
        :t(0) //経過時間
{
    ;
}

template<typename T>
unsigned long AdsManager::AddSegment(std::shared_ptr<Segment<T>> segment_)
{
    //セグメントの挿入
    segments.push_back(segment_);

    //状態量ベクトルの確保 rvalue タイプ セグメントの個数分だけTを確保、それをメインストレージに代入
    R.push_back(vector<T>(segment_->GetNo()));

    //パラメータベクトルの確保 セグメントの個数分だけパラメータベクトルを確保
    P.push_back(vector<vector<double>>(segment_->GetNo()));

    //push backなので要素数だけ返す
    return segments.size();
}

template<typename T>
unsigned long AdsManager::AddSegment(std::shared_ptr<Segment<T>> segment_, unsigned int no_segments_)
{
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

 k1 = f(xn, tn)
 Rn = xnとなる
 #1 すべてのセグメント分z = g(Rn,t,P)を計算
 #2 すべてのセグメント分f(t, P, z, Rn, idx)を計算 -> Rk1

 k2 = f(xn + dt/2 * k1, tn + dt/2)
 #1 Rd = Rn + Rk1*dt/2 を計算
 #2 Rdに基づきzを計算z = g(Rd, t+dt/2, P)
 #3 f(t + dt/2, P, z, Rd, idx) -> Rk2

 k3 = f(xn + dt/2 * k2, tn + dt/2)
 #1 Rd = Rn + Rk2 * dt/2
 #2 Rdからzを計算 z = g(Rd, t+dt/2, P)
 #3 f(t + dt/2, P, z, Rd, idx) -> Rk3

 k4 = f(xn + dt   * k3, tn + dt  )
 #1 Rd = Rn + Rk3 * dt
 #2 Rdよりz = g(Rd, t + dt, P)
 #3 f(t + dt, P, z ,Rd, idx) -> Rk4



*/

template<typename T>
void AdsManager::Update()
{

    //出力先
    vector<vector<T> > R_out = R;


    //セグメントループ
    for (auto itr = segments.begin(); itr!=segments.end(); ++itr) {
        vector<T> R_pres = R[distance(segments.begin(), itr)];

        //k1
        auto k1 = R_pres;
        auto k2 = R_pres;
        auto k3 = R_pres;
        auto k4 = R_pres;


        //全体への影響はここで計算
        vector<double> z;
        z = (*itr)->g(t, P, R);

        //セグメントの要素数だけループ
        for (auto itr_s = R_pres.begin(), itr_k1 = k1.begin(); itr_s!=R_pres.end(); ++itr_s, ++itr_k1) {
            (*itr_k1) = (*itr)->f(t, P, z, R, distance(R_pres.begin(), itr_s));
        }


        //全体への影響はここで計算
        z = (*itr)->g(t, P, R);

        //セグメントの要素数だけループ
        for (auto itr_s = R_pres.begin(), itr_k1 = k1.begin(); itr_s!=R_pres.end(); ++itr_s, ++itr_k1) {
            (*itr_k1) = (*itr)->f(t, P, z, R, distance(R_pres.begin(), itr_s));
        }


    }

}

