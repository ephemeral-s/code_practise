.PHONY: all clean

OUTPUT_DIR = build

all: $(OUTPUT_DIR)/tcp_client $(OUTPUT_DIR)/tcp_server

$(OUTPUT_DIR)/tcp_client: tcp_client.cc
	-mkdir -p $(OUTPUT_DIR)
	g++ -o $@ $< -std=c++17 -lpthread
$(OUTPUT_DIR)/tcp_server: tcp_server.cc
	-mkdir -p $(OUTPUT_DIR)
	g++ -o $@ $< -std=c++17 -lpthread
clean:
	-rm -rf $(OUTPUT_DIR)