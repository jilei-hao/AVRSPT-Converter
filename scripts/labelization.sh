#! /bin/bash

labelizer=/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/common/TestCommon

# fnimg4d="/Users/jileihao/data/avrspt/tav48/img4d_tav48_LAS.nii.gz"

# process tp 2 -> 1 3 4 5
# tpstr="seg02"
# rsrate="2"
# rsstr="${rsrate}00"
# folder=/Users/jileihao/data/avrspt/tav48/smoothed/
# fnimg=$folder/${tpstr}.nii.gz
# fnimg_sm=${folder}/${tpstr}_sm.nii.gz
# fnimg_rssm=${folder}/${tpstr}_rs${rsstr}.nii.gz
# fnmesh=$folder/${tpstr}.vtk

# c3d -verbose $fnimg \
# -smooth-multilabel 1vox "*" \
# -o $fnimg_sm

# c3d -int 0 -verbose $fnimg_sm \
# -resample ${rsstr}% \
# -smooth-multilabel ${rsrate}vox "*" \
# -o $fnimg_rssm

# $labelizer $fnimg_rssm $fnmesh

# PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
# python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
# $fnimg4d \
# $fnimg_sm \
# "SYS_02" \
# "2" \
# "1;3;4;5" \
# "/Users/jileihao/data/avrspt/tav48/propagation_new/SYS_02" \
# "/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
# -add_mesh "labelized_mesh" $fnmesh


# process tp 9
# tpstr="seg09"
# rsrate="2"
# rsstr="${rsrate}00"
# folder=/Users/jileihao/data/avrspt/tav48/smoothed/
# fnimg=$folder/${tpstr}.nii.gz
# fnimg_sm=${folder}/${tpstr}_sm.nii.gz
# fnimg_rssm=${folder}/${tpstr}_rs${rsstr}.nii.gz
# fnmesh=$folder/${tpstr}.vtk

# c3d -verbose $fnimg \
# -smooth-multilabel 0.8vox "*" \
# -o $fnimg_sm

# c3d -int 0 -verbose $fnimg_sm \
# -resample ${rsstr}% \
# -smooth-multilabel 1.5vox "*" \
# -o $fnimg_rssm

# $labelizer $fnimg_rssm $fnmesh

# PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
# python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
# $fnimg4d \
# $fnimg_sm \
# "DIAS_09" \
# "9" \
# "6;7;8;10;11;12;13;14" \
# "/Users/jileihao/data/avrspt/tav48/propagation_new/DIAS_09" \
# "/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
# -add_mesh "labelized_mesh" $fnmesh


###############
# cta
###############
fnimg4d="/Users/jileihao/data/avrspt/bavcta008/image/bavcta008_baseline.nii.gz"

# process tp 2
tpstr="seg02"
rsrate="2"
rsstr="${rsrate}00"
folder=/Users/jileihao/data/avrspt/bavcta008/smoothing/
fnimg=$folder/${tpstr}.nii.gz
fnmesh=$folder/${tpstr}.vtk

$labelizer $fnimg $fnmesh

PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
$fnimg4d \
$fnimg \
"bavcta008-SYS" \
"2" \
"1;3;4;5;6;7;8;9;10;11;12;13;14" \
"/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-SYS" \
"/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
-warp_only \
-add_mesh "onemesh" $fnmesh


# process tp 19
tpstr="seg19"
rsrate="2"
rsstr="${rsrate}00"
folder=/Users/jileihao/data/avrspt/bavcta008/smoothing/
fnimg=$folder/${tpstr}.nii.gz
fnmesh=$folder/${tpstr}.vtk

$labelizer $fnimg $fnmesh

PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
$fnimg4d \
$fnimg \
"bavcta008-DIAS" \
"19" \
"15;16;17;18;20" \
"/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-DIAS" \
"/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
-warp_only \
-add_mesh "onemesh" $fnmesh