Before creating your root.tar.gz, uncomment the "mount mtd partitions"
mount-lines in init.rc. Otherwise, the NAND partitions will not be mounted and
you can't run off NAND.


