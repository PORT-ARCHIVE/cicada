
SEMICRF=../semicrf

all: test1 test2

test1:
	@echo "###### check0 ######"
	$(SEMICRF) -t check1 -w tmp1 --log-level 2 -e0 2.0 -e1 1.0e-5 --max-iteration 100000 --disable-date-version 2>log1 
	@diff answer/log1 log1 && echo "OK" || echo "ERROR"

FILES0=\
	check1 \
	check2 \
	check3 \
	check4 \
	check5

test2:
	@for file in $(FILES0); do \
		echo "###### $$file ######"; \
		echo "$(SEMICRF) -t $$file -w tmp1 --log-level 0 -e0 2.0 -e1 1.0e-5 --max-iteration 100000"; \
		$(SEMICRF) -t $$file -w tmp1 --log-level 0 -e0 2.0 -e1 1.0e-5 --max-iteration 100000 && echo "OK" || echo "ERROR"; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 0"; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 0 > tmp2 && echo "OK" || echo "ERROR"; \
		echo "diff -c $$file tmp2"; \
		diff -c $$file tmp2 && echo "OK" || echo "ERROR"; \
		grep -v "^#" $$file | awk '{ print $$3 }' > a; \
		grep -v "^#" tmp2 | awk '{ print $$3 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }'; \
		rm tmp1 tmp2; \
	done;

learn:
	../gen -i p0.json -r 32 -l 16 > tmp1
	$(SEMICRF) -t tmp1 -w tmp2 --log-level 1 -e0 1.0e-3 -e1 1.0e-2 --max-iteration 100000 --disable-adagrad && echo "OK" || echo "ERROR"

infer:
	@rm -f tmp5;
	@for SEED in {1..32}; do \
		../gen -i p0.json -s $$SEED -r 32 -l 16 > tmp3; \
		$(SEMICRF) -i tmp3 -w tmp2 --log-level 0 > tmp4 || echo "ERROR"; \
		grep -v "^#" tmp3 | awk '{ print $$3 }' > a; \
		grep -v "^#" tmp4 | awk '{ print $$3 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100; }' | tee -a tmp5; \
		rm a b; \
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
	-cp result*.txt weight*.txt answer
	-cp test16.txt answer/test16.txt
	-cp test17.txt answer/test17.txt
	-cp test18.txt answer/test18.txt
	-cp test19.txt answer/test19.txt

gen:
	../gen -i p0.json -s 1 -r 2 -l 16 > test16.txt
	../gen -i p0.json -s 2 -r 4 -l 32 > test17.txt
	../gen -i p0.json -s 3 -r 8 -l 64 > test18.txt
	../gen -i p0.json -s 4 -r 16 -l 64 > test19.txt

clean:
	rm -f result*.txt weight*.txt a b tmp* log1
