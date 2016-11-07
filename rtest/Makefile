
CONVERTER=../../traning-data-converter/converter.rb
CICADA=../semicrf
JCMP=../jcmp

LAVELS=../../setting_files/label_settings.json
LENGTH=16
ADIC=../dic/Noun.place.utf8.csv
JDIC=../dic/job.txt

#	2471b501418402e2.html\

TRAINING_DATA=\
	14416a08c5a9b926.html\
	27b62192221f179e.html\
	a2fb2a78ab3535d1.html\
	044539b691204dcd.html\
	ed9d1d940ccabdbb.html\
	0cd6f17aec5899c7.html\
	d52404eb286f7780.html\
	70aa7c762a0a0a18.html\
	87ef9fbb4566746e.html\
	590bd694608c67ab.html\
	6ac00f765399b136.html\
	b6ae5d8c74575cbc.html\
	a4d1ae40111a19e2.html\
	7de454dd476b60ac.html\
	e53e04c8f92ae251.html\
	dcc37ef3619217e0.html\
	31ab570617c735d1.html\
	5475bd27921f7ea9.html\
	61698eff64fdd9cb.html\
	76a24d6d68d7650f.html\
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
	da74ae14f81f8398.html

simple:
	@for file in $(TRAINING_DATA); do \
		echo $$file; \
		ruby $(CONVERTER) $$file --labels $(LAVELS) --length $(LENGTH) | jq '.' > tr.json; \
		$(CICADA) -t tr.json -w w.json --log-level 3 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a $(ADIC) -j $(JDIC); \
		$(CICADA) -i tr.json -w w.json --log-level 3 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a $(ADIC) -j $(JDIC) --output-format 1 > tmp1.json; \
		jq '. | map({ job_opening_type:. .crf_estimate.job_opening_type, place: .crf_estimate.place, title: .title })' tmp1.json > $$file.json; \
		$(JCMP) ref/$$file.json $$file.json place; \
		$(JCMP) ref/$$file.json $$file.json job_opening_type; \
	done;

learn:
	ruby $(CONVERTER) $(TRAINING_DATA) --labels $(LAVELS) --length $(LENGTH) | jq '.' > tr.json
	$(CICADA) -t tr.json -w w.json --log-level 2 -e0 1.0e-8 -e1 1.0e-2 --regularization-parameter 1e-10 -a $(ADIC) -j $(JDIC)

predict:
	echo "[" > result0.json
	@for file in $(TRAINING_DATA); do \
		echo $$file; \
		ruby ../../traning-data-converter/converter.rb $$file --labels $(LAVELS) --length $(LENGTH) | jq '.' > tr.json; \
		$(CICADA) -i tr.json -w w.json --log-level 3 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a $(ADIC) -j $(JDIC) --output-format 1 > tmp1.json; \
		jq '.[] | { job_opening_type:. .crf_estimate.job_opening_type, place: .crf_estimate.place, title: .title }' tmp1.json | tee $$file.json; \
		cat $$file.json >> result0.json; \
		echo "," >> result0.json; \
	done;
	sed -i '' '$$d' result0.json
	echo "]" >> result0.json
	jq 'sort_by(.title)' result0.json > tmp2.json
	jq 'sort_by(.title)' ref/result0.json > tmp3.json
	$(JCMP) tmp3.json tmp2.json place
	$(JCMP) tmp3.json tmp2.json job_opening_type

update:
	@for file in $(TRAINING_DATA); do \
		mv $$file.json ref/$$file.json; \
	done;
	cp result0.json ref/result0.json

clean:
	rm tmp1.json tmp2.json





