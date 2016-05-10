#include <iostream>
#include <vector>
#include <memory>

#include <segments.hpp>

using namespace ADS;
using namespace std;

template<typename T>
TreeState::TreeState()
{
    ;
}

template<typename T>
TreeState::TreeState(const std::vector<std::vector<T> >& in_)
        :R(in_)
{
    ;
}

template<typename T>
TreeState::TreeState(const TreeState& in_)
        :R(in_.R)
{
    ;
}


template<typename T>
const TreeState<T> TreeState::operator*(double a_) const
{
    TreeState<T> out;
    out.R = R;
    for (auto itr_in = R.begin(), itr_out = out.R.begin(); itr_in!=R.end(); ++itr_in, ++itr_out
            ) {
        for (auto iitr = itr_in->begin(), oitr = (*itr_out).begin()
                ; iitr!=*itr_in->begin()
                ; ++iitr, ++oitr) {
            (*oitr) = a_*(*iitr);
        }
    }
    return out;
}

template<typename T>
const TreeState<T> TreeState::operator+(const TreeState<T>& in_) const
{
    auto out = in_;
    //サイズチェック TODO:厳密でない
    if ((R.size()!=in_.R.size())) {
        cout << "Size of R1 is not equal to R2" << endl;
        return in_;
    }

    for (auto litr = R.begin(), ritr = in_.R.begin(), oitr = out.R.begin();
         litr!=R.end();
         ++litr, ++ritr, ++oitr) {
        for (auto lt = litr->begin(), rt = (*ritr).begin(), ot = (*oitr).begin();
             lt!=litr->end();
             ++lt, ++rt, ++ot) {
            (*ot) = lt+rt;
        }
    }

    return out;
}

template<typename T>
const TreeState<T> TreeState::operator-(const TreeState<T>& in_) const
{
    auto out = in_;
    //サイズチェック TODO:厳密でない
    if ((R.size()!=in_.R.size())) {
        cout << "Size of R1 is not equal to R2" << endl;
        return in_;
    }

    for (auto litr = R.begin(), ritr = in_.R.begin(), oitr = out.R.begin();
         litr!=R.end();
         ++litr, ++ritr, ++oitr) {
        for (auto lt = litr->begin(), rt = (*ritr).begin(), ot = (*oitr).begin();
             lt!=litr->end();
             ++lt, ++rt, ++ot) {
            (*ot) = lt-rt;
        }
    }

    return out;
}

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
    R.GetTree().push_back(vector<T>(segment_->GetNo()));

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

 Rk1 ~ Rk4の計算ができたので、
 Rn+1 = Rn + dt/6 * (Rk1 + Rk2 + Rk3 + Rk4)

 として次のステップの状態量を決める

 とすれば、いま使ってるツリー状の状態構造
 //状態量ツリー
 std::vector<std::vector<T> > R; において
 ツリーにおける演算子を定義したほうが楽



*/

template<typename T>
void AdsManager::Update()
{

    //出力先
    TreeState R_out = R;
    TreeState Rk1(R), Rk2(R), Rk3(R), Rk4(R);

    //グローバルエフェクトバッファ
    vector<vector<double> > z_tmp;

    //    k1 = f(xn, tn)
    //    Rn = xnとなる
    //#1 すべてのセグメント分z = g(Rn,t,P)を計算
    //#2 すべてのセグメント分f(t, P, z, Rn, idx)を計算 -> Rk1
    /*
     * 全セグメント分のzを計算
     * */
    for(auto seg = begin(segments); seg != end(segments); ++seg){
        z_tmp.push_back((*seg)->g(t, P, R));
    }

    /*
     * 全セグメント分のRk1を計算
     */
    for(auto seg = begin(segments)
        , rk = begin(Rk1.GetTree())
        , r = begin(R.GetTree())
        , z = begin(z_tmp)
            ; seg != end(segments)
            ; ++seg, ++rk, ++r, ++z
            ){

        //セグメント内
        for(auto rk_itr = rk->begin(), r_itr = r->begin()
                ; rk_itr != rk->end()
                ; ++rk_itr, ++r_itr
                ){
            rk_itr = (*seg)->f(t, P, (*z), R, distance(r->begin(), r_itr));
        }
    }

    //k2 = f(xn + dt/2 * k1, tn + dt/2)
    //#1 Rd = Rn + Rk1*dt/2 を計算
    //#2 Rdに基づきzを計算z = g(Rd, t+dt/2, P)
    //#3 f(t + dt/2, P, z, Rd, idx) -> Rk2
    TreeState Rd(Rk1 + (Rk1 * (dt*0.5)));

    for(auto seg = begin(segments), z = begin(z_tmp)
            ; seg != end(segments)
            ; ++seg, ++z
            ){
        (*z) = (*seg)->g(t, P, Rd);
    }







}

