
SEMICRF=../semicrf

all:
# 0,1,2,3
#	$(SEMICRF) -t test0.txt --debug > result0.txt && diff answer/result0.txt result0.txt && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i test1.txt --debug > result1.txt && diff answer/result1.txt result1.txt && echo "OK" || echo "ERROR"
# 4
	@echo "###### TEST 4 ######"
	@$(SEMICRF) -t test4.txt -w weight4.txt --debug-level 2 --max-length 1 -e0 1.0e-1 --max-iteration 10 --disable-adagrad 1> result4.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result4.txt result4.txt && echo "OK" || echo "ERROR"
	@diff answer/weight4.txt weight4.txt && echo "OK" || echo "ERROR"
# 5
	@echo "###### TEST 5 ######"
	@$(SEMICRF) -t test4.txt -w weight5.txt --debug-level 2 --max-length 2 -e0 1.0e-1 --max-iteration 5 --disable-adagrad 1> result5.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result5.txt result5.txt && echo "OK" || echo "ERROR"
	@diff answer/weight5.txt weight5.txt && echo "OK" || echo "ERROR"
# 6
	@echo "###### TEST 6 ######"
	@$(SEMICRF) -i test6.txt -w answer/weight4.txt --debug-level 0 --max-length 1 1> result6.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result6.txt result6.txt && echo "OK" || echo "ERROR"
# 7
	@echo "###### TEST 7 ######"
	@$(SEMICRF) -t test4.txt -w weight7.txt --debug-level 1 --max-length 1 -e0 1.0e-1 -e1 0.5e-1 --max-iteration 1024 --disable-adagrad 1> result7.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result7.txt result7.txt && echo "OK" || echo "ERROR"
	@diff answer/weight7.txt weight7.txt && echo "OK" || echo "ERROR"
# 8
	@echo "###### TEST 8 ######"
	@$(SEMICRF) -t test8.txt -w weight8.txt --debug-level 1 --max-length 2 -e0 1.0e-1 -e1 1.0e-2 --disable-adagrad 1> result8.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result8.txt result8.txt && echo "OK" || echo "ERROR"
	@diff answer/weight8.txt weight8.txt && echo "OK" || echo "ERROR"
# 9
	@echo "###### TEST 9 ######"
	@$(SEMICRF) -i test9.txt -w answer/weight8.txt --debug-level 0 --max-length 2 1> result9.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result9.txt result9.txt && echo "OK" || echo "ERROR"
# 10
	@echo "###### TEST 10 ######"
	@$(SEMICRF) -t test10.txt -w weight10.txt --debug-level 0 --max-length 1 -e0 1.0e-1 -e1 1.0e-2 --disable-adagrad 1> result10.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result10.txt result10.txt && echo "OK" || echo "ERROR"
	@diff answer/weight10.txt weight10.txt && echo "OK" || echo "ERROR"
# 11
	@echo "###### TEST 11 ######"
	@$(SEMICRF) -i test11.txt -w answer/weight10.txt --debug-level 0 --max-length 1 1> result11.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result11.txt result11.txt && echo "OK" || echo "ERROR"
# 12
	@echo "###### TEST 12 ######"
	@$(SEMICRF) -t test12.txt -w weight12.txt --debug-level 0 --max-length 2 -e0 1.0e-1 -e1 1.0e-2 --disable-adagrad 1> result12.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result12.txt result12.txt && echo "OK" || echo "ERROR"
	@diff answer/weight12.txt weight12.txt && echo "OK" || echo "ERROR"
# 13
	@echo "###### TEST 13 ######"
	@$(SEMICRF) -i test13.txt -w answer/weight12.txt --debug-level 0 --max-length 2 1> result13.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result13.txt result13.txt && echo "OK" || echo "ERROR"
# 14
	@echo "###### TEST 14 ######"
	@$(SEMICRF) -t test14.txt -w weight14.txt --debug-level 0 --max-length 2 -e0 1.0e-1 -e1 1.0e-2 --disable-adagrad 1> result14.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result14.txt result14.txt && echo "OK" || echo "ERROR"
	@diff answer/weight14.txt weight14.txt && echo "OK" || echo "ERROR"
# 15
	@echo "###### TEST 15 ######"
	@$(SEMICRF) -i test15.txt -w answer/weight14.txt --debug-level 0 --max-length 2 1> result15.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result15.txt result15.txt && echo "OK" || echo "ERROR"
# 16
	@echo "###### TEST 16 ######"
	@$(SEMICRF) -i test16.txt -w answer/weight16.txt --debug-level 0 --max-length 2 1> result16.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result16.txt result16.txt && echo "OK" || echo "ERROR"
# 17
	@echo "###### TEST 17 ######"
	@$(SEMICRF) -i test17.txt -w answer/weight17.txt --debug-level 2 --max-length 32 1> result17.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result17.txt result17.txt && echo "OK" || echo "ERROR"
# 18
	@echo "###### TEST 17 ######"
	@$(SEMICRF) -i test18.txt -w answer/weight18.txt --debug-level 2 --max-length 32 1> result18.txt 2>&1 && echo "OK" || echo "ERROR"
	@diff answer/result18.txt result18.txt && echo "OK" || echo "ERROR"



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
	rm -f result*.txt weight*.txt

