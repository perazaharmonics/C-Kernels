# Linux-Sys-Programming
Linux System Programming 

#!/bin/bash
# Script to save kernel module and driver information before Leapp upgrade

BACKUP_DIR="/root/driver-backup-$(date +%Y%m%d_%H%M%S)"
KERNEL_VERSION="$(uname -r)"

echo "Creating backup directory: $BACKUP_DIR"
mkdir -p "$BACKUP_DIR" || { echo "❌ Failed to create $BACKUP_DIR"; exit 1; }

echo "Saving loaded kernel modules..."
lsmod | sort > "$BACKUP_DIR/lsmod.txt"

echo "Saving installed kernel modules (.ko files)..."
find /lib/modules/"$KERNEL_VERSION"/kernel -type f -name '*.ko*' | sort > "$BACKUP_DIR/kernel-drivers.txt"

echo "Saving list of kernel-related RPMs..."
rpm -qa | grep -Ei 'kmod|kernel.*(core|modules|extra|devel|tools)' | sort > "$BACKUP_DIR/kernel-packages.txt"

echo "Saving DKMS module status (if any)..."
command -v dkms &> /dev/null && dkms status > "$BACKUP_DIR/dkms-status.txt" 2>/dev/null || echo "DKMS not installed" > "$BACKUP_DIR/dkms-status.txt"

echo "Saving PCI devices with driver bindings..."
lspci -k > "$BACKUP_DIR/lspci.txt"

echo "Saving manually installed/unowned kernel modules..."
find /lib/modules/"$KERNEL_VERSION" -name '*.ko' | while read -r f; do rpm -qf "$f" &>/dev/null || echo "$f"; done > "$BACKUP_DIR/unowned-modules.txt"

echo
echo "✅ Driver and module information saved in: $BACKUP_DIR"
echo "You can diff these files after the upgrade to identify missing or changed drivers."