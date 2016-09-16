
CONTAINER_NAME=cicada_container

SEMICRF=../semicrf
AREA_DIC=../dic/Noun.place.utf8.csv
JOB_DIC=../dic/job.txt

all: test1 test2 test3 test4 test5 learn infer gen_test bd2c_test bd2w_test bdc2e_test

test1:
	@echo "###### check0 ######"
	$(SEMICRF) -t check1.json -w tmp1 --log-level 0 -e0 2.0 -e1 1.0e-5 --disable-date-version --disable-regularization --disable-log-color --set-log-pattern "%v" -l auto 2>log1 
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
		echo "$(SEMICRF) -t $$file -w tmp1 --log-level 3 -e0 2.0 -e1 1.0e-3 --disable-regularization"; \
		$(SEMICRF) -t $$file -w tmp1 --log-level 3 -e0 2.0 -e1 1.0e-6 --disable-regularization && echo "OK" || echo "ERROR"; \
		echo "$(SEMICRF) -i $$file -w tmp1 --log-level 3"; \
		$(SEMICRF) -i $$file -w tmp1 --log-level 3 > tmp2 && echo "OK" || echo "ERROR"; \
		cat $$file | jq '.pages[].data[][][]' > r1; \
		cat tmp2 | jq '.pages[].data[][][]' > r2; \
		echo "diff -c r1 r2"; \
		diff -c r1 r2 && echo "OK" || echo "ERROR"; \
		cat r1 | awk '{ if(k++ % 3 == 2) print $$1 }' > a; \
		cat r2 | awk '{ if(k++ % 3 == 2) print $$1 }' > b; \
		paste a b | awk '{ if( $$1 == $$2 ) count++; total++; } END{ print count/total*100,"%"; }'; \
		rm tmp1 tmp2; \
	done;

FILES1=check6.json

MATRIX=../../word2vec/wakati.bin

test3:
	time $(SEMICRF) -t ${FILES1} -w tmp1 --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} && echo "OK" || echo "ERROR"
	$(SEMICRF) -i ${FILES1} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} > tmp2 && echo "OK" || echo "ERROR"
	jq '.pages[].data[][][2]' ${FILES1} > r1
	jq '.pages[].data[][][2]' tmp2 > r2
	echo "diff -c r1 r2"
	diff -c r1 r2 && echo "OK" || echo "ERROR"
	$(SEMICRF) -i ${FILES1} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > simple_output3.json && echo "OK" || echo "ERROR"
	diff -c answer/simple_output3.json simple_output3.json && echo "OK" || echo "ERROR"
	$(SEMICRF) -i check10.json -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > check10_output.json && echo "OK" || echo "ERROR"
	diff -c answer/check10_output.json check10_output.json && echo "OK" || echo "ERROR"
	rm tmp1 tmp2


FILES2=check12.json

test4:
	time $(SEMICRF) -t ${FILES2} -w tmp1 --log-level 2 -e0 1.0e-6 -e1 1.0e-4 --regularization-parameter 1e-8 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} && echo "OK" || echo "ERROR"
	$(SEMICRF) -i ${FILES2} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} > tmp2 && echo "OK" || echo "ERROR"
	jq '.pages[].data[][][2]' ${FILES2} > r1
	jq '.pages[].data[][][2]' tmp2 > r2
	echo "diff -c r1 r2"
	diff -c r1 r2 && echo "OK" || echo "ERROR"
	$(SEMICRF) -i ${FILES2} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > simple_output4.json && echo "OK" || echo "ERROR"
	diff -c answer/simple_output4.json simple_output4.json && echo "OK" || echo "ERROR"
	$(SEMICRF) -i check13.json -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > check13_output.json && echo "OK" || echo "ERROR"
	diff -c answer/check13_output.json check13_output.json && echo "OK" || echo "ERROR"
	rm tmp1 tmp2

FILES3=check14.json

test5:
	time $(SEMICRF) -t ${FILES3} -w tmp1 --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} && echo "OK" || echo "ERROR"
	$(SEMICRF) -i ${FILES3} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} > tmp2 && echo "OK" || echo "ERROR"
	jq '.pages[].data[][][2]' ${FILES3} > r1
	jq '.pages[].data[][][2]' tmp2 > r2
	echo "diff -c r1 r2"
	diff -c r1 r2 && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i ${FILES3} -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > simple_output4.json && echo "OK" || echo "ERROR"
#	diff -c answer/simple_output4.json simple_output4.json && echo "OK" || echo "ERROR"
#	$(SEMICRF) -i check13.json -w tmp1 --log-level 3 --w2v-matrix ${MATRIX} -a ${AREA_DIC} -j ${JOB_DIC} --enable-simple-prediction-output > check13_output.json && echo "OK" || echo "ERROR"
#	diff -c answer/check13_output.json check13_output.json && echo "OK" || echo "ERROR"
#	rm tmp1 tmp2

learn:
	../gen -i p0.json -r 32 -l 16 > tmp1
	time $(SEMICRF) -t tmp1 -w tmp2 --log-level 2 -e0 1.0e-3 -e1 1.0e-4 --regularization-parameter 1e-3 && echo "OK" || echo "ERROR"

infer:
	@rm -f r1 r2 a b tmp5;
	@for SEED in 1 2 3 4 5 6 7 8 9 10; do \
		../gen -i p0.json -s $$SEED -r 32 -l 16 > tmp3; \
		$(SEMICRF) -i tmp3 -w tmp2 --log-level 2 > tmp4 || echo "ERROR"; \
		cat tmp3 | jq '.pages[].data[][][]' > r1; \
		cat tmp4 | jq '.pages[].data[][][]' > r2; \
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
	@echo "../bd2c -b body0.json -w ../../word2vec/wakati.bin -l labels0.json --set-overlap-size 8 --set-sentence-size 1024 > check8.json"
	@../bd2c -b body0.json -w ../../word2vec/wakati.bin -l labels0.json --set-overlap-size 8 --set-sentence-size 1024 > check8.json && echo "OK" || echo "ERROR"
	@diff -c answer/check8.json check8.json && echo "OK" || echo "ERROR"

bd2w_test:
	@echo "###### bd2w test 1 ######"
	@echo "../bd2w body1.json > body1.txt"
	@../bd2w body1.json > body1.txt && echo "OK" || echo "ERROR"
	@diff answer/body1.txt body1.txt && echo "OK" || echo "ERROR"

bdc2e_test:
	@echo "###### bdc2e test 1 ######"
	@echo "../bdc2e -b t0.json -c t1.json"
	@../bdc2e -b t0.json -c t1.json > bdc2e_result.json && echo "OK" || echo "ERROR"
	@diff answer/bdc2e_result.json bdc2e_result.json && echo "OK" || echo "ERROR"

docker_run:
	docker run -v /var/projects/research-playground/semi-crf/test_data:/home/docker_user/research-playground/semi-crf/test_data \
	--name $(CONTAINER_NAME) -it `docker images | grep "docker_common/cicada" | awk '{ print $$3 }'` \
	/bin/sh -c 'cd semi-crf/test_data && make test1 && make test2 && make gen_test && make bd2w_test && make bdc2e_test'
	docker stop $(CONTAINER_NAME)
	docker rm $(CONTAINER_NAME)

docker_kill:
	docker stop $(CONTAINER_NAME)
	docker rm $(CONTAINER_NAME)

update:
	-cp log1 answer/log1
	-cp check8.json answer/check8.json
	-cp test16.json answer/test16.json
	-cp test17.json answer/test17.json
	-cp test18.json answer/test18.json
	-cp test19.json answer/test19.json
	-cp body1.txt answer/body1.txt
	-cp simple_output3.json answer/simple_output3.json
	-cp simple_output4.json answer/simple_output4.json
	-cp check10_output.json answer/check10_output.json
	-cp check11_output.json answer/check11_output.json
	-cp check13_output.json answer/check13_output.json
	-cp bdc2e_result.json answer/bdc2e_result.json

clean:
	rm -f a b r1 r2 tmp* log1 check8.json
	rm -f test16.json test17.json test18.json test19.json body1.txt
