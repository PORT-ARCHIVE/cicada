
SEMICRF=../semicrf

all:
	$(SEMICRF) -t test0.txt --debug > result0.txt && diff answer/result0.txt result0.txt && echo "OK" || echo "ERROR"
	$(SEMICRF) -i test1.txt --debug > result1.txt && diff answer/result1.txt result1.txt && echo "OK" || echo "ERROR"

update:
	cp result0.txt answer/result0.txt
	cp result1.txt answer/result1.txt
