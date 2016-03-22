
SEMICRF=../semicrf

all:
# 0,1,2,3
#	$(SEMICRF) -t test0.txt --debug > result0.txt && diff answer/result0.txt result0.txt && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i test1.txt --debug > result1.txt && diff answer/result1.txt result1.txt && echo "OK" || echo "ERROR"
# 4
	$(SEMICRF) -t test4.txt -w weight4.txt --debug-level 2 --max-length 1 -e0 1.0e-1 --max-iteration 10 1> result4.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result4.txt result4.txt && echo "OK" || echo "ERROR"
	diff answer/weight4.txt weight4.txt && echo "OK" || echo "ERROR"
# 5
	$(SEMICRF) -t test4.txt -w weight5.txt --debug-level 2 --max-length 2 -e0 1.0e-1 --max-iteration 5  1> result5.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result5.txt result5.txt && echo "OK" || echo "ERROR"
	diff answer/weight5.txt weight5.txt && echo "OK" || echo "ERROR"
# 6
	$(SEMICRF) -i test6.txt -w answer/weight4.txt --debug-level 0 --max-length 1 1> result6.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result6.txt result6.txt && echo "OK" || echo "ERROR"
# 7
	$(SEMICRF) -t test4.txt -w weight7.txt --debug-level 1 --max-length 1 -e0 1.0e-1 -e1 0.5e-1 --max-iteration 1024 1> result7.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result7.txt result7.txt && echo "OK" || echo "ERROR"
	diff answer/weight7.txt weight7.txt && echo "OK" || echo "ERROR"
# 8
	$(SEMICRF) -t test8.txt -w weight8.txt --debug-level 1 --max-length 2 -e0 1.0e-1 -e1 1.0e-2 1> result8.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result8.txt result8.txt && echo "OK" || echo "ERROR"
	diff answer/weight8.txt weight8.txt && echo "OK" || echo "ERROR"
# 9
	$(SEMICRF) -i test9.txt -w answer/weight8.txt --debug-level 0 --max-length 2 1> result9.txt 2>&1 && echo "OK" || echo "ERROR"
	diff answer/result9.txt result9.txt && echo "OK" || echo "ERROR"

update:
	cp result*.txt weight*.txt answer

clean:
	rm -f result*.txt weight*.txt

