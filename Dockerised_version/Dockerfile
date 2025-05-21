FROM debian:bullseye-slim

# Install required tools
RUN apt update && \
    apt install -y procps bc mailutils msmtp && \
    apt clean

# Configure mailer
COPY msmtprc /etc/msmtprc
RUN chmod 600 /etc/msmtprc

# Copy monitoring script
COPY system_monitor.sh /monitor.sh
RUN chmod +x /monitor.sh

# Create logs directory
RUN mkdir -p /var/log && touch /var/log/system_monitor.log /var/log/msmtp.log

CMD ["/monitor.sh"]
