#! /bin/bash

c3d="c3d"
vtklevelset="vtklevelset"
decimate="/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/QuadricDecimate"
taubin="/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/TaubinSmooth"

data_root="/Users/jileihao/data/avrspt/smoothing"

$c3d -int 0 -verbose $data_root/seg.nii.gz \
-threshold 1 inf 1 0  \
-smooth-multilabel 1vox "*" \
-resample 200% -smooth-multilabel 2vox "*" \
-resample 200% -smooth-multilabel 4vox "*" \
-o $data_root/seg_rs400_sm.nii.gz

$vtklevelset $data_root/seg_rs400_sm.nii.gz $data_root/seg.vtk 1

$decimate $data_root/seg.vtk 0.2 $data_root/seg_dc20.vtk

$taubin $data_root/seg_dc20.vtk 100 0.01 0 $data_root/seg_dc20.vtk

$decimate $data_root/seg_dc20.vtk 0.9375 $data_root/seg_dc95.vtk

$taubin $data_root/seg_dc95.vtk 100 0.05 0 $data_root/seg_dc95.vtk