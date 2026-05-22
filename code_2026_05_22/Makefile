.PHONY: all clean

# 1. 定义一个变量存放输出目录，方便后面修改
OUTPUT_DIR = build

# 2. 这里的目标也要写上路径，否则 Make 会找不到它们
all: $(OUTPUT_DIR)/udpserver $(OUTPUT_DIR)/udpclient

# 3. 编译服务器：加上目录前缀，并在编译前确保目录存在
$(OUTPUT_DIR)/udpserver: udp_server.cc
	@mkdir -p $(OUTPUT_DIR)
	g++ $^ -o $@ -std=c++17

# 4. 编译客户端
$(OUTPUT_DIR)/udpclient: udp_client.cc
	@mkdir -p $(OUTPUT_DIR)
	g++ $^ -o $@ -std=c++17

clean:
	rm -rf $(OUTPUT_DIR)