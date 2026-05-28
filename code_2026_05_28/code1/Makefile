OUTPUT=build

$(OUTPUT)/mytest:main.cc
	@mkdir -p $(OUTPUT)
	g++ -o $@ $< -std=c++17

.PHONY:clean
clean:
	rm -rf $(OUTPUT)