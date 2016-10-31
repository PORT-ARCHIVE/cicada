
JCMP=../jcmp

LAVEL_FILE=../../setting_files/label_settings.json

TRAINING_DATA=\
	3710a0af5fec95c9.html\
	9565259e79069401.html\
	a75001d37fcf72f5.html\
	aa69bf79b034eaf1.html\
	bf6173d2216db4bd.html\
	8c69f07d0b731092.html\
	b4eedd4f7ef60ab6.html\
	e6e7a3baab0d3a01.html\
	b01df03d39b14d51.html\
	7503f280a4b2af3c.html\
	afe9fdd85e915cd7.html\
	1c52e1f96f3a3a08.html\
	935ecbf0e73d351f.html\
	da74ae14f81f8398.html\
	e53e04c8f92ae251.html

LOG_LEVEL=2
RESULT0=result0.json
RESULT1=result1.json

learn:
	echo "[" > tmp1.json
	for file in $(TRAINING_DATA); do \
		echo $$file; \
		ruby ../../traning-data-converter/converter.rb $$file --labels ../../setting_files/label_settings.json --length 16 | jq '.' > tr.json; \
		../semicrf -t tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt; \
		../semicrf -i tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1 > tmp2.json; \
		jq '.[] | { job_opening_type:. .crf_estimate.job_opening_type, place: .crf_estimate.place, title: .title }' tmp2.json >> tmp1.json; \
		echo "," >> tmp1.json; \
	done;
	sed -i '' '$$d' tmp1.json
	echo "]" >> tmp1.json
	jq 'sort_by(.title)' tmp1.json > $(RESULT1)
	$(JCMP) $(RESULT0) $(RESULT1) place
	$(JCMP) $(RESULT0) $(RESULT1) job_opening_type

learn_all:
	ruby ../../traning-data-converter/converter.rb *.html --labels ../../setting_files/label_settings.json --length 16 | jq '.' > tr.json
	../semicrf -t tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt
	../semicrf -i tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1 > tmp1.json;
	jq '. | map({ job_opening_type: .crf_estimate.job_opening_type, place: .crf_estimate.place, title: .title })' tmp1.json > tmp2.json;
	jq 'sort_by(.title)' tmp2.json > $(RESULT1)
	$(JCMP) $(RESULT0) $(RESULT1) place
	$(JCMP) $(RESULT0) $(RESULT1) job_opening_type

predict:
	rm -f $(RESULT1)
	../bd2c -b body.json -l ../../setting_files/label_settings.json --set-overlap-size 8 --set-sentence-size 1024 > tmp1.json
	../semicrf -i tmp1.json -w w.json --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1 > tmp2.json
	jq '. | map({ job_opening_type: .crf_estimate.job_opening_type, place: .crf_estimate.place, title: .title })' tmp2.json > tmp3.json;
	jq 'sort_by(.title)' tmp3.json > $(RESULT1)
	$(JCMP) $(RESULT0) $(RESULT1) place
	$(JCMP) $(RESULT0) $(RESULT1) job_opening_type

update:
	cp $(RESULT1) $(RESULT0)



