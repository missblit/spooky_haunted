build:
	g++ -o a.out main.cpp process.cpp -lwthttp -lwt -lboost_signals-mt -std=c++11

build2:
	g++ -o a.out test2.cpp process.cpp -lwthttp -lwt -lboost_signals-mt -std=c++11

run: build
	./a.out --docroot . --http-address 0.0.0.0 --http-port 9090

run2: build2
	./a.out --docroot . --http-address 0.0.0.0 --http-port 9090
