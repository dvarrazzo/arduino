PYTHON = python -u
SERVER = http://s4mples.appspot.com

send_all: send_google send_arduino

send_google:
	$(PYTHON) google_api.py London | \
	$(PYTHON) sender.py --device agdzNG1wbGVzcg0LEgZEZXZpY2UY6QcM \
	--server $(SERVER) &

send_arduino:
	$(PYTHON) serial_read.py | \
	$(PYTHON) sender.py --device agdzNG1wbGVzcgwLEgZEZXZpY2UYAQw \
	--server $(SERVER) &

