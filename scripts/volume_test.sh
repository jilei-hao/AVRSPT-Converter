#! /bin/bash

cd ../build/converters
# -i "/Users/jileihao/data/_snap/_4d/img4d_example.nii.gz" \

./VolumeGenerator \
-i "/Users/jileihao/data/avrspt/bavcta008/image/bavcta008_baseline.nii.gz" \
-rs 0.4 \
-oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/image_rs40_bavcta008_%02d.vti"