#ifdef COMMON_SEGMENTS_HPP
#define COMMON_SEGMENTS_HPP

#include <iostream>
#include <vector>


/*

セグメントクラスの設計

このクラスは自立分散システム？（ADS）における自立個を表現する

まず、自立個として必要なことは、
・自分の状態を保持する自己状態ベクトルr
を保持し、
・自立分散システム全体の状態を表現するシステム状態ベクトルR
・系全体の状態Rから得られるベクトル？z
を受け取ることで、次の自己状態ベクトルをえることである。

 */

namespace ADS{

  // ADS内のセグメント挙動を記述するインターフェイスクラス？　これを継承して具体的なセグメントのシステムを記述する
  template <typename T> class SegmentImplementation{

  public:

    //更新関数 1タイムステップ後の自立個における自己状態ベクトルを計算する
    // 入力として、システム系全体の状態ベクトルR、系全体から定まる拘束？ベクトルz、を受け取る
    virtual std::vector<T> Update(const std::vector<T> &R, const std::vector<T> &z) = 0;
    // 入力として、系の状態ベクトルzのみを受け取る　実際システムを書くとすればこっちの方が多い気がする
    virtual std::vector<T> Update(const std::vector<T> &R) = 0;

    // 入力として、系の状態ベクトルを受け取り、系全体から定まるベクトルを返す
    virtual std::vector<T> GlobalEffects(const std::vector<T> &R) = 0;
    

    // ID設定関数 インラインインプリメント
    void SetID(const unsigned int id_){id = id_;}
    
  private:
    unsigned int id;		// シミュレーションマネージャ登録時にふられるID
    
  };

  //ここでは各々のセグメントの時間発展とそれらの組み込み、初期状態設定を行う　継
  template <typename T> class AdsManager{
  public:

    //セグメントの登録を行う、セグメントIDを返す
    unsigned int InsertSegment(SegmentImplementation<T> *);

    
    
  private:
    std::vector<SegmentImplementation<T> *> segments;
    
  }

  
  
  
};

#endif	// COMMON_SEGMENTS_HPP













































