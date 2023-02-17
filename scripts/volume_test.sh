#! /bin/bash

cd ../build/converters
# -i "/Users/jileihao/data/_snap/_4d/img4d_example.nii.gz" \

# -i "/Users/jileihao/data/avrspt/bavcta008/image/bavcta008_baseline.nii.gz" \
# -oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/image_rs40_bavcta008_%02d.vti"

# -i "/Users/jileihao/data/avrspt/bavcta008/image/seg4d_bavcta008.nii.gz"
# -oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/seg_rs40_bavcta008_%02d.vti"

./VolumeGenerator \
-i "/Users/jileihao/data/avrspt/bavcta008/image/seg4d_bavcta008.nii.gz" \
-oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/seg_rs40_bavcta008_%02d.vti" \
-rs 0.4 \
-seg
