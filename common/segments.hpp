#ifndef COMMON_SEGMENTS_HPP
#define COMMON_SEGMENTS_HPP

#include <iostream>
#include <vector>
#include <memory>
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

namespace ADS {

    // ADS内のセグメント挙動を記述するインターフェイスクラス？ これを継承して具体的なセグメントのシステムを記述する
    // 各個セグメントにおけるパラメータの設定が問題になる


    /*
     * バランス
     * 負荷配分
     *
     */

    //状態量ツリー
    template<typename T>
    class TreeState {
    public:

        TreeState();
        TreeState(const std::vector<std::vector<T> >&);
        TreeState(const TreeState&);


        //vector<vector <T>> の本体を返す
        std::vector<std::vector<T> >& GetTree() { return R; };
        //本体を更新
        //void SetTree(std::vector<std::vector<T> >& in_) {R = in_;};

        //RVOSによる最適化が働くのでそのまま帰すことにする
        //MOVEセマンティクスは考えない
        const TreeState<T> operator+(const TreeState<T>&) const;

        const TreeState<T> operator-(const TreeState<T>&) const;

        const TreeState<T> operator*(double) const;
        //const TreeState<T> operator/(double, const TreeState<T> &);

    private:

        //コア
        //状態量ツリー
        std::vector<std::vector<T> > R;

    };

    /*
     * セグメントクラスがやること
     *
     * １セグメントのシステムを書く
     * システムは
     * r_dot = f(t, P, z, R, idx)
     * (t:時間、P:系のパラメータ、z:系全体から定まるベクトル、R:系全体を示す状態ベクトル、idx:セグメントのINDEX)
     * として書かれる
     *
     * また、系全体から定まるベクトルzを計算する関数を書く
     * これは
     * z = g(t, P, R)
     * (t:時間、P:系のパラメータ、R:系全体を示す状態ベクトル)
     * として書かれる
     *
     * Rについて これはAdsManager内で管理される
     * Rは系全体の状態を示す、実質行列となる
     * セグメントiにおける状態を示すベクトルをr_iとすれば、Rは
     * R＝{r_1 r_2 ... r_n} (n個のセグメント)
     * となる
     * これはvectorで示すことにする
     *
     * Pについて これはAdsManager内で管理される
     * Pはこのセグメント群（おなじシステムで記述されるセグメントの集まり）すべてのパラメータを示す、多くの実装では行列になると思われる
     * セグメントiにおけるパラメータを示すベクトルをp_iとすれば、Pは
     * P={p_1 p_2 ... p_n} (n個のセグメント)
     * となる
     * なのでPはvectorで示すことになる
     *
     * */
    template<typename T>
    class Segment {

    public:

        Segment();

        Segment(unsigned int no_);

        //セグメントのシステム
//        * １セグメントのシステムを書く
//                * システムは
//        * r_dot = f(t, P, z, R, idx)
//                  * (t:時間、P:系のパラメータ、z:系全体から定まるベクトル、R:系全体を示す状態ベクトル、idx:セグメントのINDEX)
//        * として書かれる
        virtual T f(double t_
                , const std::vector<std::vector<std::vector<double> > >& P_
                , const std::vector<double>& z_
                , const std::vector<std::vector<T> >& R_
                , unsigned long idx_) = 0;

        virtual T f(double t_
                , const std::vector<std::vector<std::vector<double> > >& P_
                , const std::vector<double>& z_
                , const TreeState& R_
                , unsigned long idx_) = 0;


        // 入力として、系の状態ベクトルを受け取り、系全体から定まるベクトルを返す
//        * z = g(t, P, R)
//              * (t:時間、P:系のパラメータ、R:系全体を示す状態ベクトル)
//        * として書かれる
        virtual std::vector<double> g(double t_, const std::vector<std::vector<std::vector<double> > >& P_,
                const std::vector<std::vector<T> >& R_) = 0;

        virtual std::vector<double> g(double t_
                , const std::vector<std::vector<std::vector<double> > >& P_
                , const TreeState& R_) = 0;

        //このシステムで記述されるセグメント数の設定
        void SetNo(const unsigned int no_) { no_segments = no_; };

        //セグメント数の取得
        const unsigned int GetNo() { return no_segments; };

        // 組み込みセグメントIDの設定
        void SetID(const unsigned int id_) { id = id_; };

        //セグメントIDの取得
        const unsigned int GetID() { return id; };

        //セグメントの有効化、無効化
        void Enable() { is_enable = true; };

        void Disable() { is_enable = false; };

    private:

        unsigned int no_segments; //セグメント数

        unsigned int id;        // 組み込みセグメントID

        bool is_enable;         //有効化


    };

    //ここでは各々のセグメントの時間発展とそれらの組み込み、初期状態設定を行う
    /*
     * AdsManagerがホストする状態量としては
     *
     * 経過時間:t
     * 系の状態ベクトル:R
     * 系のパラメータ:P
     *
     * セグメントの実装から決まったこと
     * 系の状態ベクトルRはツリー構造をもつ
     * なので実装はR:vector<vector<T>>となる
     *
     * また、それぞれのセグメントにおけるパラメータ設定も、ツリー構造とした
     * なのでP:vector<vector<vector<double>>>となる
     *TODO: パラメータベクトルは使いやすそうなクラスかなんかにする
     *
     *
     * */

    template<typename T>
    class AdsManager {
    public:

        AdsManager();

        //セグメントの登録を行う、セグメントIDを返す
        /*
         * セグメントの登録を行う
         * 処理の流れとしては
         *
         * セグメントストレージにプッシュバック
         * 個数分の状態量、パラメータベクトルの確保を行う
         *
         * セグメントインデックスを返す
         *
         * */
        unsigned long AddSegment(std::shared_ptr<Segment<T> >);

        //セグメント数の設定も行う
        unsigned long AddSegment(std::shared_ptr<Segment<T> >, unsigned int no_segments_);


        //パラメータ設定、初期値設定インターフェース


        //各セグメントの状態を更新する
        /*
         * 全体の積分計算を行う
         * */
        void Update();

    private:

        //セグメント
        std::vector<std::shared_ptr<Segment<T> > > segments;
        //状態量ツリー
        //std::vector<std::vector<T> > R;
        TreeState R;
        //パラメータツリー
        std::vector<std::vector<std::vector<double> > > P;

        //経過時間
        double t;

        //タイムピッチ
        double dt;

    };


};

#endif	// COMMON_SEGMENTS_HPP













































