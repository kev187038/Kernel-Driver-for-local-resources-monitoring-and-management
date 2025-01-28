echo "Removing all compiled files from kernel module and GUI"

make clean

sudo rmmod SRMC_Module.ko

echo "Removal complete!"