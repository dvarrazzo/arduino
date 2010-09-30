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
	cd thermo && ${PYTHON} manage.py update -v -e ${EMAIL}

rollback:
	cd thermo && ${PYTHON} manage.py rollback

install_lib:
	wget http://googleappengine.googlecode.com/files/google_appengine_${APPENGINE_VER}.zip
	unzip -d lib google_appengine_${APPENGINE_VER}.zip
	rm google_appengine_${APPENGINE_VER}.zip
	(cd thermo && ln -s ../lib/google_appengine .google_appengine)
