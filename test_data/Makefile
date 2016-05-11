
SEMICRF=../semicrf

all: test1 test2 gen_test bd2c_test bd2w_test test3 learn infer

test1:
	@echo "###### check0 ######"
	$(SEMICRF) -t check1.json -w tmp1 --log-level 1 -e0 2.0 -e1 1.0e-5 --disable-date-version --disable-regularization --disable-log-color --set-log-pattern "%v" 2>log1 
	@diff answer/log1 log1 && echo "OK" || echo "ERROR"

FILES0=\
	check1.json \
	check2.json \
	check3.json \
	check4.json \
	check5.json

test2:
	@for file in $(FILES0); do \
		echo "###### $$file ######"; \
		echo "$(SEMICRF) -t $$file -w tmp1 --log-level 3 -e0 2.0 -e1 1.0e-6 --disable-regularization"; \
		$(SEMICRF) -t $$file -w tmp1 --log-level 3 -e0 2.0 -e1 1.0e-6 --disable-regularization && echo "OK" || echo "ERROR"; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 3"; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 3 > tmp2 && echo "OK" || echo "ERROR"; \
		cat $$file | jq '.data[][][]' > r1; \
		cat tmp2 | jq '.data[][][]' > r2; \
		echo "diff -c r1 r2"; \
		diff -c r1 r2 && echo "OK" || echo "ERROR"; \
		cat r1 | awk '{ if(k++ % 3 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 3 == 2) print $$1 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }'; \
		rm tmp1 tmp2; \
	done;

FILES1=\
	check7.json

MATRIX=../../word2vec/wakati.bin

test3:
	@for file in $(FILES1); do \
		echo "###### $$file ######"; \
		echo "$(SEMICRF) -t $$file -w tmp1 --log-level 2 -e0 2.0 -e1 1.0e-6 --disable-regularization --w2v-matrix ${MATRIX}"; \
		time $(SEMICRF) -t $$file -w tmp1 --log-level 2 -e0 2.0 -e1 1.0e-6 --disable-regularization --w2v-matrix ${MATRIX} && echo "OK" || echo "ERROR"; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 2 --w2v-matrix ${MATRIX}" ; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} > tmp2 && echo "OK" || echo "ERROR"; \
		cat $$file | jq '.data[][][]' > r1; \
		cat tmp2 | jq '.data[][][]' > r2; \
		cat r1 | awk '{ if(k++ % 4 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 4 == 2) print $$1 }' > b; \
		echo "diff -c a b"; \
		diff -c a b && echo "OK" || echo "ERROR"; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }'; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 2 --w2v-matrix ${MATRIX} --enable-simple-prediction-output" ; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} --enable-simple-prediction-output > simple_output.json && echo "OK" || echo "ERROR"; \
		echo "diff -c answer/simple_output.json simple_output.json"; \
		diff -c answer/simple_output.json simple_output.json && echo "OK" || echo "ERROR"; \
		rm tmp1 tmp2; \
	done;

learn:
	../gen -i p0.json -r 32 -l 16 > tmp1
	time $(SEMICRF) -t tmp1 -w tmp2 --log-level 2 -e0 1.0e-3 -e1 1.0e-3 && echo "OK" || echo "ERROR"

infer:
	@rm -f r1 r2 a b tmp5;
	@for SEED in 1 2 3 4 5 6 7 8 9 10; do \
		../gen -i p0.json -s $$SEED -r 32 -l 16 > tmp3; \
		$(SEMICRF) -i tmp3 -w tmp2 --log-level 2 > tmp4 || echo "ERROR"; \
		cat tmp3 | jq '.data[][][]' > r1; \
		cat tmp4 | jq '.data[][][]' > r2; \
		cat r1 | awk '{ if(k++ % 3 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 3 == 2) print $$1 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }' | tee -a tmp5; \
	done;
	@awk '{ sum += $$1; count++; } END{ print "Accuracy:", sum/count, "%" }' tmp5

gen_test:
	@echo "###### gen test 1 ######"
	@echo "../gen -i p0.json -s 1 -r 2 -l 16"
	@../gen -i p0.json -s 1 -r 2 -l 16 > test16.json && echo "OK" || echo "ERROR"
	@diff -c answer/test16.json test16.json && echo "OK" || echo "ERROR"
#
	@echo "###### gen test 2 ######"
	@echo "./gen -i p0.json -s 2 -r 4 -l 32"
	@../gen -i p0.json -s 2 -r 4 -l 32 > test17.json && echo "OK" || echo "ERROR"
	@diff -c answer/test17.json test17.json && echo "OK" || echo "ERROR"
#
	@echo "###### gen test 3 ######"
	@echo "../gen -i p0.json -s 3 -r 8 -l 64"
	@../gen -i p0.json -s 3 -r 8 -l 64 > test18.json && echo "OK" || echo "ERROR"
	@diff -c answer/test18.json test18.json && echo "OK" || echo "ERROR"
#
	@echo "###### gen test 4 ######"
	@echo "../gen -i p0.json -s 4 -r 16 -l 64"
	@../gen -i p0.json -s 4 -r 16 -l 64 > test19.json && echo "OK" || echo "ERROR"
	@diff -c answer/test19.json test19.json && echo "OK" || echo "ERROR"

bd2c_test:
	@echo "###### bd2c test 1 ######"
	@echo "../bd2c -b body0.json -w ../../word2vec/wakati.bin -l labels0.json > check8.json"
	@../bd2c -b body0.json -w ../../word2vec/wakati.bin -l labels0.json > check8.json && echo "OK" || echo "ERROR"
	@diff -c answer/check8.json check8.json && echo "OK" || echo "ERROR"

bd2w_test:
	@echo "###### bd2w test 1 ######"
	@echo "../bd2w body1.json > body1.txt"
	@../bd2w body1.json > body1.txt && echo "OK" || echo "ERROR"
	@diff answer/body1.txt body1.txt && echo "OK" || echo "ERROR"

update:
	-cp log1 answer/log1
	-cp check8.json answer/check8.json
	-cp test16.json answer/test16.json
	-cp test17.json answer/test17.json
	-cp test18.json answer/test18.json
	-cp test19.json answer/test19.json
	-cp body1.txt answer/body1.txt
	-cp simple_output.json answer/simple_output.json

clean:
	rm -f a b r1 r2 tmp* log1 check8.json
	rm -f test16.json test17.json test18.json test19.json body1.txt
