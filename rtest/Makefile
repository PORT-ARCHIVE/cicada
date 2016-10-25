

all:
	../semicrf -t tr.json -w w.json --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt
	../semicrf -i tr.json -w w.json --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1



test:
	../bd2c -b body.json -l ../../setting_files/label_settings.json --set-overlap-size 8 --set-sentence-size 1024 > tmp.json
	../semicrf -i tmp.json -w w.json --log-level 2 -e0 1.0e-6 -e1 1.0e-3 --regularization-parameter 1e-8 -a ../dic/Noun.place.utf8.csv -j ../dic/job.txt --output-format 1



