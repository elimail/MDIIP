#/bin/bash
seed=$1

dir="Instancesv3_ALL"
#max_iter_vns=0.04
#max_iter_disturb=0.01
#max_iter=10000
max_iter_vns=0.05
max_iter_disturb=0.01
max_iter=100000

ARGS="${max_iter_vns} ${max_iter_disturb} ${max_iter}"

instanceList=$(ls ${dir})

for instance in ${instanceList}; do
    sumarizedOut="Out_${instance}"
    detailedOut=out_${instance}_${seed}.out
    rm -rf ${detailedOut}
    echo "./BIRP_b2 ${dir}/${instance} ${seed} ${ARGS} > ${detailedOut}"
    ./BIRP_b2 ${dir}/${instance} ${seed} ${ARGS} > ${detailedOut}
    quality=`tail -1 ${detailedOut}`
    echo "${quality}" >> ${sumarizedOut}
done


