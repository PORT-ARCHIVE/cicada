# ライセンスがGPLにならないようにする指定
AUTOMAKE_OPTIONS = foreign
# 実行ファイル名
bin_PROGRAMS = semicrf gen bd2c
# 静的ライブラリ
semicrf_LDFLAGS = -L/usr/local/lib
semicrf_LDADD = -lmecab -lstdc++
# ソースコード
semicrf_SOURCES = Main.cpp SemiCrf.hpp SemiCrf.cpp AppTest.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp Optimizer.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp 
gen_SOURCES = Generator.cpp Logger.cpp Error.cpp ujson.cpp double-conversion.cc
bd2c_SOURCES = bd2c.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp

