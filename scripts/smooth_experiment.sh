#! /bin/bash

c3d="c3d"

$c3d -int 0 -verbose seg.nii.gz \
-threshold 1 inf 1 0  \
-smooth-multilabel 1vox "*" \
-resample 200% -smooth-multilabel 2vox "*" \
-resample 200% -smooth-multilabel 4vox "*" \
-resample 200% -smooth-multilabel 8vox "*" \
-o seg_rs800_sm.nii.gz
