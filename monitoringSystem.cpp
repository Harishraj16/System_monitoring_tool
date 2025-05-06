import java.util.ArrayList;
import java.util.List;
import java.util.Random;

interface Notification {
    void send(String message);
}

class EmailNotification implements Notification {
    @Override
    public void send(String message) {
        System.out.println("Email Alert: " + message);
    }
}

class SMSNotification implements Notification {
    @Override
    public void send(String message) {
        System.out.println("SMS Alert: " + message);
    }
}

class EventManager {
    private final List<Notification> observers = new ArrayList<>();

    public void subscribe(Notification observer) {
        observers.add(observer);
    }

    public void notifyObservers(String message) {
        for (Notification observer : observers) {
            observer.send(message);
        }
    }
}

class Metrics {
    private final Random rand = new Random();

    public int getCPUUsage() {
        return rand.nextInt(100);
    }

    public int getMemoryUsage() {
        return rand.nextInt(100);
    }
}

class MonitoringSystem implements Runnable {
    private final Metrics metrics = new Metrics();
    private final EventManager eventManager;
    private final int cpuThreshold;
    private final int memoryThreshold;

    public MonitoringSystem(EventManager eventManager, int cpuThreshold, int memoryThreshold) {
        this.eventManager = eventManager;
        this.cpuThreshold = cpuThreshold;
        this.memoryThreshold = memoryThreshold;
    }

    @Override
    public void run() {
        while (true) {
            int cpuUsage = metrics.getCPUUsage();
            int memoryUsage = metrics.getMemoryUsage();

            if (cpuUsage > cpuThreshold) {
                eventManager.notifyObservers("High CPU Usage: " + cpuUsage + "%");
            }

            if (memoryUsage > memoryThreshold) {
                eventManager.notifyObservers("High Memory Usage: " + memoryUsage + "%");
            }

            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}

public class Main {
    public static void main(String[] args) {
        EventManager eventManager = new EventManager();
        eventManager.subscribe(new EmailNotification());
        eventManager.subscribe(new SMSNotification());

        MonitoringSystem monitoringSystem = new MonitoringSystem(eventManager, 75, 80);
        Thread monitoringThread = new Thread(monitoringSystem);
        monitoringThread.start();
    }
} 