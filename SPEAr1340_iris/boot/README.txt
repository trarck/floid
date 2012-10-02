The run.img file is made using the mkimage command:
>mkimage -n "Demo Run Script" -A arm  -T script -C none  -d run.txt run.img

It’s possible to remove the header information with the dd command:
>dd if=run.img of=run.txt bs=72 skip=1

