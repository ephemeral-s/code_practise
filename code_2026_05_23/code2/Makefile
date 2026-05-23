OUTPUT_DIR = build

$(OUTPUT_DIR)/myhttp:main.cpp
	@mkdir -p $(OUTPUT_DIR)
	g++ $^ -o $@ -std=c++17
.PHONY:clean
clean:
	rm -rf $(OUTPUT_DIR)