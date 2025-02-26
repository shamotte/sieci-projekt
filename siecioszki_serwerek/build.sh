g++ -o serwer -I ./external -I ./src -I ./external/JSON/include/include src/*.cpp -lsqlite3 -Wall
openssl req -newkey rsa:2048 -nodes -keyout server.key -x509 -days 365 -out server.crt
