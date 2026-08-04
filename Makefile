

all: main

main:
	python3 main.py

clean:
	find . -name "__pycache__" -type d -exec rm -r {} +
