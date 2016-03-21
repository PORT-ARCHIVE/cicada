
SEMICRF=../semicrf

all:
	$(SEMICRF) -t test4.txt --debug-level 2 --max-length 1 -e0 1.0e-1 --max-iteration 10 1>result4.txt 2>&1 && diff answer/result4.txt result4.txt && echo "OK" || echo "ERROR"
#	$(SEMICRF) -t test0.txt --debug > result0.txt && diff answer/result0.txt result0.txt && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i test1.txt --debug > result1.txt && diff answer/result1.txt result1.txt && echo "OK" || echo "ERROR"

update:
	cp result4.txt answer/result4.txt
#	cp result0.txt answer/result0.txt
#	cp result1.txt answer/result1.txt
