
SEMICRF=../semicrf

all:
#	$(SEMICRF) -t test0.txt --debug > result0.txt && diff answer/result0.txt result0.txt && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i test1.txt --debug > result1.txt && diff answer/result1.txt result1.txt && echo "OK" || echo "ERROR"

	$(SEMICRF) -t test4.txt -w weight4.txt --debug-level 2 --max-length 1 -e0 1.0e-1 --max-iteration 10 1> result4.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result4.txt result4.txt && echo "OK" || echo "ERROR"
	diff answer/weight4.txt weight4.txt && echo "OK" || echo "ERROR"
#
	$(SEMICRF) -t test4.txt -w weight5.txt --debug-level 2 --max-length 2 -e0 1.0e-1 --max-iteration 5  1> result5.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result5.txt result5.txt && echo "OK" || echo "ERROR"
	diff answer/weight5.txt weight5.txt && echo "OK" || echo "ERROR"
#
	$(SEMICRF) -i test6.txt -w weight4.txt --debug-level 0 --max-length 1 1> result6.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result6.txt result6.txt && echo "OK" || echo "ERROR"

update:
#	cp result0.txt answer/result0.txt
#	cp result1.txt answer/result1.txt
#
	cp result4.txt answer/result4.txt
	cp weight4.txt answer/weight4.txt
#
	cp result5.txt answer/result5.txt
	cp weight5.txt answer/weight5.txt
#
	cp result6.txt answer/result6.txt
