#include <iostream>
#include <vector>

#include <segments.hpp>


using namespace ADS;
using namespace std;



Segment::Segment()
        : is_enable(true)
        , id(0)
        , no_segments(0)
{
    ;
}

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



//積分計算
template <typename T>
void AdsManager::Update() {



}

