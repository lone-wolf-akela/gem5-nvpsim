rm m5out/batch_res.csv

for energy in {1..50}
do
	for cap in {1..50}
	do
		echo "========================================================="
		echo "cap: $cap; energy: $energy"
	 	build/ARM/gem5.debug configs/example/LiuRuoyang_engy_vdev_batch.py $cap $energy
	done
done

