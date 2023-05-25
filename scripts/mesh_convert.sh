#! /bin/bash

vtk2vtp="/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/VTKToVTP"

dir_input="/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-SYS/mesh"
dir_output="/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/onemesh"

ptn_original="seg_bavcta008-SYS_onemesh"
ptn_new="onemesh"

mkdir ${dir_output}
mkdir ${dir_output}/vtp

cd ${dir_input}

for f in ${ptn_original}*.vtk; do
  cp ${f} ${dir_output}/${f}
done

cd ${dir_output}

for f in ${ptn_original}*.vtk; do
  mv ${f} ${f/${ptn_original}/${ptn_new}}
done

for f in ${ptn_new}*.vtk; do
  ${vtk2vtp} ${f} ./vtp/${f/.vtk/.vtp} dummy 0
done


vtk2vtp="/Users/jileihao/dev/avrspt-dev/AVRSPT-Converter/build/bin/VTKToVTP"

dir_input="/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/bavcta008-DIAS/mesh"
dir_output="/Users/jileihao/data/avrspt/bavcta008/propagation/bavcta008/onemesh"

ptn_original="seg_bavcta008-DIAS_onemesh"
ptn_new="onemesh"

mkdir ${dir_output}
mkdir ${dir_output}/vtp

cd ${dir_input}

for f in ${ptn_original}*.vtk; do
  cp ${f} ${dir_output}/${f}
done

cd ${dir_output}

for f in ${ptn_original}*.vtk; do
  mv ${f} ${f/${ptn_original}/${ptn_new}}
done

for f in ${ptn_new}*.vtk; do
  ${vtk2vtp} ${f} ./vtp/${f/.vtk/.vtp} dummy 0
done