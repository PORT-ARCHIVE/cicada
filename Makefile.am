# ライセンスがGPLにならないようにする指定
AUTOMAKE_OPTIONS = foreign
# 実行ファイル名
bin_PROGRAMS = semicrf gen bd2c bdc2e
# 静的ライブラリ
semicrf_LDFLAGS = -L/usr/local/lib
#semicrf_LDADD = -lmecab -lstdc++
semicrf_LDADD = -lstdc++
gen_LDADD = -lstdc++
bd2c_LDADD = -L/usr/local/lib -lmecab -lstdc++
bdc2e_LDADD = -lstdc++
# ソースコード
semicrf_SOURCES = Main.cpp SemiCrf.cpp SemiCrfData.cpp FeatureFunction.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp Optimizer.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp Signal.cpp
gen_SOURCES = Generator.cpp Logger.cpp Error.cpp ujson.cpp double-conversion.cc
bd2c_SOURCES = bd2c.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp
bdc2e_SOURCES = bdc2e.cpp SemiCrfData.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp

