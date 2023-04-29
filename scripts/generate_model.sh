#! /bin/bash

cd /Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/converters

fnimg=/Users/jileihao/data/avrspt/case230424/seg_230424_00.nii.gz
fnmesh_ref=/Users/jileihao/data/avrspt/case230424/mdl/mdl_230424_00.vtk
fnmesh_dc=/Users/jileihao/data/avrspt/case230424/mdl/mdl_230424_dc50_00.vtk

dc=50

## DIAS RUN 19 -> 15;16;17;18;20

vtklevelset -pl \
$fnimg \
$fnmesh_ref \
1

./ModelProcessor \
$fnmesh_ref \
$fnmesh_dc \
$dc \
nr

# PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
# python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008_baseline.nii.gz" \
# "/Users/jileihao/data/avrspt/bavcta008/seg/seg19_bavcta008_relabeled_JD.nii.gz" \
# "bavcta008-DIAS" \
# "19" \
# "15;16;17;18;20" \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-DIAS" \
# "/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
# -warp_only \
# -add_mesh "relabeled_mesh" $fnmesh_dc

## SYSTOLE RUN 2 -> 1;3;4;5;6;7;8;9;10;11;12;13;14

# fnimg=/Users/jileihao/data/avrspt/bavcta008/seg/seg02_bacvta008_relabeled_JD.nii.gz
# fnmesh_ref=/Users/jileihao/data/avrspt/bavcta008/mesh/dc00/seg02_bavcta008_relabeled.vtk
# fnmesh_dc=/Users/jileihao/data/avrspt/bavcta008/sandbox/mesh_bavcta008_snap_dc75_02.vtk

# vtklevelset -pl \
# $fnimg \
# $fnmesh_ref \
# 1

# ./ModelProcessor \
# $fnmesh_ref \
# $fnmesh_dc \
# $dc \
# nr


# PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
# python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008_baseline.nii.gz" \
# $fnimg \
# "bavcta008-SYS" \
# "2" \
# "1;3;4;5;6;7;8;9;10;11;12;13;14" \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-SYS" \
# "/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
# -warp_only \
# -add_mesh "relabeled_mesh" $fnmesh_dc


