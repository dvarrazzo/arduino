EMAIL = daniele.varrazzo@gmail.com

PYTHON_VER = 2.6
APPENGINE_VER = 1.3.7

PYTHON = python${PYTHON_VER}

.PHONY: serve shell upload rollback check install_lib

serve:
	# cd thermo && ${PYTHON} manage.py runserver
	python ./lib/google_appengine/dev_appserver.py thermo/

check:
	cd thermo && ${PYTHON} manage.py test -v2 app

shell:
	cd thermo && ${PYTHON} manage.py shell

upload:
	python ./lib/google_appengine/appcfg.py -e ${EMAIL} update thermo/

rollback:
	cd thermo && ${PYTHON} manage.py rollback

install_lib: install_appengine install_chart

install_appengine: thermo/.google_appengine

install_chart: thermo/pygooglechart.py

thermo/.google_appengine:
	wget http://googleappengine.googlecode.com/files/google_appengine_${APPENGINE_VER}.zip
	unzip -d lib google_appengine_${APPENGINE_VER}.zip
	rm google_appengine_${APPENGINE_VER}.zip
	(cd thermo && ln -s ../lib/google_appengine .google_appengine)

thermo/pygooglechart.py:
	PYTHONPATH=lib easy_install -d lib pygooglechart
	(cd thermo/ && ln -sf `find ../lib/ -name pygooglechart.py`)
