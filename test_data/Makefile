
SEMICRF=../semicrf

all: test1 test2

test1:
	@echo "###### check0 ######"
	$(SEMICRF) -t check1.json -w tmp1 --log-level 2 -e0 2.0 -e1 1.0e-5 --max-iteration 100000 --disable-date-version --disable-regularization 2>log1 
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
		echo "$(SEMICRF) -t $$file -w tmp1 --log-level 0 -e0 2.0 -e1 1.0e-6 --max-iteration 100000 --disable-regularization"; \
		$(SEMICRF) -t $$file -w tmp1 --log-level 0 -e0 2.0 -e1 1.0e-6 --max-iteration 100000 --disable-regularization && echo "OK" || echo "ERROR"; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 0"; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 0 > tmp2 && echo "OK" || echo "ERROR"; \
		cat $$file | jq '.data[][][]' > r1; \
		cat tmp2 | jq '.data[][][]' > r2; \
		echo "diff -c r1 r2"; \
		diff -c r1 r2 && echo "OK" || echo "ERROR"; \
		cat r1 | awk '{ if(k++ % 3 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 3 == 2) print $$1 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }'; \
		rm tmp1 tmp2; \
	done;

learn:
	../gen -i p0.json -r 32 -l 16 > tmp1
	$(SEMICRF) -t tmp1 -w tmp2 --log-level 1 -e0 1.0e-3 -e1 1.0e-3 --max-iteration 100000 --disable-adagrad && echo "OK" || echo "ERROR"

infer:
	@rm -f r1 r2 a b tmp5;
	@for SEED in 1 2 3 4 5 6 7 8 9 10; do \
		../gen -i p0.json -s $$SEED -r 32 -l 16 > tmp3; \
		$(SEMICRF) -i tmp3 -w tmp2 --log-level 0 > tmp4 || echo "ERROR"; \
		cat tmp3 | jq '.data[][][]' > r1; \
		cat tmp4 | jq '.data[][][]' > r2; \
		cat r1 | awk '{ if(k++ % 3 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 3 == 2) print $$1 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }' | tee -a tmp5; \
	done;
	@awk '{ sum += $$1; count++; } END{ print "Accuracy:", sum/count, "%" }' tmp5

gen_test: gen
# 1
	@echo "###### GEN TEST 1 ######"
	@diff answer/test16.txt test16.txt && echo "OK" || echo "ERROR"
# 2
	@echo "###### GEN TEST 2 ######"
	@diff answer/test17.txt test17.txt && echo "OK" || echo "ERROR"
# 3
	@echo "###### GEN TEST 3 ######"
	@diff answer/test18.txt test18.txt && echo "OK" || echo "ERROR"
# 4
	@echo "###### GEN TEST 4 ######"
	@diff answer/test19.txt test19.txt && echo "OK" || echo "ERROR"

update:
	cp log1 answer/log1

gen:
	../gen -i p0.json -s 1 -r 2 -l 16 > test16.txt
	../gen -i p0.json -s 2 -r 4 -l 32 > test17.txt
	../gen -i p0.json -s 3 -r 8 -l 64 > test18.txt
	../gen -i p0.json -s 4 -r 16 -l 64 > test19.txt

clean:
	rm -f result*.txt weight*.txt a b tmp* log1
