#!/bin/bash
cmd="$1"
wd=$(dirname $0)
shift

case $(uname -s) in
	Linux)
		echo 64 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

		modprobe uio_pci_generic

		lspci_n=$(lspci -n)
		for busid in $(echo "${lspci_n}" | awk '{ if ($2 == "0401:" || $2 == "0403:") print $1 }') ; do
			echo "${lspci_n}" | grep "^${busid}\s" | awk '{ print $3 }' | tr ':' ' ' \
				> /sys/bus/pci/drivers/uio_pci_generic/new_id || return 2
		done
	;;
esac

LD_PRELOAD=./rumposs.so exec "$cmd" "$@"
