# オセロプログラム
オセロをCUI上で実行するプログラム．対人版，CPU搭載版，AI搭載版の3種類がある．MacOSとUbuntu14.04で動作確認済み．

## 対人版（interpersonal_version.c）
1P vs 2Pの対戦．詳しくは[こちら](http://johoko.blog.fc2.com/blog-entry-18.html)．

![interpersonal](https://user-images.githubusercontent.com/11649428/116286251-f70cac00-a7c9-11eb-952b-685cc651ec51.png)

## CPU搭載版（random_cpu.c)
1P vs CPUの対戦．CPUは自分が置ける場所にランダムに石を置く．モード選択ができ，対人版と切り替えられる．詳しくは[こちら](http://johoko.blog.fc2.com/blog-entry-21.html)．

![random_cpu](https://user-images.githubusercontent.com/11649428/116291240-51f4d200-a7cf-11eb-8186-80349aa05595.png)

## AI搭載版（alpha_beta_search.c）
1P vs CPUの対戦．CPUはαβ法の結果を元に石を置く．モード選択ができ，対人版と切り替えられる．また，EASYモード（CPU搭載版）とNORMALモード（AI搭載版）と切り替えられる．詳しくは[こちら](http://johoko.blog.fc2.com/blog-entry-22.html)．

![alpha_beta_search](https://user-images.githubusercontent.com/11649428/116295521-258f8480-a7d4-11eb-9a3b-6b38dd312f63.png)
