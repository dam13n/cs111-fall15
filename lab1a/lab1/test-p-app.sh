#! /bin/bash

# UCLA CS 111 Lab 1 - Personal test

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
true

g++ -c foo.c

: : :

cat < /etc/passwd | tr a-z A-Z | sort -u  || echo sort failed!

a b<c > d
EOF

cat >test.exp <<'EOF'
# 1
  true
# 2
  g++ -c foo.c
# 3
  : : :
# 4
      cat</etc/passwd \
    |
      tr a-z A-Z \
    |
      sort -u \
  ||
    echo sort failed!
# 5
  a b<c>d
EOF

../timetrash -p test.sh >test.out 2>test.err || exit

diff -u test.exp test.out || exit
test ! -s test.err || {
  cat test.err
  exit 1
}

) || exit

rm -fr "$tmp"
