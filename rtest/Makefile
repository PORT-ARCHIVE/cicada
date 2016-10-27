
TRAINING_DATA=\
	3710a0af5fec95c9.html
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

LOG_LEVEL=1

learn:
	for file in $(TRAINING_DATA); do \
		ruby ../../traning-data-converter/converter.rb $$file --labels ../../setting_files/label_settings.json --length 16 | jq '.' > tr.json; \
		../semicrf -t tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt; \
		../semicrf -i tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1; \
	done;

learn_all:
	ruby ../../traning-data-converter/converter.rb *.html --labels ../../setting_files/label_settings.json --length 16 | jq '.' > tr.json
	../semicrf -t tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt
	../semicrf -i tr.json -w w.json --log-level $(LOG_LEVEL) -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1

test:
	../bd2c -b body.json -l ../../setting_files/label_settings.json --set-overlap-size 8 --set-sentence-size 1024 > tmp.json
	../semicrf -i tmp.json -w w.json --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1



