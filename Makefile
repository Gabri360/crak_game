

all: main

main:
	python main.py

clean:
	find . -name "__pycache__" -type d -exec rm -r {} +
