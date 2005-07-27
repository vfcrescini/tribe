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
  am[${i}]="${line}"
  i=$[i + 1]
done < ${m_file}
sm=${i}

i=0
while read line; do
  af[${i}]="${line}"
  i=$[i + 1]
done < ${f_file}
sf=${i}

# cleanup
rm -f ${m_file} ${f_file}

# now check occurences in M that is not in F
i=0
while [ ${i} -lt ${sm} ]; do
  j=0
  match=0
  while [ ${j} -lt ${sf} ]; do
    if [ -n "${af[${j}]}" ]; then
      if [ "${am[${i}]}" = "${af[${j}]}" ]; then
        unset af[${j}]
        match=1
        break
      fi
    fi
    j=$[${j} + 1]
  done
  if [ ${match} -eq 0 ]; then
    echo "${am[${i}]}"
  fi
  i=$[${i} + 1]
done

exit 0
