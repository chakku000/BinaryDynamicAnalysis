Pinを使用したバイナリ計装

- wrapper.cpp 
    - pthread_createを置き換えて親スレッドから子スレッドに値を渡す実験
    - 結果として，小さなコードでは動作するがそれなりに大きな規模のコードでテストしたときにデッドロックを起こす場合があるので，親から子にデータの受け渡しをするのに実用的な方法ではない

- test_tls.cpp
    - TLSを使用するテストコード
    - getTLSとsetTLSを用意しておくと便利という知見
    - 使い方は
        1. TLS_KEYを宣言
        2. tls_keyを初期化(*PIN_CreateThreadDataKey*)
        3. PIN_GetThreadData(スレッドキー,スレッドID)でデータの取得
            - データが設定されていなければNULLが返る
        4. PIN_SetThreadData(スレッドキー,データ,スレッドID)でデータの設定
