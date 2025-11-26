// comprehensive_stress_test.cpp
#include "ThreadSafeCounter.h" // 您的线程安全计数器头文件
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <cassert>

// 压力测试结果结构体
struct StressTestResult {
    std::string test_name;
    long long duration_ms;
    int expected_count;
    int actual_count;
    bool passed;
    size_t total_operations;
    double throughput_ops_per_sec;
};

/**
 * 基础压力测试：验证正确性并测量性能
 */
StressTestResult basic_stress_test(ThreadSafeCounter& counter, int num_threads, int increments_per_thread, const std::string& test_name) {
    std::cout << "=== " << test_name << " ===" << std::endl;
    std::cout << "配置: " << num_threads << " 线程 × " << increments_per_thread << " 次递增" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    auto increment_task = [&counter](int count) {
        for (int i = 0; i < count; ++i) {
            counter.increment();
        }
    };

    // 创建并启动所有线程
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_task, increments_per_thread);
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int final_count = counter.get();
    int expected_count = num_threads * increments_per_thread;
    bool test_passed = (final_count == expected_count);
    size_t total_ops = num_threads * increments_per_thread;
    double throughput = (duration.count() > 0) ? (total_ops * 1000.0) / duration.count() : 0.0;

    std::cout << "实际计数: " << final_count << std::endl;
    std::cout << "预期计数: " << expected_count << std::endl;
    std::cout << "耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "吞吐量: " << std::fixed << std::setprecision(2) << throughput << " 操作/秒" << std::endl;
    std::cout << (test_passed ? "✅ 测试通过" : "❌ 测试失败") << "\n" << std::endl;

    return {test_name, duration.count(), expected_count, final_count, test_passed, total_ops, throughput};
}

/**
 * 混合读写压力测试：模拟真实场景，同时有读写操作
 */
StressTestResult mixed_read_write_stress_test(ThreadSafeCounter& counter, int num_writer_threads, int writes_per_writer, int num_reader_threads, int reads_per_reader) {
    std::string test_name = "混合读写压力测试";
    std::cout << "=== " << test_name << " ===" << std::endl;
    std::cout << "写线程: " << num_writer_threads << " × " << writes_per_writer << " 次写入" << std::endl;
    std::cout << "读线程: " << num_reader_threads << " × " << reads_per_reader << " 次读取" << std::endl;

    std::atomic<bool> stop_test{false};
    std::atomic<int> read_errors{0};
    std::atomic<long> total_reads{0};
    std::atomic<int> last_read_value{0};
    int initial_count = counter.get();

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> writer_threads;
    std::vector<std::thread> reader_threads;

    // 启动写线程
    auto writer_task = [&counter, writes_per_writer]() {
        for (int i = 0; i < writes_per_writer; ++i) {
            counter.increment();
            // 模拟一点工作量
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    };

    for (int i = 0; i < num_writer_threads; ++i) {
        writer_threads.emplace_back(writer_task);
    }

    // 启动读线程
    auto reader_task = [&counter, &read_errors, &total_reads, &last_read_value, reads_per_reader, &stop_test]() {
        for (int j = 0; j < reads_per_reader && !stop_test; ++j) {
            int value = counter.get();
            total_reads++;
            last_read_value = value;

            // 基本合理性检查：值不应为负
            if (value < 0) {
                read_errors++;
            }

            // 短暂睡眠，模拟读操作处理
            std::this_thread::sleep_for(std::chrono::microseconds(2));
        }
    };

    for (int i = 0; i < num_reader_threads; ++i) {
        reader_threads.emplace_back(reader_task);
    }

    // 等待所有写线程完成
    for (auto& t : writer_threads) {
        t.join();
    }

    // 通知读线程停止
    stop_test = true;

    // 等待所有读线程完成
    for (auto& t : reader_threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int final_count = counter.get();
    int expected_writes = num_writer_threads * writes_per_writer;
    int expected_final_count = initial_count + expected_writes;
    bool test_passed = (final_count == expected_final_count) && (read_errors == 0);

    size_t total_ops = expected_writes + total_reads;
    double throughput = (duration.count() > 0) ? (total_ops * 1000.0) / duration.count() : 0.0;

    std::cout << "初始计数: " << initial_count << std::endl;
    std::cout << "实际最终计数: " << final_count << std::endl;
    std::cout << "预期最终计数: " << expected_final_count << std::endl;
    std::cout << "总读取次数: " << total_reads << std::endl;
    std::cout << "读取错误数: " << read_errors << std::endl;
    std::cout << "耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "吞吐量: " << std::fixed << std::setprecision(2) << throughput << " 操作/秒" << std::endl;
    std::cout << (test_passed ? "✅ 测试通过" : "❌ 测试失败") << "\n" << std::endl;

    return {test_name, duration.count(), expected_final_count, final_count, test_passed, total_ops, throughput};
}

/**
 * 极限压力测试：创建远超CPU核心数的线程
 */
StressTestResult extreme_stress_test(ThreadSafeCounter& counter) {
    // 创建大量线程，远超CPU核心数
    const unsigned int hardware_concurrency = std::thread::hardware_concurrency();
    const int num_threads = (hardware_concurrency > 0) ? hardware_concurrency * 4 : 64; // 大量线程
    const int increments_per_thread = 1000;

    std::string test_name = "极限压力测试(线程数:" + std::to_string(num_threads) + ")";
    std::cout << "=== " << test_name << " ===" << std::endl;
    std::cout << "硬件并发数: " << hardware_concurrency << std::endl;
    std::cout << "测试线程数: " << num_threads << " (约" << (hardware_concurrency > 0 ? hardware_concurrency * 4 : 64) << "倍)" << std::endl;
    std::cout << "每个线程递增次数: " << increments_per_thread << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&counter, increments_per_thread]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                counter.increment();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int final_count = counter.get();
    int expected_count = num_threads * increments_per_thread;
    bool test_passed = (final_count == expected_count);
    double throughput = (duration.count() > 0) ? (expected_count * 1000.0) / duration.count() : 0.0;

    std::cout << "实际计数: " << final_count << std::endl;
    std::cout << "预期计数: " << expected_count << std::endl;
    std::cout << "耗时: " << duration.count() << " ms" << std::endl;
    std::cout << "吞吐量: " << std::fixed << std::setprecision(2) << throughput << " 操作/秒" << std::endl;
    std::cout << (test_passed ? "✅ 极限测试通过" : "❌ 极限测试失败") << "\n" << std::endl;

    return {test_name, duration.count(), expected_count, final_count, test_passed, 
            static_cast<size_t>(expected_count), throughput};
}

/**
 * 性能对比测试：运行不同规模的测试并对比结果
 */
void performance_comparison_test() {
    std::cout << "=== 性能对比测试 ===" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(20) << "测试场景" 
              << std::setw(12) << "线程数" 
              << std::setw(12) << "操作数"
              << std::setw(10) << "耗时(ms)" 
              << std::setw(15) << "吞吐量(ops/s)" 
              << std::setw(10) << "状态" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    // 定义不同的测试场景
    std::vector<std::pair<std::string, std::pair<int, int>>> test_scenarios = {
        {"轻度负载", {4, 1000}},
        {"中等负载", {8, 5000}},
        {"重度负载", {16, 10000}},
        {"高并发", {32, 2000}},
        {"大规模操作", {8, 50000}}
    };

    std::vector<StressTestResult> results;

    // 运行每个测试场景
    for (const auto& scenario : test_scenarios) {
        ThreadSafeCounter counter; // 每个测试使用新的计数器实例
        std::string name = scenario.first;
        int threads = scenario.second.first;
        int operations = scenario.second.second;
        
        StressTestResult result = basic_stress_test(counter, threads, operations, name);
        results.push_back(result);
        
        std::cout << std::setw(20) << name 
                  << std::setw(12) << threads 
                  << std::setw(12) << operations
                  << std::setw(10) << result.duration_ms 
                  << std::setw(15) << std::fixed << std::setprecision(2) << result.throughput_ops_per_sec
                  << std::setw(10) << (result.passed ? "PASS" : "FAIL") << std::endl;
    }

    std::cout << std::string(80, '=') << std::endl;
    
    // 计算平均吞吐量
    double total_throughput = 0;
    int passed_tests = 0;
    for (const auto& result : results) {
        if (result.passed) {
            total_throughput += result.throughput_ops_per_sec;
            passed_tests++;
        }
    }
    
    std::cout << "平均吞吐量: " << (passed_tests > 0 ? total_throughput / passed_tests : 0) 
              << " 操作/秒 (基于" << passed_tests << "个通过测试)" << std::endl;
    std::cout << "总测试数: " << results.size() << "，通过: " << passed_tests 
              << "，失败: " << (results.size() - passed_tests) << "\n" << std::endl;
}

/**
 * 长时间稳定性测试
 */
void long_running_stability_test() {
    std::cout << "=== 长时间稳定性测试 (运行10秒) ===" << std::endl;
    
    ThreadSafeCounter counter;
    std::atomic<bool> stop_test{false};
    std::atomic<int> increments_done{0};
    std::atomic<int> reads_done{0};
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 创建多个工作线程
    std::vector<std::thread> workers;
    const int num_workers = 8;
    
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back([&counter, &stop_test, &increments_done, i]() {
            while (!stop_test) {
                counter.increment();
                increments_done++;
                // 偶尔休息一下
                if (i % 2 == 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        });
    }
    
    // 创建读线程
    std::thread reader([&counter, &stop_test, &reads_done]() {
        while (!stop_test) {
            int val = counter.get();
            reads_done++;
            // 读取的值应该非负
            if (val < 0) {
                std::cerr << "错误: 计数器值为负!" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
    });
    
    // 运行10秒
    std::this_thread::sleep_for(std::chrono::seconds(10));
    stop_test = true;
    
    // 等待所有线程结束
    for (auto& t : workers) {
        t.join();
    }
    reader.join();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    int final_count = counter.get();
    
    std::cout << "测试时长: " << duration.count() << " ms" << std::endl;
    std::cout << "最终计数值: " << final_count << std::endl;
    std::cout << "总递增次数: " << increments_done.load() << std::endl;
    std::cout << "总读取次数: " << reads_done.load() << std::endl;
    std::cout << "吞吐量: " << (increments_done * 1000.0 / duration.count()) << " 递增操作/秒" << std::endl;
    
    // 验证：最终计数应与总递增次数一致
    bool consistent = (final_count == increments_done);
    std::cout << "数据一致性: " << (consistent ? "✅ 一致" : "❌ 不一致") << "\n" << std::endl;
}

int main() {
    std::cout << "🎯 线程安全计数器全面压力测试套件" << std::endl;
    std::cout << "开始时间: " << __TIME__ << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    try {
        // 1. 基础压力测试
        ThreadSafeCounter counter1;
        basic_stress_test(counter1, 10, 10000, "基础压力测试");
        
        // 2. 混合读写压力测试
        ThreadSafeCounter counter2;
        mixed_read_write_stress_test(counter2, 5, 2000, 3, 5000);
        
        // 3. 极限压力测试
        ThreadSafeCounter counter3;
        extreme_stress_test(counter3);
        
        // 4. 性能对比测试
        performance_comparison_test();
        
        // 5. 长时间稳定性测试
        long_running_stability_test();
        
        std::cout << "🎉 所有压力测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ 未知错误导致测试失败" << std::endl;
        return 1;
    }
    
    return 0;
}