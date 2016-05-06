#ifndef COMMON_SEGMENTS_HPP
#define COMMON_SEGMENTS_HPP

#include <iostream>
#include <vector>
#include <Eigen/Dense>


/*

セグメントクラスの設計

このクラスは自立分散システム？（ADS）における自立個を表現する

まず、自立個として必要なことは、
・自分の状態を保持する自己状態ベクトルr
を保持し、
・自立分散システム全体の状態を表現するシステム状態ベクトルR
・系全体の状態Rから得られるベクトル？z
を受け取ることで、次の自己状態ベクトルをえることである。


 ながれとしては

 各セグメントの
 システムの記述、パラメータの設定
 ・パラメータは同じシステムでも異なってくるのでシステム定義とは異なり、複数存在しなければならない
 ・複数のセグメントの状態、パラメータをそれぞれのセグメントにどのように渡すのか？



 まず、

 セグメントクラス
 含有として
 ・状態
 ・

 */

namespace ADS{

    // ADS内のセグメント挙動を記述するインターフェイスクラス？ これを継承して具体的なセグメントのシステムを記述する
    // 各個セグメントにおけるパラメータの設定が問題になる
    template <typename T> class SegmentImplementation{

    public:

        SegmentImplementation();

        //更新関数 1タイムステップ後の自立個における自己状態ベクトルを計算する
        // 入力として、システム系全体の状態ベクトルR、系全体から定まる拘束？ベクトルz、を受け取る
        virtual T Update(const std::vector<T> &R, const std::vector<T> &z) = 0;
        // 入力として、系の状態ベクトルzのみを受け取る 実際システムを書くとすればこっちの方が多い気がする
        virtual T Update(const std::vector<T> &R) = 0;

        // 入力として、系の状態ベクトルを受け取り、系全体から定まるベクトルを返す
        virtual T GlobalEffects(const std::vector<T> &R) = 0;


        // ID設定関数 インラインインプリメント
        void SetID(const unsigned int id_){id = id_;};

        //セグメントの有効化、無効化
        void Enable(){is_enable = true;};
        void Disable(){is_enable = false;};



    private:
        unsigned int id;		// シミュレーションマネージャ登録時にふられるID

        bool is_enable;         //有効化

        T r; //状態
        std::vector<double> param; //パラメータ
    };

    //ここでは各々のセグメントの時間発展とそれらの組み込み、初期状態設定を行う
    template <typename T> class AdsManager{
    public:

        AdsManager();

        //セグメントの登録を行う、セグメントIDを返す
        unsigned int InsertSegment(SegmentImplementation<T> *);

        //各セグメントにおける要素数の設定
        void SetNumberOfSegments(const unsigned int id_, const unsigned int number_of_elements_);

        //各セグメントの状態を更新する
        void update();



    private:

        std::vector<SegmentImplementation<T> *> segments;

    };



};

#endif	// COMMON_SEGMENTS_HPP













































