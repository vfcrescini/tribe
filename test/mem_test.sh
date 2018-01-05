#!/bin/sh

if [ ${#} -ne 1 ] || ! [ -r ${1} ]; then
  echo "usage: memtest <tribe-output-log>"
  exit 1
fi

out="${1}"
m_file=".${$}_M"
f_file=".${$}_F"

# separate the log contents into different files
grep "^TBE_MEM M" ${out} | sort | cut -d" " -f3 > ${m_file}
grep "^TBE_MEM F" ${out} | sort | cut -d" " -f3 > ${f_file}

# load the log contents into arrays
i=0
while read line; do
  eval "am_${i}=\"${line}\""
  i=$((i + 1))
done < ${m_file}
sm=${i}

i=0
while read line; do
  eval "af_${i}=\"${line}\""
  i=$((i + 1))
done < ${f_file}
sf=${i}

# cleanup
rm -f ${m_file} ${f_file}

# now check occurences in M that is not in F
i=0
while [ ${i} -lt ${sm} ]; do
  j=0
  match=0
  eval "am_i=\${am_${i}}"
  while [ ${j} -lt ${sf} ]; do
    eval "af_j=\${af_${j}}"
    if [ -n "${af_j}" ]; then
      if [ "${am_i}" = "${af_j}" ]; then
        eval "af_${j}=\"\""
        match=1
        break
      fi
    fi
    j=$((${j} + 1))
  done
  if [ ${match} -eq 0 ]; then
    echo "M ${am_i}"
  fi
  i=$((${i} + 1))
done

# also show the remaining F's, if any
i=0
while [ ${i} -lt ${sf} ]; do
  eval "af_i=\${af_${i}}"
  if [ -n "${af_i}" ]; then
    echo "F ${af_i}"
  fi
  i=$((${i} + 1))
done

exit 0
