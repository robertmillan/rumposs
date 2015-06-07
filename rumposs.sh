#!/bin/sh
cmd="$1"
shift

case $(uname -s) in
	Linux)
		echo 64 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

		modprobe uio_pci_generic

		## auich
		echo "8086 2415" > /sys/bus/pci/drivers/uio_pci_generic/new_id

		## hdaudio
		echo "8086 2668" > /sys/bus/pci/drivers/uio_pci_generic/new_id
		echo "8086 27d8" > /sys/bus/pci/drivers/uio_pci_generic/new_id
	;;
esac

LD_PRELOAD=./rumposs.so exec $cmd $@
