# 编译器设置
CXX = g++
TARGET = comprehensive_test
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g -pthread
LDFLAGS = -pthread

# 源文件
SRCS = ThreadSafeCounter.cpp comprehensive_test.cpp
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 主目标
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "构建完成: $(TARGET)"

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 调试版本
debug: CXXFLAGS += -DDEBUG -O0
debug: $(TARGET)

# 使用 ThreadSanitizer 的版本
tsan: CXXFLAGS += -fsanitize=thread -O1 -fno-omit-frame-pointer
tsan: LDFLAGS += -fsanitize=thread
tsan: $(TARGET)
	@echo "ThreadSanitizer 版本已构建"

# 性能优化版本
release: CXXFLAGS += -O3 -DNDEBUG
release: LDFLAGS += -s
release: $(TARGET)

# 运行测试
run: $(TARGET)
	./$(TARGET)

# 运行性能测试
run-perf: $(TARGET)
	@echo "运行性能测试..."
	./$(TARGET)

# 清理
clean:
	rm -f $(OBJS) $(TARGET) *.log

# 安装依赖 (Ubuntu/Debian)
install-deps:
	sudo apt update
	sudo apt install g++ build-essential

# 显示帮助
help:
	@echo "可用目标:"
	@echo "  all       - 标准编译 (默认)"
	@echo "  debug     - 调试版本编译"
	@echo "  tsan      - 使用 ThreadSanitizer 编译"
	@echo "  release   - 发布版本编译"
	@echo "  run       - 编译并运行测试"
	@echo "  run-perf  - 运行性能测试"
	@echo "  clean     - 清理生成的文件"
	@echo "  install-deps - 安装编译依赖"

.PHONY: all debug tsan release run run-perf clean install-deps help