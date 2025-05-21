#!/bin/bash

# Thresholds
CPU_THRESHOLD=1
MEM_THRESHOLD=1
DISK_THRESHOLD=1

# Email settings
TO_EMAIL="harishtemp23@gmail.com"
SUBJECT="🚨 System Alert from Docker Monitor"

# Log file
LOGFILE="/var/log/system_monitor.log"
[ -w "$LOGFILE" ] || LOGFILE="./system_monitor.log"

log() {
  echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOGFILE"
}

send_email() {
  echo "$1" | mail -s "$SUBJECT" "$TO_EMAIL"
}

trap "echo 'Exiting...'; exit 0" SIGINT

while true; do
  CPU=$(top -bn1 | grep "Cpu(s)" | awk '{print 100 - $8}' | awk -F. '{print $1}')
  MEM=$(free | grep Mem | awk '{print int($3/$2 * 100)}')
  DISK=$(df / | tail -1 | awk '{print $5}' | sed 's/%//')

  log "CPU: ${CPU}%, Memory: ${MEM}%, Disk: ${DISK}%"

  ALERT_MSG=""

  if [ "$CPU" -gt "$CPU_THRESHOLD" ]; then
    ALERT_MSG+="CPU is high (${CPU}%)\n"
  fi

  if [ "$MEM" -gt "$MEM_THRESHOLD" ]; then
    ALERT_MSG+="Memory is high (${MEM}%)\n"
  fi

  if [ "$DISK" -gt "$DISK_THRESHOLD" ]; then
    ALERT_MSG+="Disk usage is high (${DISK}%)\n"
  fi

  if [ -n "$ALERT_MSG" ]; then
    log "⚠️ ALERT:\n$ALERT_MSG"
    send_email "$ALERT_MSG"
  fi

  sleep 60
done
