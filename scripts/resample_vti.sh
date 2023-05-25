#! /bin/bash

vti2nii=/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/vti2nii
nii2vti=/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/nii2vti
c3d=c3d

dir_data=/Users/jileihao/data/avrspt/dist/bavcta008-tav48/echo/seg
dir_work=${dir_data}/work
c3d_int="-int 0"
rs_rate="70"
rs_str="rs${rs_rate}"
old_rs_str="rs100"

mkdir $dir_work

cd $dir_data

for f in *.vti; do
  $vti2nii $f work/${f/.vti/.nii.gz}
done

cd work

for f in *.nii.gz; do
  $c3d $c3d_int $f -resample ${rs_rate}% -o ${f/${old_rs_str}/${rs_str}}
done

for f in *${rs_str}*.nii.gz; do
  $nii2vti $f ../${f/.nii.gz/.vti}
done
