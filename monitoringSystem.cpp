#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

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

class EventManager {
private:
    vector<Notification*> observers;

public:
    void subscribe(Notification* observer) {
        observers.push_back(observer);
    }

    void notify(const string& message) {
        for (auto observer : observers) {
            observer->send(message);
        }
    }
};

class Metrics {
public:
    int getCPUUsage() {
        return rand() % 100;
    }

    int getMemoryUsage() {
        return rand() % 100;
    }
};

class MonitoringSystem {
private:
    Metrics metrics;
    EventManager& eventManager;
    int cpuThreshold;
    int memoryThreshold;

public:
    MonitoringSystem(EventManager& em, int cpuTh, int memTh)
        : eventManager(em), cpuThreshold(cpuTh), memoryThreshold(memTh) {}

    void monitor() {
        while (true) {
            int cpuUsage = metrics.getCPUUsage();
            int memoryUsage = metrics.getMemoryUsage();

            if (cpuUsage > cpuThreshold) {
                eventManager.notify("High CPU Usage: " + to_string(cpuUsage) + "%");
            }

            if (memoryUsage > memoryThreshold) {
                eventManager.notify("High Memory Usage: " + to_string(memoryUsage) + "%");
            }

            this_thread::sleep_for(chrono::seconds(2));
        }
    }
};

int main() {
    EventManager eventManager;
    EmailNotification emailNotification;
    SMSNotification smsNotification;

    eventManager.subscribe(&emailNotification);
    eventManager.subscribe(&smsNotification);

    MonitoringSystem monitoringSystem(eventManager, 75, 80);

    thread monitoringThread(&MonitoringSystem::monitor, &monitoringSystem);
    monitoringThread.join();

    return 0;
}
