#!/bin/bash
cmd="$1"
wd=$(dirname $0)
shift

case $(uname -s) in
	Linux)
		echo 64 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

		modprobe uio_pci_generic

		cat ${wd}/pcidevs/{auich,hdaudio} | while read vendor device ; do
			echo "${vendor} ${device}" > /sys/bus/pci/drivers/uio_pci_generic/new_id
		done
	;;
esac

LD_PRELOAD=./rumposs.so exec $cmd $@
