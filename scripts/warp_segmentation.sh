

# img4d
# seg
# tag
# tpr
# tpt(; separated)
# outdir
# config

## DIAS RUN 19 -> 15;16;17;18;20

# PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
# python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008_baseline.nii.gz" \
# "/Users/jileihao/data/avrspt/bavcta008/image/seg19_bavcta008_relabeled_JD.nii.gz" \
# "bavcta008-DIAS" \
# "19" \
# "15;16;17;18;20" \
# "/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-DIAS" \
# "/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
# -warp_only

## SYS RUN 2 -> 1;3;4;5;6;7;8;9;10;11;12;13;14
PYTHONPATH="/Users/jileihao/dev/propagation-dev/segmentation-propagation/src" \
python3 /Users/jileihao/dev/propagation-dev/segmentation-propagation/propagation.py \
"/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008_baseline.nii.gz" \
"/Users/jileihao/data/avrspt/bavcta008/image/seg02_bacvta008_relabeled_JD.nii.gz" \
"bavcta008-SYS" \
"2" \
"1;3;4;5;6;7;8;9;10;11;12;13;14" \
"/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-SYS" \
"/Users/jileihao/dev/propagation-dev/segmentation-propagation/config.json" \
-warp_only
