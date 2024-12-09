#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

// Notification interface and implementations
class Notification {
public:
    virtual void send(const string& message) = 0;
    virtual ~Notification() = default;
};

class EmailNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "Email Alert: " << message << endl;
    }
};

class SMSNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "SMS Alert: " << message << endl;
    }
};

// NotificationType Enum
enum class NotificationType {
    EMAIL,
    SMS
};

// NotificationCreator Factory
class NotificationCreator {
public:
    shared_ptr<Notification> create(NotificationType type) {
        if (type == NotificationType::EMAIL)
            return make_shared<EmailNotification>();
        else if (type == NotificationType::SMS)
            return make_shared<SMSNotification>();
        else
            return nullptr;
    }
};

// EventManager for managing subscribers
class EventManager {
private:
    vector<shared_ptr<Notification>> subscribers;

public:
    void addSubscriber(shared_ptr<Notification> subscriber) {
        subscribers.push_back(subscriber);
    }

    void notifySubscribers(const string& message) {
        for (auto& subscriber : subscribers)
            subscriber->send(message);
    }
};

// Function to execute a system command and get the output
std::string executeCommand(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        std::cerr << "popen failed!" << std::endl;
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

// SystemMetrics class to fetch system data
class SystemMetrics {
public:
    int getCPUUsage() {
        std::string cpuUsage = executeCommand("top -bn1 | grep 'Cpu(s)' | awk '{print $2 + $4}'");
        return std::stoi(cpuUsage);  // Convert the output string to an integer
    }

    int getMemoryUsage() {
        std::string memoryUsage = executeCommand("free | grep Mem | awk '{print $3/$2 * 100.0}'");
        return std::stoi(memoryUsage);  // Convert the output string to an integer
    }
};

// MonitoringSystem class to monitor system metrics and send alerts
class MonitoringSystem {
private:
    SystemMetrics metrics;
    shared_ptr<EventManager> eventManager;
    int cpuLimit;
    int memoryLimit;

public:
    MonitoringSystem(shared_ptr<EventManager> manager, int cpuThreshold, int memoryThreshold)
        : eventManager(manager), cpuLimit(cpuThreshold), memoryLimit(memoryThreshold) {}

    void monitor() {
        while (true) {
            int currentCPU = metrics.getCPUUsage();
            int currentMemory = metrics.getMemoryUsage();

            if (currentCPU > cpuLimit) {
                eventManager->notifySubscribers("High CPU Usage: " + to_string(currentCPU) + "%");
            }

            if (currentMemory > memoryLimit) {
                eventManager->notifySubscribers("High Memory Usage: " + to_string(currentMemory) + "%");
            }

            this_thread::sleep_for(chrono::seconds(2));  // Delay between checks
        }
    }
};

// Main function to initialize the system and start monitoring
int main() {
    // Create an event manager and notification factory
    auto eventManager = make_shared<EventManager>();
    NotificationCreator creator;

    // Add email and SMS notification subscribers
    eventManager->addSubscriber(creator.create(NotificationType::EMAIL));
    eventManager->addSubscriber(creator.create(NotificationType::SMS));

    // Initialize the monitoring system with thresholds (e.g., 75% CPU, 80% memory)
    MonitoringSystem system(eventManager, 75, 80);

    // Start monitoring in a separate thread
    thread monitoringThread(&MonitoringSystem::monitor, &system);
    monitoringThread.join();

    return 0;
}
