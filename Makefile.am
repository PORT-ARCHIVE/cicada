# ライセンスがGPLにならないようにする指定
AUTOMAKE_OPTIONS = foreign
# 実行ファイル名
bin_PROGRAMS = semicrf
# 静的ライブラリ
semicrf_LDFLAGS = -L/usr/local/lib
semicrf_LDADD = -lmecab -lstdc++
# ソースコード
semicrf_SOURCES = Main.cpp SemiCrf.hpp SemiCrf.cpp AppReqs.cpp

