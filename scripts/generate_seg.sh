#! /bin/bash

cd /Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/converters
# -i "/Users/jileihao/data/_snap/_4d/img4d_example.nii.gz" \

# -i "/Users/jileihao/data/avrspt/bavcta008/image/bavcta008_baseline.nii.gz" \
# -oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/image_rs40_bavcta008_%02d.vti"

# -i "/Users/jileihao/data/avrspt/bavcta008/image/seg4d_bavcta008.nii.gz"
# -oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/seg_rs40_bavcta008_%02d.vti"

# ./VolumeGenerator \
# -i "/Users/jileihao/data/avrspt/bavcta008/image/seg4d_bavcta008.nii.gz" \
# -oo "/Users/jileihao/data/avrspt/bavcta008/image/volgen/seg_rs40_bavcta008_%02d.vti" \
# -rs 0.4 \
# -seg


# Generating relabeld data using rs40 setting
# ./VolumeGenerator \
# -i "/Users/jileihao/data/avrspt/case230424/img_230424_00.nii.gz" \
# -oo "/Users/jileihao/data/avrspt/case230424/vol/vol_230424_%02d.vti" \
# -rs 1.0 \


./VolumeGenerator \
-i "/Users/jileihao/data/avrspt/case230424/seg_230424_00.nii.gz" \
-oo "/Users/jileihao/data/avrspt/case230424/seg/seg_230424_%02d.vti" \
-rs 1.0 \
-seg

